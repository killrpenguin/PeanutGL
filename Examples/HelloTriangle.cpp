#include "BufferResource.hpp"
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
#include "Utilities.hpp"
#include "VertexArray.hpp"
#include "ViewComponent.hpp"

#include <cassert>
#include <span>
#include <stdexcept>

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

        auto* model{ triangle_entity->AddComponent< ModelComponent >( shader_program ) };
        constexpr float Degrees{ 10.0F };
        model->SetDegrees( Degrees );

        (void)triangle_entity->AddComponent< ViewComponent >( shader_program );
        (void)triangle_entity->AddComponent< ProjectionComponent >(
            shader_program, engine->GetPlatform()->GetWindowWidth(), engine->GetPlatform()->GetWindowHeight() );

        /* Position, Texture */
        const std::span< const VertexBufferElement > vao_layout{
            triangle_component->SetAttributes< VertexBuffer::BufferType >( { 3, 2 } )
        };

        VAO->SetLayout( vao_layout );

        shader_program->SetUniform( container );
        shader_program->SetUniform( awesomeface );

        triangle_component->SetVertices( CubeVerticies );

        triangle_component->SetIndices( { 0, 1, 3, 1, 2, 3 } );

        triangle_entity->Initialize();
    } else {
        throw std::runtime_error( "Found invalid resource handle during engine setup." );
    }
}
