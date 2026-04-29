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

#include "DebugSystem.hpp"

// Remote unwinding is not async-signal-safe.
#define UNW_LOCAL_ONLY
#include <libunwind.h>

#include "Utilities.hpp"
#include <quill/LogMacros.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <ctime>
#include <execinfo.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <print>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <utility>

namespace PeanutGL {
    namespace fs = std::filesystem;
    /**
     * @brief Class for crash reporting and minidump generation.
     */
    class CrashReporter {
      private:
        // Private constructor for singleton
        CrashReporter() = default;

        std::mutex mutex;

        bool initialized{ false };

        // Minidump directory
        fs::path minidumpDir{ fs::current_path() / fs::path( "crash_report" ) };

        // Application info
        constexpr static std::string appName{ "PeanutGL" };
        constexpr static std::string appVersion{ "1.0.0" };

        // Crash callbacks
        std::unordered_map< int, std::function< void( const std::string& ) > > crashCallbacks;
        int nextCallbackId = 0;
        std::atomic< bool > handlingCrash{ false };

      public:
        ~CrashReporter() {
            Cleanup();
        }

        // Delete copy constructor and assignment operator
        CrashReporter( CrashReporter&& ) noexcept            = delete;
        CrashReporter& operator=( CrashReporter&& ) noexcept = delete;
        CrashReporter( const CrashReporter& )                = delete;
        CrashReporter& operator=( const CrashReporter& )     = delete;

        explicit CrashReporter( std::unordered_map< int, std::function< void( const std::string& ) > > crashCallbacks )
            : crashCallbacks( std::move( crashCallbacks ) ) {
        }

        static auto GetSymbolTest() noexcept -> void {
            // NOLINTBEGIN

            unw_context_t context{}; // uc
            if ( const int success = unw_getcontext( &context ); success != 0 ) {
                const char msg[] = "Failed to get unwind context.";
                constexpr std::size_t msg_size{ std::size( msg ) };
                static_assert( msg_size != 0 );
                write( STDERR_FILENO, msg, msg_size );
                return;
            }

            unw_cursor_t cursor{};
            // use UNW_INIT_SIGNAL_FRAME once this is a crash handler function. see unw_init_local() documentation.
            if ( const int success = unw_init_local( &cursor, &context ); success != 0 ) {
                const char msg[] = "Failed to init stack frame.";
                constexpr std::size_t msg_size{ std::size( msg ) };
                static_assert( msg_size != 0 );
                write( STDERR_FILENO, msg, msg_size );
                return;
            }

            unw_word_t instruction_ptr{};
            unw_word_t stack_ptr{};
            unw_word_t offset{};

            while ( unw_step( &cursor ) > 0 ) {
                if ( const int success = unw_get_reg( &cursor, UNW_REG_IP, &instruction_ptr ); success != 0 ) {
                    return;
                }
                if ( const int success = unw_get_reg( &cursor, UNW_REG_SP, &stack_ptr ); success != 0 ) { return; }
                constexpr std::size_t BUFF_SIZE{ 256 };
                std::array< char, BUFF_SIZE > symbol_name{};

                offset = instruction_ptr - stack_ptr;

                if ( const int success = unw_get_proc_name( &cursor, symbol_name.data(), BUFF_SIZE, &offset );
                     success != 0 ) {
                    switch ( success ) {
                        case UNW_EUNSPEC: {
                            const char error_msg[] = "An unspecified error occurred.";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            (void)write( STDERR_FILENO, error_msg, error_msg_size );

                            return;
                        }
                        case UNW_ENOINFO: {
                            const char error_msg[] = "Libunwind was unable to determine the name of the procedure.";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            (void)write( STDERR_FILENO, error_msg, error_msg_size );

                            return;
                        }
                        case UNW_ENOMEM: {
                            const char error_msg[] = "The procedure name is too long to fit in the buffer provided. A "
                                                     "truncated version of the name has been returned.";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            (void)write( STDERR_FILENO, error_msg, std::size( error_msg ) );
                            return;
                        }
                        default: return;
                    };
                    return;
                }
                (void)write( STDERR_FILENO, symbol_name.data(), BUFF_SIZE );
            }
            // NOLINTEND
        }

