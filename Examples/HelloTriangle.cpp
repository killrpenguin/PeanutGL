#include "BufferResource.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "EntryPoint.hpp"
#include "MaterialResource.hpp"
#include "MeshComponent.hpp"
#include "ModelComponent.hpp"
#include "ViewComponent.hpp"
#include "ResourceBase.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "ProjectionComponent.hpp"
#include "Utilities.hpp"
#include "VertexArray.hpp"

#include <array>
#include <cassert>
#include <span>

namespace {
    // clang-format off
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	constexpr std::array< PeanutGL::MeshComponent::vertices_type, 180 > CubeVerticies{
			 -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,
     0.5F, -0.5F, -0.5F,  1.0F, 0.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 0.0F,

    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 1.0F,
    -0.5F,  0.5F,  0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,

    -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,

    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,
     0.5F, -0.5F, -0.5F,  1.0F, 1.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
     0.5F, -0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F, -0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F, -0.5F, -0.5F,  0.0F, 1.0F,

    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F,
     0.5F,  0.5F, -0.5F,  1.0F, 1.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
     0.5F,  0.5F,  0.5F,  1.0F, 0.0F,
    -0.5F,  0.5F,  0.5F,  0.0F, 0.0F,
    -0.5F,  0.5F, -0.5F,  0.0F, 1.0F
		   };
  
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    // clang-format on

} // namespace
void PeanutGL::EngineSetup( Engine* const engine ) {
    const auto vert_shader{ engine->LoadResource< VertShader >( "TriangleVert" ) };
    const auto frag_shader{ engine->LoadResource< FragShader >( "TriangleFrag" ) };

    const auto VBO{ engine->LoadResource< VertexBuffer >( "VBO" ) };

    const auto VAO{ engine->LoadResource< VertexArray >( "VAO", VBO->Name(), Stride( 5 ) ) };

    const auto container{ engine->LoadResource< Texture2D >( "container.jpg", 0 ) };
    const auto awesomeface{ engine->LoadResource< Texture2D >( "awesomeface.png", 1, true ) };

    if ( vert_shader and frag_shader and VBO and VAO and container and awesomeface ) {
        Entity* const triangle_entity{ engine->CreateEntity( "Triangle" ) };

        const auto shader_program{ engine->LoadResource< ShaderProgram >(
            "TriangleShader", *vert_shader, *frag_shader ) };

        auto* const triangle_component{ triangle_entity->AddComponent< MeshComponent >( VBO ) };

        //        (void)triangle_entity->AddComponent< TransformComponent >( shader_program );
        (void)triangle_entity->AddComponent< ModelComponent >( shader_program );
        (void)triangle_entity->AddComponent< ViewComponent >( shader_program );
        (void)triangle_entity->AddComponent< ProjectionComponent >(
            shader_program, engine->GetPlatform()->GetWindowWidth(), engine->GetPlatform()->GetWindowHeight() );

        /* Position, Color, Texture */
        const std::span< const VertexBufferElement > vao_layout{
            triangle_component->SetAttributes< VertexBuffer::BufferType >( { 3, 2 } )
        };

        VAO->SetLayout( vao_layout );

        shader_program->SetUniform( container );
        shader_program->SetUniform( awesomeface );

        triangle_component->SetVertices( CubeVerticies );

        triangle_component->SetIndices( { 0, 1, 3, 1, 2, 3 } );

        triangle_entity->Initialize();
    }
}
