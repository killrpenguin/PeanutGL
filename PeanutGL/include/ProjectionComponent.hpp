#pragma once

#include "Component.hpp"
#include "UtilityTypes.hpp"

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

        Degrees degrees{};
        ZNear zNear{};

        glm::mat4 projection{ 1.0F };

      public:
        ProjectionComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param width The screen width.
         * @param height The screen height.
         * @param componentName The name of the component.
         */
        explicit ProjectionComponent( const std::string& componentName, const Width width, const Height height );

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

        /**
         * @brief Get the matrix data calculated during the last update call.
         */
        auto MatrixData() const noexcept -> glm::mat4;

        /**
         * @brief Set the degrees. The component will convert the float to radians.
         * @param new_degrees The new degrees to use when calculating the matrix.
         */
        constexpr auto SetDegrees( const float new_degrees ) noexcept -> void {
            degrees = new_degrees;
        }

        /**
         * @brief Set the zNear.
         * @param new_zNear The new zNear to use when calculating the matrix.
         */
        constexpr auto SetzNear( const float new_zNear ) noexcept -> void {
            zNear = new_zNear;
        }
    };
} // namespace PeanutGL
