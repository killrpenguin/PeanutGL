#pragma once

#include "Component.hpp"
#include "ShaderProgram.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {
    /**
     * @brief Model component.
     *
     * This class implements the component interface.
     */
    class ModelComponent final : public Component {
      private:
        ResourceHandle< ShaderProgram > shader_program{};
        float degrees{};

      public:
        ModelComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param componentName The name of the component.
         */
        explicit ModelComponent(
            const ResourceHandle< ShaderProgram >& handle, const float degrees = -55.0F,
            const std::string& componentName = "model" );

        ModelComponent( const ModelComponent& )            = delete;
        ModelComponent( ModelComponent&& )                 = delete;
        ModelComponent& operator=( const ModelComponent& ) = delete;
        ModelComponent& operator=( ModelComponent&& )      = delete;

        ~ModelComponent() noexcept override = default;

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

        /**
         * @brief Set the degrees.
         */
        auto SetDegrees( const float val ) noexcept -> void;
    };
} // namespace PeanutGL
