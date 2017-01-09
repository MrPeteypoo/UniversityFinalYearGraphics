#include "Renderer.hpp"


// Engine headers.
#include <scene/scene.hpp>


// Personal headers.
#include <Rendering/Renderer/Programs/Shaders.hpp>
#include <Utility/Algorithm.hpp>


void Renderer::setInternalResolution (const glm::ivec2& resolution) noexcept
{
    // Only change the resolution if it's different from the current value.
    if (m_resolution.internalWidth != resolution.x || m_resolution.internalHeight != resolution.y)
    {
        m_resolution.internalWidth  = resolution.x;
        m_resolution.internalHeight = resolution.y;

        // We'll need to rebuild the framebuffers due to a resolution changes.
        if (m_gbuffer.isInitialised() && m_lbuffer.isInitialised())
        {
            // This will also trigger the resetting of uniforms.
            buildFramebuffers();
            buildUniforms();
        }
    }
}


void Renderer::setDisplayResolution (const glm::ivec2& resolution) noexcept
{
    // Nothing extra needs doing as display resolution only effects the final blitting.
    m_resolution.displayWidth   = resolution.x;
    m_resolution.displayHeight  = resolution.y;
}


bool Renderer::initialise (scene::Context* scene, const glm::ivec2& internalRes, const glm::ivec2& displayRes) noexcept
{
    // The following objects have external dependancies:
    // Framebuffers:
    //  -   Internal resolution
    // Uniforms:
    //  -   Programs
    //  -   Materials
    //  -   Framebuffers
    // Geometry: 
    //  -   Materials
    //  -   Instancing buffers
    // InstanceID vector:
    //  -   Geometry
    
    // Make sure we keep areference to the scene.
    m_scene = scene;

    // Programs can be built immediately.
    if (!buildPrograms())
    {
        return false;
    }

    // Same with materials.
    if (!buildMaterials())
    {
        return false;
    }

    // And dynamic object buffers.
    if (!buildDynamicObjectBuffers())
    {
        return false;
    }

    // Aaaaaand light object buffers.
    if (!buildLightBuffers())
    {
        return false;
    }

    // With materials and instancing buffers done we can build the geometry.
    if (!buildGeometry())
    {
        return false;
    }

    // Set the resolutions.
    setInternalResolution (internalRes);
    setDisplayResolution (displayRes);

    // We can safely build the framebuffers now.
    if (!buildFramebuffers())
    {
        return false;
    }

    // With the framebuffers and materials built we can build the uniforms.
    if (!buildUniforms())
    {
        return false;
    }

    // Finally we've succeeded my lord!
    fillDynamicInstances();
    return true;
}


void Renderer::clean() noexcept
{
    m_scene = nullptr;
    m_programs.clean();
    m_dynamics.clear();
    m_materials.clean();
    m_objectDrawing.buffer.clean();
    m_materialIDs.clean();
    m_objectTransforms.clean();
    m_lightDrawing.buffer.clean();
    m_lightTransforms.clean();
    m_gbuffer.clean();
    m_lbuffer.clean();
    m_uniforms.clean();
    m_geometry.clean();
    m_resolution.internalWidth  = 0;
    m_resolution.internalHeight = 0;
    m_resolution.displayWidth   = 0;
    m_resolution.displayHeight  = 0;
}


bool Renderer::buildPrograms() noexcept
{
    // Firstly we must compile the shaders.
    auto shaders = Shaders { };
    
    if (!shaders.initialise())
    {
        return false;
    }

    // Next we can link the shaders together to create programs.
    return m_programs.initialise (shaders);
}


bool Renderer::buildMaterials() noexcept
{
    // As simple as initialising the materials.
    return m_materials.initialise (*m_scene, materialsStartingTextureUnit);
}


