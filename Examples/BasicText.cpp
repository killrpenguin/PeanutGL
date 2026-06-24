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
#include "VertexArray.hpp"
#include "ViewComponent.hpp"

#include <array>
#include <stdexcept>

namespace {

    struct CPUData {
        std::array< float, 3 > position;
        std::array< float, 3 > color;
        std::array< float, 2 > tex;
    };

    constexpr std::array< CPUData, 4 > Verticies{
        CPUData{ .position = { 0.5F, 0.5F, 0.0F }, .color = { 1.0F, 0.0F, 0.0F }, .tex = { 1.0F, 1.0F } },
        CPUData{ .position = { 0.5F, -0.5F, 0.0F }, .color = { 0.0F, 1.0F, 0.0F }, .tex = { 1.0F, 0.0F } },
        CPUData{ .position = { -0.5F, -0.5F, 0.0F }, .color = { 0.0F, 0.0F, 1.0F }, .tex = { 0.0F, 0.0F } },
        CPUData{ .position = { -0.5F, 0.5F, 0.0F }, .color = { 1.0F, 1.0F, 0.0F }, .tex = { 0.0F, 1.0F } }
    };

} // namespace

void PeanutGL::EngineSetup( Engine* const engine ) {
    const auto vert_shader{ engine->LoadResource< VertShader >( "BasicTextVert" ) };
    const auto frag_shader{ engine->LoadResource< FragShader >( "BasicTextFrag" ) };

    const auto SSBO{ engine->LoadResource< SSBOResource< CPUData > >( "ShaderStorageBuffer" ) };

    const auto VAO{ engine->LoadResource< EmptyVAO< CPUData > >( "VAO", SSBO->Name() ) };

    const auto container{ engine->LoadResource< Texture2D >( "container.jpg", 0 ) };

    if ( vert_shader and frag_shader and SSBO and container ) {
        Entity* const text_entity{ engine->CreateEntity( "Text" ) };

        const auto shader_program{ engine->LoadResource< ShaderProgram >(
            text_entity->GetName(), *vert_shader, *frag_shader ) };

        auto* const mesh{ text_entity->AddComponent< MeshComponent< CPUData, unsigned int > >() };

        mesh->SetVertices( Verticies );

        SSBO->Write( mesh->GetVertices() );

        engine->SetActiveCamera( text_entity->AddComponent< CameraComponent >( "FlyingCamera" ) );

        (void)text_entity->AddComponent< ModelComponent >( "model" );

        (void)text_entity->AddComponent< ViewComponent >( "view" );

        (void)text_entity->AddComponent< ProjectionComponent >(
            "projection", engine->GetPlatform()->GetWindowWidth(), engine->GetPlatform()->GetWindowHeight() );

        text_entity->Initialize();
    } else {
        throw std::runtime_error( "Found invalid resource handle during engine setup." );
    }
}

//  LocalWords:  VAO
