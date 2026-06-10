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

#include "Renderer.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>

namespace PeanutGL {

    namespace chrono = std::chrono;

    class PhysicsSystem {
      public:
        /**
         * @brief Default constructor.
         */
        PhysicsSystem() noexcept = default;

        // Constructor-based initialization replacing separate Initialize/Set* calls
        explicit PhysicsSystem( Renderer* _renderer ) {
            SetRenderer( _renderer );
            if ( !Initialize() ) { throw std::runtime_error( "PhysicsSystem: initialization failed" ); }
        }

        /**
         * @brief Copy/Move assignment operators and constructors are deleted for thread safety.
         */
        PhysicsSystem( const PhysicsSystem& )                = delete;
        PhysicsSystem( PhysicsSystem&& ) noexcept            = delete;
        PhysicsSystem& operator=( const PhysicsSystem& )     = delete;
        PhysicsSystem& operator=( PhysicsSystem&& ) noexcept = delete;

        /**
         * @brief Destructor for proper cleanup.
         */
        ~PhysicsSystem() = default;

        /**
         * @brief Update the physics system.
         * @param deltaTime The time elapsed since the last update.
         */
        void Update( [[maybe_unused]] chrono::milliseconds deltaTime ) {
            // TODO: Unimplemented.
        }

        /**
         * @brief Set the gravity of the physics world.
         * @param _gravity The gravity vector.
         */
        void SetGravity( const glm::vec3& _gravity );

        /**
         * @brief Get the gravity of the physics world.
         * @return The gravity vector.
         */
        [[nodiscard]] glm::vec3 GetGravity() const;

        /**
         * @brief Perform a raycast.
         * @param origin The origin of the ray.
         * @param direction The direction of the ray.
         * @param maxDistance The maximum distance of the ray.
         * @param hitPosition Output parameter for the hit position.
         * @param hitNormal Output parameter for the hit normal.
         * @param hitEntity Output parameter for the hit entity.
         * @return True if the ray hit something, false otherwise.
         */
        bool Raycast(
            const glm::vec3& origin, const glm::vec3& direction, float maxDistance, glm::vec3* hitPosition,
            glm::vec3* hitNormal, Entity** hitEntity ) const;

        /**
         * @brief Set the maximum number of objects that can be simulated on the GPU.
         * @param maxObjects The maximum number of objects.
         */
        void SetMaxGPUObjects( uint32_t maxObjects ) {
            maxGPUObjects = maxObjects;
        }

        /**
         * @brief Set the renderer to use during GPU acceleration.
         * @param _renderer The renderer.
         */
        void SetRenderer( Renderer* _renderer ) {
            renderer = _renderer;
        }

        /**
         * @brief Set the current camera position for geometry-relative ball checking.
         * @param _cameraPosition The current camera position.
         */
        void SetCameraPosition( const glm::vec3& _cameraPosition ) {
            cameraPosition = _cameraPosition;
        }

      private:
        /**
         * @brief Initialize the physics system (called by constructor).
         * @return True if initialization was successful, false otherwise.
         */
        bool Initialize();

        struct RigidBody {};

        // Rigid bodies
        mutable std::mutex rigidBodiesMutex; // Protect concurrent access to rigidBodies
        std::vector< std::unique_ptr< RigidBody > > rigidBodies;

        // Gravity
        glm::vec3 gravity = glm::vec3( 0.0F, -9.81F, 0.0F );

        // Whether the physics system is initialized
        bool initialized = false;

        // GPU acceleration
        std::uint32_t maxGPUObjects    = 1024;
        std::uint32_t maxGPUCollisions = 4096;
        Renderer* renderer             = nullptr;

        // Camera position for geometry-relative ball checking
        glm::vec3 cameraPosition{};
    };

} // namespace PeanutGL
