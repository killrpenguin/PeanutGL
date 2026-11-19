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

#include <condition_variable>
#include <mutex>
#include <thread>
#include <variant>

namespace PeanutGL {

    using EventVariant = std::variant<>;

    template < class... Ts > struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template < class... Ts > overloaded( Ts... ) -> overloaded< Ts... >;

    class MessageQueue {
      private:
      public:
        MessageQueue()                                           = default;
        MessageQueue( const MessageQueue& other )                = default;
        MessageQueue( MessageQueue&& other ) noexcept            = default;
        ~MessageQueue() noexcept                                 = default;

        MessageQueue& operator=( MessageQueue&& other ) noexcept = default;
    };

    class EventSystem {
      private:
        std::mutex mutex;
        std::condition_variable cv;
        [[maybe_unused]] bool quit{ false };

      public:
        EventSystem()           = default;
        ~EventSystem() noexcept = default;

        EventSystem( const EventSystem& other )            = delete;
        EventSystem& operator=( const EventSystem& other ) = delete;

        EventSystem( EventSystem&& other ) noexcept            = delete;
        EventSystem& operator=( EventSystem&& other ) noexcept = delete;
    };
} // namespace PeanutGL

