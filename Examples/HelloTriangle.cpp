#include "BufferResource.hpp"
#include "Engine.hpp"
#include "Entity.hpp"
#include "EntryPoint.hpp"
#include "GeneratedConstants.hpp"
#include "MaterialResource.hpp"
#include "MeshComponent.hpp"
#include "ResourceBase.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"

#include <glad/gl.h>

#include <cassert>


void PeanutGL::EngineSetup( Engine* engine ) {
    const auto vert_shader{ engine->LoadResource< Shader >( "TriangleVert", GL_VERTEX_SHADER ) };
    const auto frag_shader{ engine->LoadResource< Shader >( "TriangleFrag", GL_FRAGMENT_SHADER ) };

    const auto VBO{ engine->LoadResource< VertexBufferResource >( "VBO" ) };
    const auto EBO{ engine->LoadResource< ElementBufferResource >( "EBO" ) };

    const auto Texture{ engine->LoadResource< Texture2D >(
        "awesomeface", std::string( ASSETS_ROOT ) + "/awesomeface.png" ) };

    // auto VAO {engine->LoadResource< VAOResource >( "VAO" ) };

    if ( vert_shader && frag_shader && VBO ) {
        Entity* triangle_entity{ engine->CreateEntity( "Triangle" ) };

        const ResourceHandle< ShaderProgram > shader{ engine->LoadResource< ShaderProgram >(
            "TriangleShader", *vert_shader, *frag_shader ) };

        MeshComponent* const triangle_component{ triangle_entity->AddComponent< MeshComponent >( VBO, EBO ) };

        // clang-format off
		// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
        triangle_component->SetVertices({
		       // positions          // colors 
           0.5F,  0.5F, 0.0F,    1.0F,  0.0F, 0.0F,  // top right
           0.5F, -0.5F, 0.0F,    0.0F, 1.0F, 0.0F, // bottom right
        -0.5F, -0.5F, 0.0F,   0.0F, 0.0F, 1.0F, // bottom left
        -0.5F,  0.5F, 0.0F,   1.0F, 1.0F, 0.0F, // top left 
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
