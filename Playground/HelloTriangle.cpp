#include "BufferResource.hpp"
#include "CameraComponent.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "EntryPoint.hpp"
#include "MaterialResource.hpp"
#include "MeshComponent.hpp"
#include "ModelComponent.hpp"
#include "ProjectionComponent.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "UtilityTypes.hpp"
#include "UtilityVerticies.hpp"
#include "VertexArray.hpp"
#include "ViewComponent.hpp"

#include <array>
#include <cassert>
#include <span>
#include <stdexcept>

namespace {
    // NOLINTBEGIN(misc-include-cleaner)

    using VertexType   = float;
    using TriangleVBO  = PeanutGL::PersistentVBO< VertexType >;
    using TriangleVAO  = PeanutGL::BasicVAO< VertexType >;
    using TriangleMesh = PeanutGL::MeshComponent< VertexType, unsigned int >;

    // clang-format off
	[[maybe_unused]] constexpr std::array<glm::vec3, 9> CubePositions {
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
    // NOLINTEND(misc-include-cleaner)

} // namespace

void PeanutGL::EngineSetup( Engine* const engine ) {
    const auto vert_shader{ engine->LoadResource< VertShader >( "TriangleVert" ) };
    const auto frag_shader{ engine->LoadResource< FragShader >( "TriangleFrag" ) };

    const auto VBO{ engine->LoadResource< TriangleVBO >( "VBO" ) };
    const auto VAO{ engine->LoadResource< TriangleVAO >( "VAO", VBO->Name(), Stride< VertexType >( 5 ) ) };

    const auto container{ engine->LoadResource< Texture2D >( "container.jpg", 0 ) };
    const auto awesomeface{ engine->LoadResource< Texture2D >( "awesomeface.png", 1, true ) };

    if ( vert_shader and frag_shader and VBO and VAO and container and awesomeface ) {
        Entity* const triangle_entity{ engine->CreateEntity( "Triangle" ) };

        const auto shader_program{ engine->LoadResource< ShaderProgram >(
            triangle_entity->GetName(), *vert_shader, *frag_shader ) };

        auto* const triangle_component{ triangle_entity->AddComponent< TriangleMesh >() };

        triangle_component->SetVertices( CubeVerticies );

        VBO->Write( triangle_component->GetVertices() );

        engine->SetActiveCamera( triangle_entity->AddComponent< CameraComponent >( "FlyingCamera" ) );

        auto* const models_array{ triangle_entity->AddComponent< ModelsArray >( "ModelsArray" ) };

        models_array->AddModel( "model" ); // Checks that the default cube position is 0.0F, 0.0F, 0.0F.

        for ( const auto& pos : CubePositions ) {
            models_array->AddModel( "model", pos );
        }

        for ( auto& value : *models_array ) {
            value.UseUniform();
        }

        auto* const view_component{ triangle_entity->AddComponent< ViewComponent >( "view" ) };
        view_component->UseUniform();

        auto* const projection_component{ triangle_entity->AddComponent< ProjectionComponent >(
            "projection", engine->GetPlatform()->GetWindowWidth(), engine->GetPlatform()->GetWindowHeight() ) };
        projection_component->UseUniform();

        /* Position, Texture */
        const std::span< const VertexBufferElement > vao_layout{ triangle_component->SetAttributes< float >(
            { 3, 2 } ) };

        VAO->SetLayout( vao_layout );

        shader_program->SetUniform( container );

        shader_program->SetUniform( awesomeface );

        triangle_entity->Initialize();

    } else {
        throw std::runtime_error( "Found invalid resource handle during engine setup." );
    }
}
