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

namespace PeanutGL {
    /**
     * @brief Base class for all resources.
     */
    class Resource {
      private:
        template < typename Self > auto unload_inner( this Self&& self ) noexcept -> void {
            std::forward< Self >( self ).Unload();
        }

        template < typename Self, typename Parent > auto load_inner( this Self&& self ) noexcept -> bool {
            //            self.loaded = true;
            return std::forward< Self >( self ).Load();
        }

      protected:
        std::string resourceId{};
        bool loaded{ false };

      public:
        Resource();

        /**
         * @brief Virtual destructor for proper cleanup.
         */
        virtual ~Resource();

        Resource( const Resource& )                = default;
        Resource( Resource&& ) noexcept            = default;
        Resource& operator=( const Resource& )     = default;
        Resource& operator=( Resource&& ) noexcept = default;

        /**
         * @brief Constructor with a resource ID.
         * @param id The unique identifier for the resource.
         */
        explicit Resource( const std::string_view resource_id )
            : resourceId{ resource_id } {
        }

        /**
         * @brief Get the resource ID.
         * @return The resource ID.
         */
        constexpr auto GetId() const noexcept -> const std::string& {
            return resourceId;
        }

        /**
         * @brief Check if the resource is loaded.
         * @return True if the resource is loaded, false otherwise.
         */
        constexpr auto IsLoaded() const noexcept -> bool {
            return loaded;
        }

        /**
         * @brief Load the resource.
         * @return True if the resource was loaded successfully, false otherwise.
         */
        virtual auto Load() noexcept -> bool = 0;

        /**
         * @brief Unload the resource.
         */
        virtual auto Unload() noexcept -> void = 0;
    };

    template < typename T >
    concept IsResourceBase = std::is_base_of_v< Resource, T >;

    /**
     * @brief Template class for resource handles.
     * @tparam T The type of resource.
     */
    template < typename T >
        requires IsResourceBase< T >
    class ResourceHandle {
      private:
        using ResourceManagerHandle = class ResourceManager*;

        using derived_resource  = T;
        using resource_ptr      = derived_resource*;
        using resource_refrence = derived_resource&;

        std::string resourceId{};
        ResourceManagerHandle resourceManager{ nullptr };

      public:
        /**
         * @brief Default constructor.
         */
        ResourceHandle() = default;

        /**
         * @brief Constructor with a resource ID and resource manager.
         * @param id The resource ID.
         * @param manager The resource manager.
         */
        ResourceHandle( const std::string_view resource_id, class ResourceManager* manager )
            : resourceId( resource_id ), resourceManager( manager ) {
        }

        /**
         * @brief Get the resource.
         * @return A pointer to the resource, or nullptr if not found.
         */
        constexpr auto Get() const noexcept -> resource_ptr;

        /**
         * @brief Check if the handle is valid.
         * @return True if the handle is valid, false otherwise.
         */
        constexpr auto IsValid() const noexcept -> bool;

        /**
         * @brief Get the resource ID.
         * @return The resource ID.
         */
        auto GetId() const -> std::string_view {
            return { resourceId };
        }

        /**
         * @brief Convenience operator for accessing the resource.
         * @return A pointer to the resource.
         */
        auto operator->() const -> resource_ptr {
            return Get();
        }

        /**
         * @brief Convenience operator for dereferencing the resource.
         * @return A reference to the resource.
         */
        auto operator*() const -> resource_refrence {
            return *Get();
        }

        /**
         * @brief Convenience operator for checking if the handle is valid.
         * @return True if the handle is valid, false otherwise.
         */
        explicit operator bool() const {
            return IsValid();
        }
    };

} // namespace PeanutGL
