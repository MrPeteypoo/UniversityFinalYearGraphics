#include "Uniforms.hpp"


// Personal headers.
#include <Rendering/Renderer/Drawing/GeometryBuffer.hpp>
#include <Rendering/Renderer/Materials/Materials.hpp>
#include <Rendering/Renderer/Programs/Programs.hpp>
#include <Rendering/Renderer/Uniforms/DirectionalLight.hpp>
#include <Rendering/Renderer/Uniforms/FullBlock.hpp>
#include <Rendering/Renderer/Uniforms/PointLight.hpp>
#include <Rendering/Renderer/Uniforms/Scene.hpp>
#include <Rendering/Renderer/Uniforms/Spotlight.hpp>


// Initialise the static int.
GLint Uniforms::alignment = 0;


bool Uniforms::initialise (const GeometryBuffer& geometryBuffer, const Materials& materials) noexcept
{
    // Don't modify the current object unless we need to.
    auto staticBlocks   = decltype (m_staticBlocks) { };
    auto dynamicBlocks  = decltype (m_dynamicBlocks) { };

    // Ensure the buffers initialise.
    if (!(staticBlocks.initialise() && 
        m_dynamicBlocks.initialise (calculateDynamicBlockSize(), false, true, false)))
    {
        return false;
    }

    // Ensure we have a correct alignment value.
    glGetIntegerv (GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    // Fill the static block buffers.
    if (!buildStaticBlocks (staticBlocks, geometryBuffer, materials))
    {
        return false;
    }

    // We can make use of the data now.
    m_staticBlocks  = std::move (staticBlocks);
    m_dynamicBlocks = std::move (dynamicBlocks);

    // Don't forget to bind the blocks to the current partition.
    rebindStaticBlocks();
    bindBlocksToPartition (0);

    // Success!
    return true;
}


void Uniforms::clean() noexcept
{
    m_staticBlocks.clean();
    m_dynamicBlocks.clean();

    m_partition     = 0;
    m_scene         = decltype (m_scene) { };
    m_directional   = decltype (m_directional) { };
    m_point         = decltype (m_point) { };
    m_spot          = decltype (m_spot) { };
}


void Uniforms::bindBlocksToProgram (const Programs& programs) const noexcept
{
    const auto bindAllBlocks = [&] (const Program& program)
    {
        bindBlockToProgram (program, GBuffer::blockBinding);
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
    // Set the partition value.
    m_partition = partitionIndex;

    // Reset each block data object.
    resetBlockData();

    // Now rebind each dynamic buffer.
    rebindDynamicBlocks();
}


void Uniforms::notifyModifiedDataRange (const ModifiedRange& range) noexcept
{
    m_dynamicBlocks.notifyModifiedDataRange (m_partition, range);
}


GLintptr Uniforms::calculateDynamicBlockSize() const noexcept
{
    const auto sceneBlock       = calculateAlignedSize<decltype (m_scene.data)>();
    const auto dirLightBlock    = calculateAlignedSize<decltype (m_directional.data)>();
    const auto pointLightBlock  = calculateAlignedSize<decltype (m_point.data)>();
    const auto spotlightBlock   = calculateAlignedSize<decltype (m_spot.data)>();
    
    return sceneBlock + dirLightBlock + pointLightBlock + spotlightBlock;
}


bool Uniforms::buildStaticBlocks (Buffer& staticBlocks, 
            const GeometryBuffer& gbuffer, const Materials& materials) const noexcept
{
    // We don't want to allow access to glBufferSubData so we must place the data in a temporary buffer because the
    // alignment value can't be known at compile time.
    auto tempBuffer = Buffer { };
    if (!tempBuffer.initialise())
    {
        return false;
    }

    // We need to know how many texture arrays exist.
    auto textures       = Textures { };
    const auto first    = materials.getFirstTextureUnit() - GL_TEXTURE0;
    const auto last     = materials.getLastTextureUnit() - GL_TEXTURE0;
    
    // Retrieve the gbuffer data.
    textures.gbuffer.positions  = gbuffer.getPositionTexture().getDesiredTextureUnit();
    textures.gbuffer.normals    = gbuffer.getNormalTexture().getDesiredTextureUnit();
    textures.gbuffer.materials  = gbuffer.getMaterialTexture().getDesiredTextureUnit();

    // Retrieve the texture array data.
    for (auto unit = first; unit <= last; ++unit)
    {
        textures.arrays.arrays[unit - first] = unit;
    }

    // Now we can buffer the data.
    tempBuffer.immutablyFillWith (textures);

    // Taking into account alignment requirements we need to calculate offsets.
    const auto alignedOffset    = calculateAlignedSize<Textures::GBuffer>();
    const auto storageSize      = alignedOffset + sizeof (Textures::Arrays);

    // We don't need any client-side access to the memory so specify no flags.
    staticBlocks.allocateImmutableStorage (storageSize, 0);

    // Copy the data, taking into account the alignment requirements of the UBO blocks.
    glCopyNamedBufferSubData (tempBuffer.getID(), staticBlocks.getID(), 0, 0, sizeof (Textures::GBuffer));
    glCopyNamedBufferSubData (tempBuffer.getID(), staticBlocks.getID(), 
        sizeof (Textures::GBuffer), alignedOffset, sizeof (Textures::Arrays));

    // Success!
    return true;
}


void Uniforms::bindBlockToProgram (const Program& program, const GLuint blockBinding) const noexcept
{
    // Check if the program uses the block.
    const auto index = glGetUniformBlockIndex (program.getID(), m_blockNames.at (blockBinding));
    if (index == GL_INVALID_INDEX) 
    {
        return;
    }

    // Bind the block to the program.
    glUniformBlockBinding (program.getID(), index, blockBinding);
}


void Uniforms::resetBlockData() noexcept
{
    // Ask for a pointer to the specified partition.
    auto pointer = m_dynamicBlocks.pointer (m_partition);

    // Create a helper.
    const auto setBlockData = [=] (auto& block, const auto offset)
    {
        // Set the block parameters correctly.
        block.data      = (decltype (block.data)) (pointer + offset);
        block.offset    = offset;
    };

    // We also need the base offset.
    const auto baseOffset = m_dynamicBlocks.partitionOffset (m_partition);

    // Now set the value of each object.
    setBlockData (m_scene,          baseOffset);
    setBlockData (m_directional,    m_scene.offset          + calculateAlignedSize<decltype (m_scene.data)>());
    setBlockData (m_point,          m_directional.offset    + calculateAlignedSize<decltype (m_directional.data)>());
    setBlockData (m_spot,           m_point.offset          + calculateAlignedSize<decltype (m_point.data)>());
}


void Uniforms::rebindStaticBlocks() const noexcept
{
    // Only the texture blocks are static.
    glBindBufferRange (GL_UNIFORM_BUFFER, GBuffer::blockBinding, m_staticBlocks.getID(), 
        0, sizeof (Textures::GBuffer));

    glBindBufferRange (GL_UNIFORM_BUFFER, TextureArrays::blockBinding, m_staticBlocks.getID(), 
        calculateAlignedSize<Textures::GBuffer>(), sizeof (Textures::Arrays));
}


void Uniforms::rebindDynamicBlocks() const noexcept
{
    // We have four dynamic blocks.
    constexpr auto count = GLsizei { 4 };

    // They all exist in the same buffer.
    const auto buffer = m_dynamicBlocks.getID();

    // And they start at the index of the scene block.
    constexpr auto index = Scene::blockBinding;
    
    // Construct the parameters we need for glBindBuffersRange().
    const GLuint    buffers[count]  = { buffer, buffer, buffer, buffer };
    const GLintptr  offsets[count]  = { m_scene.offset, m_directional.offset, m_point.offset, m_spot.offset };
    const GLintptr  sizes[count]    = { sizeof (*m_scene.data), sizeof (*m_directional.data), sizeof (*m_point.data), sizeof (*m_spot.data) };

    // Bind each block.
    glBindBuffersRange (GL_UNIFORM_BUFFER, index, count, buffers, offsets, sizes);
}