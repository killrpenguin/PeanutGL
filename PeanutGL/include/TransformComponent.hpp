#pragma once

#include "Component.hpp"
#include "ShaderProgram.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {

    class TransformComponent final : public Component {
      private:
        ResourceHandle< ShaderProgram > shader_program;

      public:
        TransformComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param componentName The name of the component.
         */
        explicit TransformComponent(
            // NOLINTNEXTLINE
            const ResourceHandle< ShaderProgram > handle, const std::string& componentName = "transform" );

        TransformComponent( const TransformComponent& )            = delete;
        TransformComponent( TransformComponent&& )                 = delete;
        TransformComponent& operator=( const TransformComponent& ) = delete;
        TransformComponent& operator=( TransformComponent&& )      = delete;

        ~TransformComponent() noexcept override = default;

        /**
         * @brief Initialize the component.
         */
        auto Initialize() noexcept -> void override;

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( const std::chrono::milliseconds deltaTime ) -> void override;

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void override;

        auto MatrixData() const noexcept -> glm::mat4 override {
            return {};
        }
    };
} // namespace PeanutGL
