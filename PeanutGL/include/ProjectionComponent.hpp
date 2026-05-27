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
     * @brief Projection component.
     *
     * This class implements the component interface.
     */
    class ProjectionComponent final : public Component {
      private:
        Width width;
        Height height;

        ResourceHandle< ShaderProgram > shader_program{};

      public:
        ProjectionComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param width The screen width.
         * @param height The screen height.
         * @param componentName The name of the component.
         */
        explicit ProjectionComponent(
            const ResourceHandle< ShaderProgram >& handle, const Width width, const Height height,
            const std::string& componentName = "projection" );

        ProjectionComponent( const ProjectionComponent& )            = delete;
        ProjectionComponent( ProjectionComponent&& )                 = delete;
        ProjectionComponent& operator=( const ProjectionComponent& ) = delete;
        ProjectionComponent& operator=( ProjectionComponent&& )      = delete;

        ~ProjectionComponent() noexcept override = default;

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
    };
} // namespace PeanutGL