        /**
         * @brief Get the singleton instance of the crash reporter.
         * @return Reference to the crash reporter instance.
         */
        static CrashReporter& GetInstance() {
            static CrashReporter instance;
            return instance;
        }

        /**
         * @brief Initialize the crash reporter.
         * @return True if initialization was successful, false otherwise.
         */
        auto Initialize() noexcept -> bool {
            std::lock_guard< std::mutex > lock( mutex );

            try {
                // If the Directory already exists false is returned, not an error.
                if ( const bool exists = fs::create_directory( minidumpDir ); !exists ) {
                    LOG_INFO( QuillPtr(), "Crashreport folder already exists." );
                }
            } catch ( const fs::filesystem_error& err ) {
                // clang-format off
                // LOG_ERROR(
                //     QuillPtr(),
                //     R"(what: {}\n
				// 	  path1: {}\n
				// 	  path2: {}\n
				// 	  code value: {}\n
				// 	  code message: {}\n
                //       code category: {}\n)",
                //     err.what(), err.path1(), err.path2(),
				// 	err.code().value(), err.code().message(),
                //     err.code().category().name() );
                // clang-format on
                return false;
            } catch ( ... ) {
                LOG_ERROR( QuillPtr(), "Uncaught exception thrown during CrashReporter Initialization." );
                return false;
            }

            InstallCrashHandlers();

            DebugSystem::Get().SetCrashHandler(
                [this]( const std::string& message ) { this->HandleCrash( message ); } );

            initialized = true;
            return true;
        }

        /**
         * @brief Clean up crash reporter resources.
         */
        auto Cleanup() noexcept -> void {
            std::lock_guard< std::mutex > lock{ mutex };

            if ( initialized ) {
                // Uninstall crash handlers
                UninstallCrashHandlers();

                initialized = false;
            }
        }

        /**
         * @brief Handle a crash.
         * @param message The crash message.
         */
        auto HandleCrash( const std::string& message ) noexcept -> void {
            HandleCrashInternal( message, nullptr );
        }

        /**
         * @brief Register a crash callback.
         * @param callback The callback function to be called when a crash occurs.
         * @return An ID that can be used to unregister the callback.
         */
        auto RegisterCrashCallback( std::function< void( const std::string& ) > callback ) -> int {
            std::lock_guard< std::mutex > lock( mutex );

            int callback_id             = nextCallbackId++;
            crashCallbacks[callback_id] = std::move( callback );
            return callback_id;
        }

        /**
         * @brief Unregister a crash callback.
         * @param callback_id The ID of the callback to unregister.
         */
        auto UnregisterCrashCallback( int callback_id ) noexcept -> void {
            std::lock_guard< std::mutex > lock( mutex );

            crashCallbacks.erase( callback_id );
        }

