#include "ViewComponent.hpp"

namespace PeanutGL {
    ViewComponent::ViewComponent(
        // NOLINTNEXTLINE
        const ResourceHandle< ShaderProgram >& handle, const float zAxis, const std::string& componentName )
        : Component( componentName ), shader_program{ handle }, zAxis{ zAxis } {
        Initialize();
    }

    auto ViewComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ViewComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
    }

    auto ViewComponent::Render() const noexcept -> void {
        auto view = glm::mat4( 1.0F );
        view      = glm::translate( view, glm::vec3( 0.0F, 0.0F, zAxis ) );

        if ( shader_program ) { shader_program->SetUniform( Component::GetName(), view ); }
    }

} // namespace PeanutGL
