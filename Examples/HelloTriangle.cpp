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

#include <cassert>
#include <span>
#include <stdexcept>

namespace {
    using VertexType   = float;
    using TriangleVBO  = PeanutGL::PersistentVBO< VertexType >;
    using TriangleVAO  = PeanutGL::BasicVAO< VertexType >;
    using TriangleMesh = PeanutGL::MeshComponent< VertexType, unsigned int >;
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
            "TriangleShader", *vert_shader, *frag_shader ) };

        auto* const triangle_component{ triangle_entity->AddComponent< TriangleMesh >() };

        triangle_component->SetVertices( CubeVerticies );

        VBO->Write( triangle_component->GetVertices() );

        auto* model{ triangle_entity->AddComponent< ModelComponent >( shader_program ) };

        constexpr float Degrees{ 10.0F };
        model->SetDegrees( Degrees );

        auto* const active_camera{ triangle_entity->AddComponent< CameraComponent >( shader_program ) };

        engine->SetActiveCamera( active_camera );

        (void)triangle_entity->AddComponent< ProjectionComponent >(
            shader_program, engine->GetPlatform()->GetWindowWidth(), engine->GetPlatform()->GetWindowHeight() );

        /* Position, Texture */
        const std::span< const VertexBufferElement > vao_layout{ triangle_component->SetAttributes< float >(
            { 3, 2 } ) };

        VAO->SetLayout( vao_layout );

        shader_program->SetUniform( container );
        shader_program->SetUniform( awesomeface );

        triangle_component->SetIndices( { 0, 1, 3, 1, 2, 3 } );

        triangle_entity->Initialize();
    } else {
        throw std::runtime_error( "Found invalid resource handle during engine setup." );
    }
}
