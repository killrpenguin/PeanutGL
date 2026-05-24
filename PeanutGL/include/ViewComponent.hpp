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
     * @brief View component.
     *
     * This class implements the component interface.
     */
    class ViewComponent final : public Component {
      private:
        ResourceHandle< ShaderProgram > shader_program{};
        float zAxis{};

      public:
        ViewComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param componentName The name of the component.
         */
        explicit ViewComponent(
            const ResourceHandle< ShaderProgram >& handle, const float zAxis = -3.0F,
            const std::string& componentName = "view" );

        ViewComponent( const ViewComponent& )            = delete;
        ViewComponent( ViewComponent&& )                 = delete;
        ViewComponent& operator=( const ViewComponent& ) = delete;
        ViewComponent& operator=( ViewComponent&& )      = delete;

        ~ViewComponent() noexcept override = default;

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
