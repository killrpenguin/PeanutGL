#include "ModelComponent.hpp"

namespace PeanutGL {

    ModelComponent::ModelComponent( const std::string& componentName )
        : Component( componentName ) {
        Initialize();
    }

    auto ModelComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ModelComponent::MatrixData() const noexcept -> glm::mat4 {
        return model;
    }

    auto ModelComponent::Update( const std::chrono::milliseconds /*deltaTime*/ ) -> void {
        model = glm::translate( model, position );

        model = glm::rotate( model, glm::radians( angle ), rotation_axis );
    }

    auto ModelComponent::Render() const noexcept -> void {
        constexpr GLsizei Count{ 36 };
        glDrawArrays( GL_TRIANGLES, 0, Count );
    }
} // namespace PeanutGL
