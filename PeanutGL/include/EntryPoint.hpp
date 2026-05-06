/*
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

#include "CrashReporterSystem.hpp"
#include "Engine.hpp"
#include "RenderDocSystem.hpp"

#include "Utilities.hpp"
#include "quill/LogMacros.h"

// NOLINTBEGIN(misc-definitions-in-headers, readability-redundant-declaration)
extern void PeanutGL::EngineSetup( Engine* engine );

namespace PeanutGL {
    auto Main() -> int {
        try {
            CrashReporter::GetInstance().Initialize();

            (void)RenderDocSystem::Get();

            Engine engine;

            if ( !engine.Initialize() ) { throw std::runtime_error( "Failed to initialize engine" ); }

            EngineSetup( &engine );

            engine.Run();

            // CrashReporter::GetInstance().Cleanup();

            return 0;
        } catch ( const FatalSignalError& err ) {
            LOG_CRITICAL( QuillPtr(), "Fatal signal from system: {}", err.what() );
            return err();
        } catch ( const std::exception& err ) {
            LOG_CRITICAL( QuillPtr(), "Exception: {}", err.what() );
            CrashReporter::GetInstance().Cleanup();
            return 1;
        }
    }

} // namespace PeanutGL

int main( [[maybe_unused]] int argc, [[maybe_unused]] char** argv ) {
    return PeanutGL::Main();
}

// NOLINTEND(misc-definitions-in-headers, readability-redundant-declaration)
