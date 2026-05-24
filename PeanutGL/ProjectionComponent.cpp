
#include "ProjectionComponent.hpp"

namespace PeanutGL {

    ProjectionComponent::ProjectionComponent(
        const ResourceHandle< ShaderProgram >& handle, const Width width, const Height height,
        const std::string& componentName )
        : Component( componentName ), width{ width }, height{ height }, shader_program{ handle } {
        Initialize();
    }
    auto ProjectionComponent::Initialize() noexcept -> void {
        constexpr float DefaultDegrees{ 45.0F };
        constexpr float zNear{ 0.1F };

        projection = glm::perspective(
            glm::radians( DefaultDegrees ), static_cast< float >( width() ) / static_cast< float >( height() ), zNear,
            100.0F );

        if ( shader_program ) { shader_program->SetUniform( Component::GetName(), projection ); }

        SetState();
    }
    auto ProjectionComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
    }

    auto ProjectionComponent::Render() const noexcept -> void {
    }
} // namespace PeanutGL
