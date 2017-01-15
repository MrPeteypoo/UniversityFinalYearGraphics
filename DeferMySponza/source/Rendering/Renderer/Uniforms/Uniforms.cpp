#include "Uniforms.hpp"


// Personal headers.
#include <Rendering/Renderer/Drawing/GeometryBuffer.hpp>
#include <Rendering/Renderer/Materials/Materials.hpp>
#include <Rendering/Renderer/Programs/Programs.hpp>
#include <Rendering/Renderer/Uniforms/Blocks/Scene.hpp>
#include <Rendering/Renderer/Uniforms/Blocks/FullBlock.hpp>
#include <Rendering/Renderer/Uniforms/Components/DirectionalLight.hpp>
#include <Rendering/Renderer/Uniforms/Components/PointLight.hpp>
#include <Rendering/Renderer/Uniforms/Components/Spotlight.hpp>


// Initialise the static int.
GLint Uniforms::alignment = 0;


bool Uniforms::initialise (const GeometryBuffer& geometryBuffer, const Materials& materials) noexcept
{
    // Ensure we have a correct alignment value.
    glGetIntegerv (GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    // Don't modify the current object unless we need to.
    auto blocks = decltype (m_blocks) { };

    // Ensure the buffers initialise.
    if (!blocks.initialise (calculateBlockSize(), false, false))
    {
        return false;
    }

    // We can make use of the data now.
    m_blocks = std::move (blocks);

    // Don't forget to bind the blocks to the current partition.
    bindBlocksToPartition (0);
    retrieveSamplerData (m_samplers, geometryBuffer, materials);

    // Success!
    return true;
}


void Uniforms::clean() noexcept
{
    m_blocks.clean();

    m_scene         = decltype (m_scene) { };
    m_directional   = decltype (m_directional) { };
    m_point         = decltype (m_point) { };
    m_spot          = decltype (m_spot) { };
}


void Uniforms::bindUniformsToPrograms (const Programs& programs) const noexcept
{
    // Construct a vector with the texture unit values of each texture array.
    const auto count    = m_samplers.textureSamplerCount;
    auto start          = m_samplers.textures.unit;
    auto values         = std::vector<GLint> (count);

    std::generate (std::begin (values), std::end (values), [&] () { return start++; });

    // Construct lambdas to add each uniform and all blocks.
    const auto bindSampler = [] (const Program& program, const Samplers::Sampler& sampler)
    {
        const auto location = glGetUniformLocation (program.getID(), sampler.name);
        
        if (location >= 0)
        {
            glProgramUniform1i (program.getID(), location, sampler.unit);
        }
    };

    const auto bindAllBlocks = [&, count] (const Program& program)
    {
        // Bind each block.
        bindBlockToProgram (program, Scene::blockBinding);
        bindBlockToProgram (program, DirectionalLights::blockBinding);
        bindBlockToProgram (program, PointLights::blockBinding);
        bindBlockToProgram (program, Spotlights::blockBinding);

        // Bind the individual uniforms.
        bindSampler (program, m_samplers.gbufferPositions);
        bindSampler (program, m_samplers.gbufferPositions);
        bindSampler (program, m_samplers.gbufferPositions);
        bindSampler (program, m_samplers.gbufferPositions);

        // And finally the texture arrays.
        const auto location = glGetUniformLocation (program.getID(), m_samplers.textures.name);

        if (location >= 0)
        {
            glProgramUniform1iv (program.getID(), location, count, values.data());
        }
    };

    programs.performActionOnPrograms (bindAllBlocks);
}


void Uniforms::bindBlocksToPartition (const size_t partitionIndex) noexcept
{
    // Reset each block data object.
    resetBlockData (partitionIndex);

    // Now rebind each dynamic buffer.
    rebindDynamicBlocks();
}


GLintptr Uniforms::calculateBlockSize() const noexcept
{
    const auto sceneBlock       = calculateAlignedSize<decltype (m_scene.data)>();
    const auto dirLightBlock    = calculateAlignedSize<decltype (m_directional.data)>();
    const auto pointLightBlock  = calculateAlignedSize<decltype (m_point.data)>();
    const auto spotlightBlock   = calculateAlignedSize<decltype (m_spot.data)>();
    
    return sceneBlock + dirLightBlock + pointLightBlock + spotlightBlock;
}


void Uniforms::retrieveSamplerData (Samplers& samplers, 
            const GeometryBuffer& gbuffer, const Materials& materials) const noexcept
{
    // Retrieve the gbuffer data.
    samplers.gbufferPositions.unit  = gbuffer.getPositionTexture().getDesiredTextureUnit();
    samplers.gbufferNormals.unit    = gbuffer.getNormalTexture().getDesiredTextureUnit();
    samplers.gbufferMaterials.unit  = gbuffer.getMaterialTexture().getDesiredTextureUnit();

    // Retrieve the sampler data.
    samplers.materials.unit         = materials.getMaterialTextureUnit();
    samplers.textures.unit          = materials.getTextureArrayStartingUnit();
    samplers.textureSamplerCount    = materials.getTextureArrayCount();
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


void Uniforms::resetBlockData (const size_t partition) noexcept
{
    // Ask for a pointer to the specified partition.
    auto pointer = m_blocks.pointer (partition);

    // Create a helper.
    const auto setBlockData = [=] (auto& block, const auto offset)
    {
        // Set the block parameters correctly.
        block.data      = (decltype (block.data)) (pointer + offset);
        block.offset    = offset;
    };

    // We also need the base offset.
    const auto baseOffset = m_blocks.partitionOffset (partition);

    // Now set the value of each object.
    setBlockData (m_scene,          baseOffset);
    setBlockData (m_directional,    m_scene.offset          + calculateAlignedSize<decltype (m_scene.data)>());
    setBlockData (m_point,          m_directional.offset    + calculateAlignedSize<decltype (m_directional.data)>());
    setBlockData (m_spot,           m_point.offset          + calculateAlignedSize<decltype (m_point.data)>());
}


void Uniforms::rebindDynamicBlocks() const noexcept
{
    // We have four dynamic blocks.
    const auto count = static_cast<GLsizei> (m_blockNames.size());

    // They all exist in the same buffer.
    const auto buffer = m_blocks.getID();

    // And they start at the index of the scene block.
    constexpr auto index = Scene::blockBinding;
    
    // Construct the parameters we need for glBindBuffersRange().
    const GLuint    buffers[]  = { buffer, buffer, buffer, buffer };
    const GLintptr  offsets[]  = { m_scene.offset, m_directional.offset, m_point.offset, m_spot.offset };
    const GLintptr  sizes[]    = { sizeof (*m_scene.data), sizeof (*m_directional.data), sizeof (*m_point.data), sizeof (*m_spot.data) };

    // Ensure we have valid sizes.
    assert (sizeof (buffers) / sizeof (GLuint) == count && 
            sizeof (offsets) / sizeof (GLintptr) == count && 
            sizeof (sizes) / sizeof (GLintptr) == count);

    // Bind each block.
    glBindBuffersRange (GL_UNIFORM_BUFFER, index, count, buffers, offsets, sizes);
}