bool Renderer::buildDynamicObjectBuffers() noexcept
{
    // We need to find out how many dynamic instances there are.
    const auto& instances = m_scene->getAllInstances();

    // We need to track how many unique meshes there are and the total dynamic instance count.
    auto uniqueMeshes   = std::unordered_set<scene::MeshId> { };
    auto instanceCount  = size_t { 0 };

    std::for_each (instances, [&] (const scene::Instance& instance) 
    { 
        if (!instance.isStatic())
        {
            uniqueMeshes.emplace (instance.getMeshId());
            ++instanceCount;
        }
    });

    // Now we can allocate enough memory.
    const auto materialIDSize   = static_cast<GLsizeiptr> (instanceCount * sizeof (GLuint) * m_materialIDs.partitions);
    const auto transformSize    = static_cast<GLsizeiptr> (instanceCount * sizeof (glm::mat4x3) * m_objectTransforms.partitions);

    // Initialise the objects with the correct memory values.
    if (!(m_objectDrawing.buffer.initialise() && 
        m_materialIDs.initialise (materialIDSize, false, true, false) && 
        m_objectTransforms.initialise (transformSize, false, true, false)))
    {
        return false;
    }

    // Now set up the draw buffer.
    m_objectDrawing.buffer.allocateImmutableStorage (uniqueMeshes.size() * sizeof (MultiDrawElementsIndirectCommand));

    m_objectDrawing.capacity    = static_cast<GLuint> (uniqueMeshes.size());
    m_objectDrawing.count       = m_objectDrawing.capacity;

    // We're done!
    return true;
}


bool Renderer::buildLightBuffers() noexcept
{
    // We need to count the amount of lights that exist.
    const auto& point   = m_scene->getAllPointLights();
    const auto& spot    = m_scene->getAllSpotLights();

    // The final count must allow for the drawing of a full screen quad.
    const auto count            = point.size() + spot.size() + 1;
    const auto transformSize    = static_cast<GLsizeiptr> (count * sizeof (glm::mat4x3) * m_lightTransforms.partitions);
    
    // Now we can initialise the buffers
    if (!(m_lightDrawing.buffer.initialise() && 
        m_lightTransforms.initialise (transformSize, false, true, false)))
    {
        return false;
    }

    // Finally set up the draw buffer.
    m_lightDrawing.buffer.allocateImmutableStorage (lightVolumeCount * sizeof (MultiDrawElementsIndirectCommand));

    m_lightDrawing.capacity = static_cast<GLuint> (count);
    m_lightDrawing.count    = m_lightDrawing.capacity;

    // And we're done.
    return true;
}


bool Renderer::buildGeometry() noexcept
{
    // We need to collate the static instances first.
    const auto& instances   = m_scene->getAllInstances();
    auto staticInstances    = std::map<scene::MeshId, std::vector<scene::Instance>> { };

    std::for_each (instances, [&] (const scene::Instance& instance)
    {
        if (instance.isStatic())
        {
            staticInstances[instance.getMeshId()].push_back (instance);
        }
    });

    // Now we can try to initialise the geometry object.
    return m_geometry.initialise (m_materials, staticInstances, m_materialIDs, m_objectTransforms, m_lightTransforms);
}


bool Renderer::buildFramebuffers() noexcept
{
    // We need width and height values to initialise with.
    const auto width    = m_resolution.internalWidth;
    const auto height   = m_resolution.internalHeight;

    // Now we can initialise the framebuffers.
    return  m_gbuffer.initialise (width, height) &&
            m_lbuffer.initialise (m_gbuffer.getDepthStencilTexture(), GL_RGB8, width, height);
}


bool Renderer::buildUniforms() noexcept
{
    // Make sure the uniforms build correctly.
    if (!m_uniforms.initialise (m_gbuffer, m_materials))
    {
        return false;
    }

    // Now we can bind the uniform blocks to each program.
    m_uniforms.bindBlocksToProgram (m_programs);

    // And we're done!
    return true;
}


void Renderer::fillDynamicInstances() noexcept
{
    // Ensure the target vector is clean.
    m_dynamics.clear();

    // We need to iterate through mesh IDs and retrieve the dynamic instances for each.
    const auto& sceneMeshes = m_geometry.getMeshes();

    // Reserve enough memory to speed the process up.
    m_dynamics.reserve (sceneMeshes.size());

    for (const auto& pair : sceneMeshes)
    {
        // Retrieve the instances for the current mesh.
        const auto instances = m_scene->getInstancesByMeshId (pair.first);

        // We only want the dynamic instance IDs.
        auto dynamicIDs = std::vector<scene::InstanceId> { };
        dynamicIDs.reserve (instances.size());
        for (const auto& instance : instances)
        {
            if (!m_scene->getInstanceById (instance).isStatic())
            {
                dynamicIDs.push_back (instance);
            }
        }

        // Don't keep extra memory we don't need.
        dynamicIDs.shrink_to_fit();

        // Finally add the mesh if necessary.
        if (dynamicIDs.size() > 0)
        {
            m_dynamics.emplace_back (pair.second, std::move (dynamicIDs));
        }
    }

    // Finally remove any excess memory in the dynamic container.
    m_dynamics.shrink_to_fit();
}