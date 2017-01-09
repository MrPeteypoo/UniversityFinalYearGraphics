#include "Uniforms.hpp"


// Personal headers.
#include <Rendering/Renderer/Programs/Programs.hpp>


bool Uniforms::initialise (const GeometryBuffer& geometryBuffer, const Materials& materials) noexcept
{
    /*if (!m_ubo.initialise())
    {
        return false;
    }

    m_ubo.allocateImmutableStorage (sizeof (UniformBlocks));*/
    return true;
}


void Uniforms::clean() noexcept
{
    m_staticBlocks.clean();
    m_dynamicBlocks.clean();

    m_scene         = Scene { };
    m_directional   = DirectionalLights { };
    m_point         = PointLights { };
    m_spot          = Spotlights { };
}


void Uniforms::bindBlocksToProgram (const Programs& programs) const noexcept
{
    const auto bindAllBlocks = [&] (const Program& program)
    {
        bindBlockToProgram (program, Gbuffer::blockBinding);
        bindBlockToProgram (program, TextureArrays::blockBinding);
        bindBlockToProgram (program, Scene::blockBinding);
        bindBlockToProgram (program, DirectionalLights::blockBinding);
        bindBlockToProgram (program, PointLights::blockBinding);
        bindBlockToProgram (program, Spotlights::blockBinding);
    };
    
    bindAllBlocks (programs.geometry);
    bindAllBlocks (programs.globalLight);
    bindAllBlocks (programs.pointLight);
    bindAllBlocks (programs.spotlight);
}


void Uniforms::bindBlocksToPartition (const size_t partitionIndex) noexcept
{

}


void Uniforms::notifyModifiedDataRange (const GLintptr startOffset, const GLsizei length) noexcept
{
    m_dynamicBlocks.notifyModifiedDataRange (m_partition, startOffset, length);
}


void Uniforms::bindBlockToProgram (const Program& program, const GLuint blockBinding) const noexcept
{
    // Check if the program uses the block.
    const auto index = glGetUniformBlockIndex (program.getID(), blockNames.at (blockBinding));
    if (index == GL_INVALID_INDEX) 
    {
        return;
    }

    // Bind the block to the program.
    glUniformBlockBinding (program.getID(), index, blockBinding);
}