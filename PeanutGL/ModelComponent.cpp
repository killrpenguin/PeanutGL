#include "ModelComponent.hpp"

#include <ranges>

namespace PeanutGL {
    namespace {
        // clang-format off
	  constexpr std::array<glm::vec3, 10> CubePositions {
    glm::vec3( 0.0F,  0.0F,  0.0F), 
    glm::vec3( 2.0F,  5.0F, -15.0F), 
    glm::vec3(-1.5F, -2.2F, -2.5F),  
    glm::vec3(-3.8F, -2.0F, -12.3F),  
    glm::vec3( 2.4F, -0.4F, -3.5F),  
    glm::vec3(-1.7F,  3.0F, -7.5F),  
    glm::vec3( 1.3F, -2.0F, -2.5F),  
    glm::vec3( 1.5F,  2.0F, -2.5F), 
    glm::vec3( 1.5F,  0.2F, -1.5F), 
    glm::vec3(-1.3F,  1.0F, -1.5F)  
	  };
        // clang-format on
    } // namespace

    ModelComponent::ModelComponent(
        const ResourceHandle< ShaderProgram >& handle, const float degrees, const std::string& componentName )
        : Component( componentName ), shader_program{ handle }, degrees{ degrees } {
        Initialize();
    }

    auto ModelComponent::SetDegrees( const float val ) noexcept -> void {
        degrees = val;
    }

    auto ModelComponent::Initialize() noexcept -> void {
        SetState();
    }

    auto ModelComponent::Update( [[maybe_unused]] const std::chrono::milliseconds deltaTime ) -> void {
    }

    auto ModelComponent::Render() const noexcept -> void {
        for ( const auto [idx, position] : std::views::enumerate( std::views::as_const( CubePositions ) ) ) {
            auto model = glm::mat4( 1.0F );

            model = glm::translate( model, position );

            const float angle{ degrees * static_cast< float >( idx ) };

            constexpr float yVal{ 0.3F };
            constexpr float zVal{ 0.5F };
            model = glm::rotate(
                model, static_cast< float >( glfwGetTime() ) * glm::radians( angle ), glm::vec3( 1.0F, yVal, zVal ) );

            if ( shader_program ) { shader_program->SetUniform( Component::GetName(), model ); }

            constexpr GLsizei Count{ 36 };
            glDrawArrays( GL_TRIANGLES, 0, Count );
        }
    }
} // namespace PeanutGL
