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
#include "Utilities.hpp"
#include <system_error>

#define UNW_LOCAL_ONLY // Remote unwinding is not async-signal-safe.
#include "libunwind.h"

#include <quill/LogMacros.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <cxxabi.h>
#include <execinfo.h>
#include <fcntl.h>
#include <filesystem>
#include <functional>
#include <mutex>
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
        using FilePointer =
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
            std::unique_ptr< FILE, decltype( []( FILE* file_ptr ) { (void)std::fclose( file_ptr ); } ) >;

      private:
        // Private constructor to enforce the singleton pattern.
        CrashReporter() = default;

        bool initialized{ false };
        std::atomic< bool > handling_crash{ false };
        std::atomic< bool > report_created{ false };

        fs::path minidump_dir{ fs::current_path() / fs::path( "CrashReport" ) };
        fs::path file_path{};

        FilePointer file_ptr{};
        int report_fd{};

        std::mutex mutex;
        std::unordered_map< int, std::function< void( const std::string& ) > > crash_callbacks;
        int nextCallbackId = 0;

      public:
        ~CrashReporter() {
            Cleanup();
        }

        // Delete the copy/move constructors and assignment operators to enforce the singleton pattern.
        CrashReporter( CrashReporter&& ) noexcept            = delete;
        CrashReporter& operator=( CrashReporter&& ) noexcept = delete;
        CrashReporter( const CrashReporter& )                = delete;
        CrashReporter& operator=( const CrashReporter& )     = delete;

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
                fs::create_directory( minidump_dir );
            } catch ( const fs::filesystem_error& err ) {
                // clang-format off
                LOG_ERROR(
                    QuillPtr(),
                    R"(what: {}\n
				 	  first path: {}\n
				 	  second path: {}\n
				 	  code value: {}\n
				 	  code message: {}\n
                       code category: {}\n)",
                    err.what(), err.path1().c_str(), err.path2().c_str(),
				 	err.code().value(), err.code().message(),
                    err.code().category().name() );
                // clang-format on
                return false;
            } catch ( ... ) {
                LOG_ERROR( QuillPtr(), "Uncaught exception thrown during CrashReporter Initialization." );
                return false;
            }

            using namespace std::literals; // enables literal suffixes, e.g. 24h, 1ms, 1s.

            const std::chrono::time_point now{ std::chrono::system_clock::now() };

            const std::string tmp_filename{ std::format( "{:%Y-%m-%d_%H:%M:%S}.txt", now ) };

            file_path = minidump_dir / fs::path( tmp_filename );

            errno = 0;
            if ( FILE* file_handle{ std::fopen( file_path.c_str(), "a" ) };
                 file_handle != nullptr ) { // Truncates (erases) content
                // make_unique does not accept a custom deleter so use the default constructor with a try/catch.
                try {
                    file_ptr = FilePointer( file_handle );
                } catch ( const std::bad_alloc& err ) {
                    LOG_ERROR( QuillPtr(), "" );
                    return false;
                } catch ( ... ) {
                    LOG_ERROR( QuillPtr(), "Uncaught exception during unique_ptr constructor call in CrashReporter." );
                    return false;
                }

            } else {
                LOG_ERROR(
                    QuillPtr(),
                    "Failed to get file handle in CrashReporter.Initialize().\nError number: {}\nDescription {}", errno,
                    std::strerror( errno ) );
                return false;
            }

            if ( report_fd = fileno( file_ptr.get() ); report_fd == -1 ) {
                LOG_ERROR(
                    QuillPtr(),
                    "Failed to get file descriptor in CrashReporter.Initialize().\nError number: {}\nDescription {}",
                    errno, std::strerror( errno ) );
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
                std::error_code err{};
                if ( const bool success = fs::remove( file_path, err ); !success ) {
                    LOG_ERROR(
                        QuillPtr(), "The CrashReporter report file removal failed. \nValue: {}\n Message: {}.",
                        err.value(), err.message() );
                }
                UninstallCrashHandlers();
                initialized = false;
            }
        }

        /**
         * @brief Handle a crash.
         * @param message The crash message.
         */
        template < typename T > auto HandleCrash( T message ) noexcept -> void;

        auto HandleCrash( const std::string& message ) noexcept -> void {
            HandleCrashInternal( message );
        }

        auto HandleCrash( const std::string_view message ) noexcept -> void {
            HandleCrashInternal( std::string( message ) );
        }

        /**
         * @brief Register a crash callback.
         * @param callback The callback function to be called when a crash occurs.
         * @return An ID that can be used to unregister the callback.
         */
        auto RegisterCrashCallback( std::function< void( const std::string& ) > callback ) -> int {
            std::lock_guard< std::mutex > lock( mutex );

            int callback_id              = nextCallbackId++;
            crash_callbacks[callback_id] = std::move( callback );
            return callback_id;
        }

        /**
         * @brief Unregister a crash callback.
         * @param callback_id The ID of the callback to unregister.
         */
        auto UnregisterCrashCallback( const int callback_id ) noexcept -> void {
            std::lock_guard< std::mutex > lock( mutex );

            crash_callbacks.erase( callback_id );
        }

        /**
         * @brief Generate a minidump.
         * @param message The crash message.
         */
        auto GenerateMinidump( const std::string& message ) const noexcept -> void {
            timespec time_data{};

            if ( const int failed = clock_gettime( CLOCK_REALTIME, &time_data ); equal( failed, -1 ) ) { return; }

            constexpr int SECS_IN_DAY{ 24 * 60 * 60 };

            // This is for a timestamp on a file. The narrowing convertion is fine.
            const char hours{ static_cast< char >( ( time_data.tv_sec % SECS_IN_DAY ) / 3600 ) };
            const char minutes{ static_cast< char >( ( time_data.tv_sec % 3600 ) / 60 ) };
            const char seconds{ static_cast< char >( time_data.tv_sec % 60 ) };

            const std::array< char, 7 > time_stamp{ hours, ':', minutes, ':', seconds, '\n', '\0' };

            write( report_fd, time_stamp.data(), time_stamp.size() );

            write( report_fd, message.c_str(), message.size() );

            GetBacktrace( report_fd );
        }

      private:
        auto HandleCrashInternal( const std::string& message ) noexcept -> void {
            bool expected{ false };

            if ( !handling_crash.compare_exchange_strong( expected, true ) ) {
                // Already handling a crash; avoid recursion.
                return;
            }

            report_created.store( true );

            std::lock_guard< std::mutex > lock( mutex );

            GenerateMinidump( message );

            for ( const auto& callback : crash_callbacks ) {
                callback.second( message );
            }
        }

        /**
         * @brief Get an async-signal-safe backtrace.
         */
        auto static GetBacktrace( const int file_descriptor = STDERR_FILENO ) noexcept -> void {
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-avoid-c-arrays,
            // cppcoreguidelines-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)
            unw_context_t context{};
            if ( const int success = unw_getcontext( &context ); success != 0 ) {
                const char msg[] = "Failed to get unwind context.";

                constexpr std::size_t msg_size{ std::size( msg ) };

                write( file_descriptor, msg, msg_size );
                return;
            }

            unw_cursor_t cursor{};
            // use UNW_INIT_SIGNAL_FRAME once this is a crash handler function. see unw_init_local() documentation.
            if ( const int success = unw_init_local( &cursor, &context ); success != 0 ) {
                const char msg[] = "Failed to init stack frame.";

                constexpr std::size_t msg_size{ std::size( msg ) };

                write( file_descriptor, msg, msg_size );
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
                constexpr std::size_t BUFF_SIZE{ 512 };
                std::array< char, BUFF_SIZE > symbol_name{};

                offset = instruction_ptr - stack_ptr;

                if ( const int success = unw_get_proc_name( &cursor, symbol_name.data(), BUFF_SIZE, &offset );
                     success != 0 ) {
                    switch ( success ) {
                        case UNW_EUNSPEC: {
                            const char error_msg[] = "An unspecified error occurred.\n";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            write( file_descriptor, error_msg, error_msg_size );

                            return;
                        }
                        case UNW_ENOINFO: {
                            const char error_msg[] = "Libunwind was unable to determine the name of the procedure.\n";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            write( file_descriptor, error_msg, error_msg_size );

                            return;
                        }
                        case UNW_ENOMEM: {
                            const char error_msg[] = "The procedure name is too long to fit in the buffer provided. A "
                                                     "truncated version of the name has been returned.\n";
                            constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                            write( file_descriptor, error_msg, error_msg_size );
                            return; // Early return here or risk allocator deadlock if cxa_demangle calls realloc.
                        }
                        default: return;
                    };
                    return;
                }
                int status{};
                std::array< char, BUFF_SIZE > demangled_name{};
                std::size_t output_buff_size{ demangled_name.size() }; // cxa_demangle won't take a const std::size_t*.

                abi::__cxa_demangle( symbol_name.data(), demangled_name.data(), &output_buff_size, &status );

                switch ( status ) {
                    case 0: {
                        (void)write( file_descriptor, demangled_name.data(), std::strlen( demangled_name.data() ) );
                        (void)write( file_descriptor, "\n\n", 2 );

                        break;
                    }
                    case -2: { //  A status of -2 indicates the string is not valid under C++ ABI mangling rules.
                        (void)write( file_descriptor, symbol_name.data(), std::strlen( symbol_name.data() ) );
                        (void)write( file_descriptor, "\n\n", 1 );
                        break;
                    }
                    default: {
                        const char error_msg[] = "An unspecified error occurred.\n";

                        constexpr std::size_t error_msg_size{ std::size( error_msg ) };

                        (void)write( file_descriptor, error_msg, error_msg_size );
                        return;
                    }
                };
            }
            // NOLINTEND(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-avoid-c-arrays,
            // cppcoreguidelines-avoid-c-arrays, cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }

        /**
         * @brief Install crash handlers.
         */
        auto static InstallCrashHandlers() -> void {
            errno = 0;
            constexpr static std::string_view seg_fault{ "Segmentation fault" };

            if ( sighandler_t old_signal = signal(
                     SIGSEGV,
                     []( [[maybe_unused]] const int sig ) constexpr {
                         CrashReporter::GetInstance().HandleCrash( seg_fault );
                         throw FatalSignalError( seg_fault );
                     } );
                 equal( old_signal, SIG_ERR ) ) {
                LOG_CRITICAL(
                    QuillPtr(), "Failed to set signal handler for {}.\nError number: {}\nDescription: {}", seg_fault,
                    errno, std::strerror( errno ) );
            }

            errno = 0;
            constexpr static std::string_view abort{ "Abort" };

            if ( sighandler_t old_signal = signal(
                     SIGABRT,
                     []( [[maybe_unused]] const int sig ) {
                         CrashReporter::GetInstance().HandleCrash( abort );
                         throw FatalSignalError( abort );
                     } );
                 equal( old_signal, SIG_ERR ) ) {
                LOG_CRITICAL(
                    QuillPtr(), "Failed to set signal handler for {}.\nError number: {}\nDescription: {}", abort, errno,
                    std::strerror( errno ) );
            }

            errno = 0;
            constexpr static std::string_view floating_point{ "Floating point exception" };

            if ( sighandler_t old_signal = signal(
                     SIGFPE,
                     []( [[maybe_unused]] const int sig ) {
                         CrashReporter::GetInstance().HandleCrash( floating_point );
                         throw FatalSignalError( floating_point );
                     } );
                 equal( old_signal, SIG_ERR ) ) {
                LOG_CRITICAL(
                    QuillPtr(), "Failed to set signal handler for {}.\nError number: {}\nDescription {}",
                    floating_point, errno, std::strerror( errno ) );
            }

            errno = 0;
            constexpr static std::string_view illegal_instruction{ "Illegal instruction" };

            if ( sighandler_t old_signal = signal(
                     SIGILL,
                     []( [[maybe_unused]] const int sig ) {
                         CrashReporter::GetInstance().HandleCrash( illegal_instruction );
                         throw FatalSignalError( illegal_instruction );
                     } );
                 equal( old_signal, SIG_ERR ) ) {
                LOG_CRITICAL(
                    QuillPtr(), "Failed to set signal handler for {}.\nError number: {}\nDescription: {}",
                    illegal_instruction, errno, std::strerror( errno ) );
            }
        }

        /**
         * @brief Uninstall crash handlers.
         */
        auto static UninstallCrashHandlers() noexcept -> void {
            errno = 0;
            if ( sighandler_t success = signal( SIGSEGV, SIG_DFL ); equal( success, SIG_ERR ) ) {
                LOG_ERROR( QuillPtr(), "Error number: {}\nDescription: {}", errno, std::strerror( errno ) );
            }
            errno = 0;
            if ( sighandler_t success = signal( SIGABRT, SIG_DFL ); equal( success, SIG_ERR ) ) {
                LOG_ERROR( QuillPtr(), "Error number: {}\nDescription: {}", errno, std::strerror( errno ) );
            }
            errno = 0;
            if ( sighandler_t success = signal( SIGFPE, SIG_DFL ); equal( success, SIG_ERR ) ) {
                LOG_ERROR( QuillPtr(), "Error number: {}\nDescription: {}", errno, std::strerror( errno ) );
            }
            errno = 0;
            if ( sighandler_t success = signal( SIGILL, SIG_DFL ); equal( success, SIG_ERR ) ) {
                LOG_ERROR( QuillPtr(), "Error number: {}\nDescription: {}", errno, std::strerror( errno ) );
            }
        }
    };
} // namespace PeanutGL
