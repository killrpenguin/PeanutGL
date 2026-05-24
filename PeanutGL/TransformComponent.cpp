#include "TransformComponent.hpp"

namespace PeanutGL {
    TransformComponent::TransformComponent(
        // NOLINTNEXTLINE
        const ResourceHandle< ShaderProgram > handle, const std::string& componentName )
        : Component( componentName ), shader_program{ handle } {
        Initialize();
    }

    auto TransformComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto TransformComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
        // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        auto transform = glm::mat4( 1.0F );
        transform      = glm::translate( transform, glm::vec3( 0.5F, -0.5F, 0.0F ) );
        transform      = glm::rotate( transform, static_cast< float >( glfwGetTime() ), glm::vec3( 0.0F, 0.0F, 1.0F ) );
        // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

        if ( shader_program ) { shader_program->SetUniform( Component::GetName(), transform ); }
    }

    auto TransformComponent::Render() const noexcept -> void {
    }

} // namespace PeanutGL
