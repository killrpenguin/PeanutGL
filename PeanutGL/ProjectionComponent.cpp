
#include "ProjectionComponent.hpp"

namespace PeanutGL {

    ProjectionComponent::ProjectionComponent( const std::string& componentName, const Width width, const Height height )
        : Component( componentName ), width{ width }, height{ height } {
        Initialize();
    }

    auto ProjectionComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ProjectionComponent::MatrixData() const noexcept -> glm::mat4 {
        return projection;
    }

    auto ProjectionComponent::Update( const std::chrono::milliseconds /*deltaTime*/ ) -> void {
        projection = glm::perspective(
            glm::radians( degrees() ), static_cast< float >( width() ) / static_cast< float >( height() ), zNear(),
            100.0F );
    }

    auto ProjectionComponent::Render() const noexcept -> void {
    }

} // namespace PeanutGL
