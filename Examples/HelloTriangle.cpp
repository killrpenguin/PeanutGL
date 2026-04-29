#include "Engine.hpp"
#include "Entity.hpp"
#include "EntryPoint.hpp"
#include "MeshComponent.hpp"
#include "MeshResource.hpp"
#include "ResourceBase.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"

#include <cassert>
#include <glad/gl.h>

void PeanutGL::EngineSetup( Engine* engine ) {
    const ResourceHandle< Shader > vert_shader{ engine->LoadResource< Shader >( "TriangleVert", GL_VERTEX_SHADER ) };
    const ResourceHandle< Shader > frag_shader{ engine->LoadResource< Shader >( "TriangleFrag", GL_FRAGMENT_SHADER ) };

    const auto triangle_mesh_resource{ engine->LoadResource< Mesh >( "TriangleMesh" ) };

    if ( vert_shader && frag_shader && triangle_mesh_resource ) {
        Entity* triangle_entity{ engine->CreateEntity( "Triangle" ) };

        const ResourceHandle< ShaderProgram > shader{ engine->LoadResource< ShaderProgram >(
            "TriangleShader", *vert_shader, *frag_shader ) };

        auto* triangle_component{ triangle_entity->AddComponent< MeshComponent >( triangle_mesh_resource ) };

        // clang-format off
	    // NOLINTBEGIN
        triangle_component->SetVertices({
	       0.5F, -0.5F, 0.0F,  1.0F, 0.0F, 0.0F,   // bottom right
           -0.5F, -0.5F, 0.0F,  0.0F, 1.0F, 0.0F,   // bottom left
           0.0F,  0.5F, 0.0F,  0.0F, 0.0F, 1.0F    // top 
    	});
        // NOLINTEND
        // clang-format on

        triangle_entity->Initialize();
    }
}

/*
  auto vert_shader { engine->LoadResource< Shader >( "basic_vert", TRIANGLE_VERT_SOURCE ) };
  auto frag_shader { engine->LoadResource< Shader >( "basic_frag", TRIANGLE_FRAG_SOURCE ) };

  auto vbo { engine->LoadResource< VertexBuffer >( "VBO" ) };
  auto vao { engine->LoadResource< VertexArray >( "VAO" ) };

  if (vbo && vao && vert_shader && frag_shader) {
      Entity* triangle_entity{ engine->CreateEntity( "Triangle" ) };

      engine->LoadResource< ShaderProgram >( vert_shader, frag_shader);

      auto* triangle_component { triangle_entity->AddComponent< MeshComponent >( vbo.Get(), vao.Get() ) };

      triangle_component->SetVerticies({
          //  data here.
      });

      triangle_entity->Initialize();
  }

*/
