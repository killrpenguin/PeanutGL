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

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include <quill/LogMacros.h>
#include <quill/core/LogLevel.h>

namespace PeanutGL {

    /**
     * @brief Class for managing debugging and logging.
     *
     * This class wraps the Quill asynchronous logging library as a static singleton.
     */
    class DebugSystem {
      private:
        struct CustomFrontendOptions {
            static constexpr quill::QueueType queue_type               = quill::QueueType::BoundedDropping;
            static constexpr size_t initial_queue_capacity             = 131'072;
            static constexpr uint32_t blocking_queue_retry_interval_ns = 800;
            static constexpr size_t unbounded_queue_max_capacity       = 2ULL * 1024U * 1024U * 1024U;
            static constexpr quill::HugePagesPolicy huge_pages_policy  = quill::HugePagesPolicy::Never;
        };

        using CustomFrontend = quill::FrontendImpl< CustomFrontendOptions >;

        using CustomLogger = quill::LoggerImpl< CustomFrontendOptions >;

        CustomLogger* logger{ nullptr };

        std::mutex mutex;

        std::unordered_map< std::string, std::chrono::high_resolution_clock::time_point > measurements{};

        std::function< void( const std::string& ) > crashHandler{};

      protected:
        DebugSystem() noexcept {
            quill::BackendOptions backend_options{};

            quill::Backend::start( backend_options );

            auto console_sink = CustomFrontend::create_or_get_sink< quill::ConsoleSink >( "sink_id_1" );

            logger = CustomFrontend::create_or_get_logger( "DebugSystem", std::move( console_sink ) );

            logger->set_log_level( quill::LogLevel::TraceL3 );

            quill::PatternFormatterOptions options{};

            // Strip common path prefixes to shorten source locations
            // "/home/user/project/src/main.cpp:42" becomes "src/main.cpp:42"
            options.source_location_path_strip_prefix = "/home/user/project/";

            // Remove relative path components like "../"
            options.source_location_remove_relative_paths = true;
        }

      public:
        DebugSystem( DebugSystem&& ) noexcept            = delete;
        DebugSystem& operator=( DebugSystem&& ) noexcept = delete;
        DebugSystem( const DebugSystem& )                = delete;
        DebugSystem& operator=( const DebugSystem& )     = delete;

        ~DebugSystem() {
            logger = nullptr;
        }

        /**
         * @brief Get the singleton instance of the debug system.
         * @return Reference to the debug system instance.
         */
        auto static Get() noexcept -> DebugSystem& {
            static DebugSystem instance;
            return instance;
        }

        auto Logger() noexcept -> CustomLogger* {
            return logger;
        }

        /**
         * @brief Start a performance measurement.
         * @param name The name of the measurement.
         */
        auto StartMeasurement( const std::string& name ) noexcept -> void {
            std::lock_guard< std::mutex > lock( mutex );

            auto now = std::chrono::high_resolution_clock::now();

            measurements[name] = now;
        }

        /**
         * @brief End a performance measurement and log the result.
         * @param name The name of the measurement.
         */
        auto StopMeasurement( const std::string& name ) noexcept -> void {
            auto now = std::chrono::high_resolution_clock::now();
            std::lock_guard< std::mutex > lock( mutex );

            auto measurement_item = measurements.find( name );

            if ( measurement_item != measurements.end() ) {
                auto duration =
                    std::chrono::duration_cast< std::chrono::microseconds >( now - measurement_item->second ).count();
                LOG_INFO( logger, "Performance {}: {} us", name, duration );
                measurements.erase( measurement_item );
            } else {
                LOG_ERROR( logger, "Performance: No measurement started with name: {}", name );
            }
        }
        /**
         * @brief Set the crash handler.
         * @param handler The crash handler function.
         */
        auto SetCrashHandler( std::function< void( const std::string& ) > handler ) -> void {
            std::lock_guard< std::mutex > lock( mutex );

            crashHandler = std::move( handler );
        }

        using QuillPtrType = CustomLogger*;
    };

    constexpr auto QuillPtr() noexcept -> DebugSystem::QuillPtrType {
        return DebugSystem::Get().Logger();
    }

} // namespace PeanutGL
