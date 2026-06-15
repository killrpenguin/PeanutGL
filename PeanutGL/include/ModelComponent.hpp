#pragma once

#include "Component.hpp"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <quill/LogMacros.h>

namespace PeanutGL {

    /**
     * @brief A component for transforming local space into world space.
     *
     */

    class ModelComponent final : public Component {
      private:
        float angle{ 0.0F };

        glm::vec3 position{ 0.0F, 0.0F, 0.0F };
        glm::vec3 rotation_axis{ 0.0F, 1.0F, 0.0F };

        glm::mat4 model{ 1.0F };

      public:
        ModelComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param handle The handle to the shader program.
         * @param componentName The name of the component.
         */
        explicit ModelComponent( const std::string& componentName );

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
         * @brief Get the Model matrix data calculated during the last update call.
         */
        auto MatrixData() const noexcept -> glm::mat4;

        /**
         * @brief Set the angle.
         */
        constexpr auto SetAngle( const float new_angle ) noexcept -> void {
            angle = new_angle;
        }

        /**
         * @brief Set the rotation axis.
         */
        constexpr auto SetRotationAxis( const glm::vec3 rotation ) noexcept -> void {
            rotation_axis = rotation;
        }
        /**
         * @brief Set the Position used to calculate the matrix.
         */
        constexpr auto SetPosition( const glm::vec3 pos ) noexcept -> void {
            position = pos;
        }
    };
} // namespace PeanutGL