        /**
         * @brief Generate a minidump.
         * @param message The crash message.
         */
        auto GenerateMinidump(
            const std::string& message, [[maybe_unused]] void* platformExceptionPointers = nullptr ) noexcept -> void {
            // Get current time for filename
            auto now  = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t( now );

            constexpr std::size_t TIMESTR_SIZE{ 20 };

            std::array< char, TIMESTR_SIZE > timeStr{};

            const std::size_t placeholder =
                std::strftime( timeStr.data(), sizeof( timeStr ), "%Y%m%d_%H%M%S", std::localtime( &time ) );

            // Create minidump filename
            const std::string_view tmp_filename{ appName + "_" + std::string_view( timeStr ) + ".dmp" };
            const std::string_view tmp_report{ appName + "_" + std::string_view( timeStr ) + ".txt" };

            const fs::path filename{ minidumpDir / fs::path( tmp_filename ) };
            const fs::path report{ minidumpDir / fs::path( tmp_report ) };

            // Also write a small sidecar text file so users can quickly see the exception code/address
            // without needing a debugger.
            try {
                std::ofstream rep( report, std::ios::out | std::ios::trunc );
                rep << "Crash Report for " << appName << " " << appVersion << "\n";
                rep << "Timestamp: " << std::string_view( timeStr ) << "\n";
                rep << "Message: " << message << "\n";
            } catch ( ... ) { LOG_ERROR( QuillPtr(), " Uncaught exception in generate minidump function. FIX ME!" ); }

            // Generate minidump based on platform

            // Unix implementation
            std::ofstream file( filename, std::ios::out | std::ios::binary );
            constexpr std::size_t CALLSTACK_SIZE{ 128 };
            if ( file.is_open() ) {
                // Get backtrace

                std::array< void*, CALLSTACK_SIZE > callstack{};
                int frames     = backtrace( callstack.data(), CALLSTACK_SIZE );
                char** symbols = backtrace_symbols( callstack.data(), frames );

                // Write header
                file << "Crash Report for " << appName << " " << appVersion << '\n';
                file << "Timestamp: " << timeStr.data() << '\n';
                file << "Message: " << message << '\n';
                file << '\n';

                // Write backtrace
                file << "Backtrace:" << '\n';
                for ( int i = 0; i < frames; i++ ) {
                    file << symbols[i] << '\n';
                }

                free( symbols );
                file.close();
            }

            // Best-effort stderr note (stdout/stderr redirection will capture this even if DebugSystem isn't
            // initialized)
            std::println( stderr, "[CrashReporter] Wrote minidump: {}\n", filename.string() );
            std::println( stderr, "[CrashReporter] Wrote report:  {}\n", report.string() );
        }

      private:
        auto HandleCrashInternal( const std::string& message, void* platformExceptionPointers ) noexcept -> void {
            bool expected{ false };

            if ( !handlingCrash.compare_exchange_strong( expected, true ) ) {
                // Already handling a crash; avoid recursion.
                return;
            }

            std::lock_guard< std::mutex > lock( mutex );

            std::string msg{ message };
            (void)platformExceptionPointers;

            LOG_CRITICAL( QuillPtr(), "CrashReporter", "Crash detected: {}", msg );

            // Generate minidump
            GenerateMinidump( msg, platformExceptionPointers );

            // Call registered callbacks
            for ( const auto& callback : crashCallbacks ) {
                callback.second( msg );
            }
        }

        /**
         * @brief Install platform-specific crash handlers.
         */
        auto static InstallCrashHandlers() noexcept -> void {
            // Unix implementation
            signal( SIGSEGV, []( int sig ) {
                CrashReporter::GetInstance().HandleCrash( "Segmentation fault" );
                exit( 1 );
            } );

            signal( SIGABRT, []( int sig ) {
                CrashReporter::GetInstance().HandleCrash( "Abort" );
                exit( 1 );
            } );

            signal( SIGFPE, []( int sig ) {
                CrashReporter::GetInstance().HandleCrash( "Floating point exception" );
                exit( 1 );
            } );

            signal( SIGILL, []( int sig ) {
                CrashReporter::GetInstance().HandleCrash( "Illegal instruction" );
                exit( 1 );
            } );
        }

        /**
         * @brief Uninstall platform-specific crash handlers.
         */
        auto static UninstallCrashHandlers() noexcept -> void {
            // Unix implementation
            signal( SIGSEGV, SIG_DFL );
            signal( SIGABRT, SIG_DFL );
            signal( SIGFPE, SIG_DFL );
            signal( SIGILL, SIG_DFL );
        }
    };

    constexpr auto SimulateCrash( const std::string& message ) noexcept -> void {
        CrashReporter::GetInstance().HandleCrash( message );
    }

} // namespace PeanutGL
