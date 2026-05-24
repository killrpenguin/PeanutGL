/*
 * Copyright (c) 2026 David McFarland
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "ResourceBase.hpp"
#include "readerwriterqueue.h"

#include <condition_variable>

#include <glad/gl.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <thread>
#include <typeindex>
#include <unordered_map>

namespace PeanutGL {

    /**
     * @brief Class for managing resources.
     *
     * This class implements the resource management system.
     */
    class ResourceManager {
        struct Deleter {
            auto operator()( Resource* resource ) noexcept {
                resource->Unload();
            }
        };

      private:
        std::unordered_map< std::type_index, std::unordered_map< std::string, std::unique_ptr< Resource > > > resources;

      public:
        /**
         * @brief Default constructor.
         */
        ResourceManager() noexcept = default;

        // We delete the copy constructors because the keys of the inner map can't be copied.
        ResourceManager( const ResourceManager& )                = delete;
        ResourceManager( ResourceManager&& ) noexcept            = default;
        ResourceManager& operator=( const ResourceManager& )     = delete;
        ResourceManager& operator=( ResourceManager&& ) noexcept = default;

        /**
         * @brief Default destructor.
         */
        ~ResourceManager() = default;

        /**
         * @brief Load a resource.
         * @tparam T The type of resource.
         * @tparam Args The types of arguments to pass to the resource constructor.
         * @param identifier The resource ID.
         * @param args The arguments to pass to the resource constructor.
         * @return A handle to the resource.
         */
        template < typename T, typename... Args >
            requires IsResourceBase< T >
        auto LoadResource( const std::string& identifier, Args&&... args ) -> ResourceHandle< T > {
            auto& typeResources = resources[std::type_index( typeid( T ) )];

            auto iter = typeResources.find( identifier );

            if ( iter != typeResources.end() ) { return ResourceHandle< T >( identifier, this ); }

            std::unique_ptr< T > resource{ std::make_unique< T >( identifier, std::forward< Args >( args )... ) };

            if ( !resource->Load() ) { throw std::runtime_error( "Failed to load resource: " + identifier ); }

            typeResources[identifier] = std::move( resource );

            return ResourceHandle< T >( identifier, this );
        }

        /**
         * @brief Get a resource.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return A pointer to the resource, or nullptr if not found.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto GetResource( const std::string& identifier ) -> T* {
            auto typeIt = resources.find( std::type_index( typeid( T ) ) );
            if ( typeIt == resources.end() ) { return nullptr; }

            auto& typeResources = typeIt->second;
            auto resourceIt     = typeResources.find( identifier );
            if ( resourceIt == typeResources.end() ) { return nullptr; }

            return static_cast< T* >( resourceIt->second.get() );
        }

        /**
         * @brief Check if a resource exists.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return True if the resource exists, false otherwise.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto HasResource( const std::string& identifier ) noexcept -> bool {
            auto typeIt = resources.find( std::type_index( typeid( T ) ) );
            if ( typeIt == resources.end() ) { return false; }

            auto& typeResources = typeIt->second;
            return typeResources.contains( identifier );
        }

        /**
         * @brief Unload a resource.
         * @tparam T The type of resource.
         * @param identifier The resource ID.
         * @return True if the resource was unloaded, false otherwise.
         */
        template < typename T >
            requires IsResourceBase< T >
        auto UnloadResource( const std::string& identifier ) noexcept -> bool {
            auto typeIt = resources.find( std::type_index( typeid( T ) ) );
            if ( typeIt == resources.end() ) { return false; }

            auto& typeResources = typeIt->second;
            auto resourceIt     = typeResources.find( identifier );
            if ( resourceIt == typeResources.end() ) { return false; }

            resourceIt->second->Unload();
            typeResources.erase( resourceIt );
            return true;
        }

        /**
         * @brief Unload all resources.
         */
        auto UnloadAllResources() noexcept -> void {
            for ( auto& [type, type_resource] : resources ) {
                for ( auto& [_unused, resource] : type_resource ) {
                    resource->Unload();
                }
            }
        }
    };

    template < typename T >
        requires IsResourceBase< T >
    constexpr auto ResourceHandle< T >::Get() const noexcept -> resource_ptr {
        if ( resourceManager == nullptr ) { return nullptr; }
        return resourceManager->GetResource< T >( resourceId );
    }

    template < typename T >
        requires IsResourceBase< T >
    constexpr auto ResourceHandle< T >::IsValid() const noexcept -> bool {
        if ( resourceManager == nullptr ) { return false; }
        return resourceManager->HasResource< T >( resourceId );
    }

    class AsyncResourceManager {
        ResourceManager resourceManager{};
        std::jthread workerThread{};
        std::mutex queueMutex{};
        std::condition_variable condition;

        [[maybe_unused]] bool running{ false };
        // NOLINTNEXTLINE
        moodycamel::ReaderWriterQueue< std::function< void() > > taskQueue{ 10 };

        constexpr auto queue_empty() const noexcept -> bool {
            return taskQueue.peek() == nullptr;
        }

        constexpr auto WorkerThread() noexcept -> void {
            while ( running ) {
                std::function< void() > task{};

                {
                    std::unique_lock< std::mutex > lock{ queueMutex };

                    condition.wait( lock, [this]() { return !queue_empty() or !running; } );

                    if ( !running and queue_empty() ) { return; }

                    const bool succeeded{ taskQueue.try_dequeue( task ) };

                    assert(
                        succeeded && "MoodyCamel queue returned not empty but try_dequeue() did not return a value." );
                }

                task();
            }
        };

      public:
        AsyncResourceManager() noexcept {
            Start();
        };

        AsyncResourceManager( const AsyncResourceManager& )                = delete;
        AsyncResourceManager( AsyncResourceManager&& ) noexcept            = delete;
        AsyncResourceManager& operator=( const AsyncResourceManager& )     = delete;
        AsyncResourceManager& operator=( AsyncResourceManager&& ) noexcept = delete;

        ~AsyncResourceManager() noexcept {
            Stop();
        };

        constexpr auto Stop() noexcept -> void {
            {
                std::lock_guard< std::mutex > lock( queueMutex );
                running = false;
            }
            condition.notify_one();
        }

        constexpr auto Start() noexcept -> void {
            running      = true;
            workerThread = std::jthread( [this]() { WorkerThread(); } );
        }

        template < typename T >
            requires IsResourceBase< T >
        auto LoadAsync( const std::string& resourceId, std::function< void( ResourceHandle< T > ) > callback ) -> void {
            std::lock_guard< std::mutex > lock{ queueMutex };

            taskQueue.try_enqueue( [this, resourceId, callback]() {
                auto handle = resourceManager.LoadResource< T >( resourceId );
                callback( handle );
            } );

            condition.notify_one();
        }

        /**
         * @brief Poll the OS for changes to Resource files.
         * @return true if ready, false otherwise;
         **/
        auto Poll() const noexcept -> bool;
    };

} // namespace PeanutGL
