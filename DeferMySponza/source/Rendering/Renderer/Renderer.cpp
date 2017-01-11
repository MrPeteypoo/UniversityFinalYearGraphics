#include "Renderer.hpp"


// STL headers.
#include <future>


// Engine headers.
#include <scene/scene.hpp>


// Personal headers.
#include <Rendering/Binders/BufferBinder.hpp>
#include <Rendering/Binders/FramebufferBinder.hpp>
#include <Rendering/Binders/VertexArrayBinder.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>
#include <Utility/Algorithm.hpp>


struct Renderer::ASyncActions final
{
    using Action        = std::future<ModifiedRange>;
    using MultiAction   = std::future<ModifiedRanges>;

    Action      sceneUniforms, 
                dynamicTransforms, dynamicMaterialIDs, dynamicDrawCommands,
                lightDrawCommands, directionalLights;

    MultiAction pointLights, spotLights;
};


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
    m_objectMaterialIDs.clean();
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
    m_deferredRender            = true;

    for (auto& sync : m_syncs) 
    {
        sync = 0;
    }
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
    const auto drawCommandSize  = static_cast<GLsizeiptr> (uniqueMeshes.size() * sizeof (MultiDrawElementsIndirectCommand));
    const auto materialIDSize   = static_cast<GLsizeiptr> (instanceCount * sizeof (GLuint));
    const auto transformSize    = static_cast<GLsizeiptr> (instanceCount * sizeof (glm::mat4x3));

    // Initialise the objects with the correct memory values.
    if (!(m_objectDrawing.buffer.initialise (drawCommandSize, false, true, false) &&
        m_objectMaterialIDs.initialise (materialIDSize, false, true, false) && 
        m_objectTransforms.initialise (transformSize, false, true, false)))
    {
        return false;
    }

    // Now set up the draw buffer and we're done.
    m_objectDrawing.capacity    = static_cast<GLuint> (uniqueMeshes.size());
    m_objectDrawing.count       = 0;
    return true;
}


bool Renderer::buildLightBuffers() noexcept
{
    // We need to count the amount of lights that exist.
    const auto& point   = m_scene->getAllPointLights();
    const auto& spot    = m_scene->getAllSpotLights();

    // The final count must allow for the drawing of a full screen quad.
    const auto count            = point.size() + spot.size() + 1;
    const auto transformSize    = static_cast<GLsizeiptr> (count * sizeof (glm::mat4x3));
    const auto drawCommandSize  = static_cast<GLsizeiptr> (lightVolumeCount * sizeof (MultiDrawElementsIndirectCommand));
    
    // Now we can initialise the buffers
    if (!(m_lightDrawing.buffer.initialise (drawCommandSize, false, true, false) && 
        m_lightTransforms.initialise (transformSize, false, true, false)))
    {
        return false;
    }

    // Finally set up the draw buffer.
    m_lightDrawing.capacity = static_cast<GLuint> (count);
    m_lightDrawing.count    = 0;
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
    return m_geometry.initialise (m_materials, staticInstances, m_objectMaterialIDs, m_objectTransforms, m_lightTransforms);
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


void Renderer::render() noexcept
{
    // We must ensure that we aren't writing to data which the GPU is currently reading from. We must avoid this race
    // condition by checking if the most recent frame that used the current partition has finished accessing the
    // memory.
    syncWithGPUIfNecessary();

    // Now we can set the correct partition on the uniforms.
    m_uniforms.bindBlocksToPartition (m_partition);

    // We need to retrieve light data before we can render.
    const auto& directional = m_scene->getAllDirectionalLights();
    const auto& point       = m_scene->getAllPointLights();
    const auto& spot        = m_scene->getAllSpotLights();

    // Light transforms also need an offset so they're added after the full-screen quad transform.
    constexpr auto transformOffset = size_t { 1 };

    // We can safely multithread the data streaming operations.
    auto actions        = ASyncActions { };
    const auto policy   = m_multiThreaded ? std::launch::async : std::launch::deferred;

    // Now execute the asynchonous tasks.
    actions.sceneUniforms       = std::async (policy, [&]() { return updateSceneUniforms(); });
    actions.dynamicDrawCommands = std::async (policy, [&]() { return updateDynamicObjectDrawCommands(); });
    actions.dynamicTransforms   = std::async (policy, [&]() { return updateDynamicObjectTransforms(); });
    actions.dynamicMaterialIDs  = std::async (policy, [&]() { return updateDynamicObjectMaterialIDs(); });
    actions.directionalLights   = std::async (policy, [&]() { return updateDirectionalLights (directional); });
    actions.pointLights         = std::async (policy, [&]() { return updatePointLights (point, transformOffset); });
    actions.spotLights          = std::async (policy, [&]() { return updateSpotlights (spot, transformOffset); });

    // We only need to fill the light draw command buffer if we're doing a deferred render.
    if (m_deferredRender)
    {
        actions.lightDrawCommands = std::async (policy, [&]() 
        { 
            return updateLightDrawCommands (point.size(), spot.size()); 
        });
    }

    // Now perform universal rendering actions. Start by ensuring each material texture unit is bound.
    m_materials.bindTextures();

    // We need to configure the scene VAO for rendering static objects.
    auto& sceneVAO = m_geometry.getSceneVAO();
    sceneVAO.useStaticBuffers();

    // Ensure the VAO is bound.
    const auto vaoBinder = VertexArrayBinder { sceneVAO.vao };

    if (m_deferredRender)
    {
        deferredRender (sceneVAO, actions, directional.size(), point.size(), spot.size());
    }

    else
    {
        forwardRender (sceneVAO, actions);
    }

    // Render to the screen.
    glBlitNamedFramebuffer (m_lbuffer.getColourBuffer().getID(), 0,
        0, 0, m_resolution.internalWidth, m_resolution.internalHeight,
        0, 0, m_resolution.displayWidth, m_resolution.displayHeight, 
        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Cleanup.
    m_materials.unbindTextures();

    // Prepare for the next frame, the fence sync only allows the given parameters.
    m_syncs[m_partition] = glFenceSync (GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    m_partition = ++m_partition % multiBuffering;
}