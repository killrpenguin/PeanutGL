#pragma once

#include "ResourceBase.hpp"
#include "ShaderProgram.hpp"

namespace PeanutGL {

    class Uniform final : public Resource {
        [[maybe_unused]] uniform_info info{};

      public:
        Uniform() = delete;

        explicit Uniform( const std::string& identifier, const uniform_info info ) noexcept
            : Resource( identifier ), info{ info } {
        }

        ~Uniform() override = default;

        Uniform( const Uniform& )                = delete;
        Uniform( Uniform&& ) noexcept            = default;
        Uniform& operator=( const Uniform& )     = delete;
        Uniform& operator=( Uniform&& ) noexcept = default;

        auto Load() noexcept -> bool override {
            return true;
        }

        auto Unload() noexcept -> void override {
        }
    };
} // namespace PeanutGL
