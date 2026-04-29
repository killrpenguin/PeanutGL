#pragma once

#include "Component.hpp"

namespace PeanutGL {
    class TransformComponent final : public Component {
      public:
        auto Initialize() noexcept -> void override {
        }

        auto Update( [[maybe_unused]] std::chrono::milliseconds deltaTime ) -> void override {
        }

        auto Render() const noexcept -> void {
        }
    };
} // namespace PeanutGL
