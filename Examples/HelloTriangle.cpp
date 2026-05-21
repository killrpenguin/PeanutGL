#include "BufferResource.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "EntryPoint.hpp"
#include "MaterialResource.hpp"
#include "MeshComponent.hpp"
#include "ResourceBase.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "TransformComponent.hpp"
#include "Utilities.hpp"
#include "VertexArray.hpp"

#include <glad/gl.h>

#include <cassert>
#include <span>

void PeanutGL::EngineSetup( Engine* const engine ) {
    const auto vert_shader{ engine->LoadResource< Shader >( "TriangleVert", GL_VERTEX_SHADER ) };
    const auto frag_shader{ engine->LoadResource< Shader >( "TriangleFrag", GL_FRAGMENT_SHADER ) };

    const auto VBO{ engine->LoadResource< VertexBuffer >( "VBO" ) };
    const auto EBO{ engine->LoadResource< ElementBuffer >( "EBO" ) };

    const auto VAO{ engine->LoadResource< VertexArray >( "VAO", VBO->Name(), EBO->Name() ) };

    const auto container{ engine->LoadResource< Texture2D >( "container.jpg", 0 ) };
    const auto awesomeface{ engine->LoadResource< Texture2D >( "awesomeface.png", 1, true ) };

    if ( vert_shader and frag_shader and VBO and EBO and VAO and container and awesomeface ) {
        Entity* const triangle_entity{ engine->CreateEntity( "Triangle" ) };

        const auto shader_program{ engine->LoadResource< ShaderProgram >(
            "TriangleShader", *vert_shader, *frag_shader ) };

        auto* const triangle_component{ triangle_entity->AddComponent< MeshComponent >( VBO, EBO ) };

        auto* const move_component{ triangle_entity->AddComponent< TransformComponent >( "transform", 1.0F ) };

        /* Position, Color, Texture */
        const std::span< const VertexBufferElement > vao_layout{
            triangle_component->SetAttributes< VertexBuffer::BufferType >( { 3, 2 } )
        };

        VAO->SetLayout( vao_layout );

        shader_program->SetUniform( container );
        shader_program->SetUniform( awesomeface );
        shader_program->SetUniform( move_component->GetName(), move_component->Translation() );

        // clang-format off
		// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        triangle_component->SetVertices({
		        // positions          // texture coords
         0.5F,  0.5F, 0.0F,   1.0F, 1.0F, // top right
         0.5F, -0.5F, 0.0F,   1.0F, 0.0F, // bottom right
        -0.5F, -0.5F, 0.0F,   0.0F, 0.0F, // bottom left
        -0.5F,  0.5F, 0.0F,   0.0F, 1.0F  // top left 
    	});

		triangle_component->SetIndices({
		  0, 1, 3,
		  1, 2, 3
		});

		// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        // clang-format on

        triangle_entity->Initialize();
    }
}
