#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <condition_variable>
#include <ctime>
#include <cxxabi.h>
#include <execinfo.h>
#include <functional>
#include <fcntl.h>
#include <memory>
#include <ranges>
#include <string>
#include <span>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <stdexcept>
#include <atomic>

// clang-format off
#include "glad/gl.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <readerwriterqueue.h>

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"

#define UNW_LOCAL_ONLY
#include "libunwind.h"
