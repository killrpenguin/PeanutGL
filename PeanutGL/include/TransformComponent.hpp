#pragma once

#include "Component.hpp"
#include "DebugSystem.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace PeanutGL {
    class TransformComponent final : public Component {
      private:
        glm::mat4 trans{ glm::mat4( 1.0F ) };
        glm::vec4 vec_begin{ 1.0F, 0.0F, 0.0F, 1.0F };
        bool initialized{ false };

      public:
        TransformComponent() = delete;
        /**
         * @brief Constructor with an optional name.
         * @param componentName The name of the component.
         */
        explicit TransformComponent( const std::string& componentName = "transform", const float ident_matrix = 1.0F )
            : Component( componentName ), trans{ glm::mat4( ident_matrix ) } {
        }

        TransformComponent( const TransformComponent& )            = delete;
        TransformComponent( TransformComponent&& )                 = delete;
        TransformComponent& operator=( const TransformComponent& ) = delete;
        TransformComponent& operator=( TransformComponent&& )      = delete;

        ~TransformComponent() noexcept override = default;

        /**
         * @brief Initialize the component.
         */
        auto Initialize() noexcept -> void override {
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            trans     = glm::translate( trans, glm::vec3( 1.0F, 1.0F, 0.0F ) );
            vec_begin = trans * vec_begin;
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
            initialized = true;
        }

        /**
         * @brief Update the component.
         * Called every frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        auto Update( [[maybe_unused]] std::chrono::milliseconds deltaTime ) -> void override {
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

            trans = glm::translate( trans, glm::vec3( 0.5F, -0.5F, 0.0F ) );
            trans = glm::rotate( trans, static_cast< float >( glfwGetTime() ), glm::vec3( 0.0F, 0.0F, 1.0F ) );
            LOG_INFO( QuillPtr(), "hello world!" );
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        }

        /**
         * @brief Render the component.
         */
        auto Render() const noexcept -> void {
        }

        auto Translation() const noexcept -> const glm::mat< 4, 4, float >& {
            return trans;
        }
    };
} // namespace PeanutGL
