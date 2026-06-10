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

#include "ResourceManager.hpp"
#include "Utilities.hpp"

#include <glad/gl.h>

namespace PeanutGL {
    /**
     * @brief Class for representing an OpenGL Memory Barrier.
     *
     * This class represents a syncronization point between CPU and GPU memory.
     */
    class MemoryBarrier : public Resource {
      private:
        GLsync fence{};

      public:
        MemoryBarrier() noexcept = default;

        explicit MemoryBarrier( std::string_view identifier ) noexcept
            : Resource( identifier ) {
        }

        // Resources are stored as unique pointers in the resource manager.
        MemoryBarrier( const MemoryBarrier& )                = delete;
        MemoryBarrier( MemoryBarrier&& ) noexcept            = default;
        MemoryBarrier& operator=( const MemoryBarrier& )     = delete;
        MemoryBarrier& operator=( MemoryBarrier&& ) noexcept = default;

        ~MemoryBarrier() override {
            Unload();
        }

        auto Unload() noexcept -> void override {
            if ( loaded ) {
                glDeleteSync( fence );
                loaded = false;
            }
        }

        auto Load() noexcept -> bool override {
            fence = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

            loaded = true;

            return loaded;
        }

        /**
         *  @brief Force the CPU to wait for the GPU to finish execution ensuring the CPU blocks indefinitely until the
         *  fence is signaled.
         *  @return Returns true when ready.
         */
        auto Ready() const noexcept -> bool {
            const GLenum waitStatus{ glClientWaitSync( fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED ) };

            return equal( waitStatus, GL_ALREADY_SIGNALED ) or equal( waitStatus, GL_CONDITION_SATISFIED );
        }

        auto Wait( const std::uint64_t timeout ) const noexcept -> void {
            while ( equal( GL_TIMEOUT_EXPIRED, glClientWaitSync( fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeout ) ) ) {}
        }
    };
} // namespace PeanutGL
