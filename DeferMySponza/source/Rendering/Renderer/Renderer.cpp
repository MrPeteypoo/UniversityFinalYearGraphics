#include "Renderer.hpp"


// STL headers.
#include <cassert>
#include <chrono>
#include <future>


// Engine headers.
#include <glm/gtc/matrix_transform.hpp>
#include <scene/scene.hpp>


// Personal headers.
#include <Rendering/Binders/BufferBinder.hpp>
#include <Rendering/Binders/FramebufferBinder.hpp>
#include <Rendering/Binders/VertexArrayBinder.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>
#include <Rendering/Renderer/Uniforms/DirectionalLight.hpp>
#include <Rendering/Renderer/Uniforms/FullBlock.hpp>
#include <Rendering/Renderer/Uniforms/PointLight.hpp>
#include <Rendering/Renderer/Uniforms/Scene.hpp>
#include <Rendering/Renderer/Uniforms/Spotlight.hpp>
#include <Utility/Algorithm.hpp>
#include <Utility/Scene.hpp>


// Namespaces.
using namespace std::chrono_literals;


struct Renderer::ASyncActions final
{
    using Action                = std::future<ModifiedRange>;
    using DynamicObjectAction   = std::future<ModifiedDynamicObjectRanges>;
    using LightVolumeAction     = std::future<ModifiedLightVolumeRanges>;

    Action              sceneUniforms, lightDrawCommands, directionalLights;
    DynamicObjectAction dynamicObjects;
    LightVolumeAction   pointLights, spotLights;
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
    // After updating the resolution we need to update the viewport.
    m_resolution.displayWidth   = resolution.x;
    m_resolution.displayHeight  = resolution.y;
    
    glViewport (0, 0, resolution.x, resolution.y);
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
    std::for_each (m_syncs, [] (auto& sync) { sync.clean(); });
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
    if (!(m_objectDrawing.buffer.initialise (drawCommandSize, false, false) &&
        m_objectMaterialIDs.initialise (materialIDSize, false, false) && 
        m_objectTransforms.initialise (transformSize, false, false)))
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
    if (!(m_lightDrawing.buffer.initialise (drawCommandSize, false, false) && 
        m_lightTransforms.initialise (transformSize, false, false)))
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
    return  m_gbuffer.initialise (width, height, gbufferStartingTextureUnit) &&
            m_lbuffer.initialise (m_gbuffer.getDepthStencilTexture(), GL_RGB8, width, height);
}


bool Renderer::buildUniforms() noexcept
{
    // Make sure the uniforms build correctly.
    if (!m_uniforms.initialise (m_gbuffer, m_materials))
    {
        return false;
    }

    // Now we can bind the uniform blocks to each program and we're done!
    m_uniforms.bindBlocksToProgram (m_programs);
    return true;
}


void Renderer::fillDynamicInstances() noexcept
{
    // We need to iterate through mesh IDs and retrieve the dynamic instances for each.
    const auto& sceneMeshes = m_geometry.getMeshes();
    m_dynamics.clear();
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
    actions.dynamicObjects      = std::async (policy, [&]() { return updateDynamicObjects(); });
    actions.directionalLights   = std::async (policy, [&]() { return updateDirectionalLights (directional); });
    actions.pointLights         = std::async (policy, [&]() { return updatePointLights (point, transformOffset); });
    actions.spotLights          = std::async (policy, [&]() { return updateSpotlights (spot, transformOffset); });

    // We only need to fill the light draw command buffer if we're doing a deferred render.
    if (m_deferredRender)
    {
        actions.lightDrawCommands = std::async (policy, [&]() 
        { 
            return updateLightDrawCommands (static_cast<GLuint> (point.size()), static_cast<GLuint> (spot.size())); 
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
    if (!m_syncs[m_partition++].initialise())
    {
        assert (false);
    }

    m_partition %= multiBuffering;
}


void Renderer::deferredRender (SceneVAO& sceneVAO, const ASyncActions& actions, 
    const size_t directionalLights, const size_t pointLights, const size_t spotlights) noexcept
{

}


void Renderer::forwardRender (SceneVAO& sceneVAO, const ASyncActions& actions) noexcept
{

}


void Renderer::syncWithGPUIfNecessary() const noexcept
{
    // Don't attempt to wait if the sync object hasn't been initialised.
    auto& sync = m_syncs[m_partition];

    if (sync.isInitialised())
    {
        // Don't force a wait if we don't need to.
        if (!sync.checkIfSignalled())
        {
            // We have to force a wait so we don't cause a data race.
            constexpr auto oneSecond = std::chrono::duration_cast<std::chrono::nanoseconds> (1s).count();
            assert (sync.waitForSignal (true, oneSecond));
        }
    }
}


ModifiedRange Renderer::updateSceneUniforms() noexcept
{
    // Retrieve the pointer to the uniforms so we can modify them.
    auto scene = m_uniforms.getWritableSceneData();

    // We'll need the camera to modify the data and we need to calculate the aspect ratio
    const auto& camera      = m_scene->getCamera();
    const auto camPosition  = util::toGLM (camera.getPosition());
    const auto camDirection = util::toGLM (camera.getDirection());
    const auto upDirection  = util::toGLM (m_scene->getUpDirection());
    const auto aspectRatio  = m_resolution.internalWidth / static_cast<float> (m_resolution.internalHeight);

    // Now we can write the data.
    scene.data->projection = glm::perspective (glm::radians (camera.getVerticalFieldOfViewInDegrees()), aspectRatio, 
        camera.getNearPlaneDistance(), camera.getFarPlaneDistance());

    scene.data->view            = glm::lookAt (camPosition, camPosition + camDirection, upDirection);
    scene.data->cameraPosition  = camPosition;
    scene.data->ambience        = util::toGLM (m_scene->getAmbientLightIntensity());

    return { scene.offset, sizeof (Scene) };
}


Renderer::ModifiedDynamicObjectRanges Renderer::updateDynamicObjects() noexcept
{
    // Retrieve the necessary pointers. We also need to keep track of how many instances there are.
    auto drawCommandBuffer  = (MultiDrawElementsIndirectCommand*) m_objectDrawing.buffer.pointer (m_partition);
    auto transformBuffer    = (glm::mat4x3*) m_objectTransforms.pointer (m_partition);
    auto materialIDBuffer   = (MaterialID*) m_objectMaterialIDs.pointer (m_partition);
    auto baseInstance       = GLuint { 0 };

    // Create lambda functions to update the data.
    const auto addDrawCommand = [&] (const auto index, const Mesh& mesh, const MeshInstances::Instances& instances)
    {
        const auto instanceCount = static_cast<GLuint> (instances.size());
        drawCommandBuffer[index] = { mesh.elementCount, instanceCount, mesh.elementsIndex, mesh.verticesIndex, baseInstance };

        // Ensure we increment the base instance.
        baseInstance += instanceCount;
    };

    const auto addTransform = [&] (const auto index, const scene::Instance& instance)
    {
        transformBuffer[index]  = util::toGLM (instance.getTransformationMatrix());
    };

    const auto addMaterialID = [&] (const auto index, const scene::Instance& instance)
    {
        materialIDBuffer[index] = m_materials[instance.getMaterialId()];
    };

    const auto addTransformAndMaterialID = [&] (const auto index, const scene::Instance& instance)
    {
        addTransform (index, instance);
        addMaterialID (index, instance);
    };

    // If we're on a single thread we should just iterate through the list once otherwise we may reduce performance.
    if (!m_multiThreaded)
    {
        forEachDynamicMeshInstance (addTransformAndMaterialID, addDrawCommand);
    }

    // Distribute the load with multiple cores. We'll iterate the contents multiple times but it should be faster.
    else
    {
        const auto transforms   = std::async (std::launch::async, [&] { forEachDynamicMeshInstance (addTransform); });
        const auto materialIDs  = std::async (std::launch::async, [&] { forEachDynamicMeshInstance (addMaterialID); });
        forEachDynamicMesh (addDrawCommand);
        transforms.wait();
        materialIDs.wait();
    }

    // Now configure the draw commands and return our modified data ranges.
    m_objectDrawing.count = static_cast<GLsizei> (m_dynamics.size());
    return 
    { 
        { m_objectDrawing.buffer.partitionOffset (m_partition), static_cast<GLsizeiptr> (sizeof (MultiDrawElementsIndirectCommand) * m_objectDrawing.count) },
        { m_objectTransforms.partitionOffset (m_partition),     static_cast<GLsizeiptr> (sizeof (glm::mat4x3) * baseInstance) },
        { m_objectMaterialIDs.partitionOffset (m_partition),    static_cast<GLsizeiptr> (sizeof (MaterialID) * baseInstance) }
    };
}


ModifiedRange Renderer::updateLightDrawCommands (const GLuint pointLights, const GLuint spotlights) noexcept
{
    // The first index is dedicated to the a full-screen quad. We also need the pointer to write to the buffer.
    constexpr auto quads    = GLuint { 1 };
    const auto bufferOffset = m_lightDrawing.buffer.partitionOffset (m_partition);
    auto lightCommands      = (MultiDrawElementsIndirectCommand*) m_lightDrawing.buffer.pointer (m_partition);

    // Cache the shape meshes.
    const auto& quad    = m_geometry.getQuad();
    const auto& sphere  = m_geometry.getSphere();
    const auto& cone    = m_geometry.getCone();

    // Finally add each draw command for the light volumes.
    lightCommands[0] = { quad.elementCount, quads, quad.elementsIndex, quad.verticesIndex, 0 };
    lightCommands[1] = { sphere.elementCount, pointLights, sphere.elementsIndex, sphere.verticesIndex, quads };
    lightCommands[2] = { cone.elementCount, spotlights, cone.elementsIndex, cone.verticesIndex, quads + pointLights };
    
    // Now return the modified range.
    m_lightDrawing.count = 3;
    return { bufferOffset, static_cast<GLsizeiptr> (sizeof (MultiDrawElementsIndirectCommand) * m_lightDrawing.count) };
}


ModifiedRange Renderer::updateDirectionalLights (const std::vector<scene::DirectionalLight>& lights) noexcept
{
    auto uniforms = m_uniforms.getWritableDirectionalLightData();
    return processLightUniforms (uniforms, lights, [] (const scene::DirectionalLight& scene, DirectionalLight& uniform)
    {
        uniform.direction = util::toGLM (scene.getDirection());
        uniform.intensity = util::toGLM (scene.getIntensity());
    });
}


Renderer::ModifiedLightVolumeRanges Renderer::updatePointLights (const std::vector<scene::PointLight>& lights, 
            const size_t transformOffset) noexcept
{
    // We need lambdas for translating scene to uniform information.
    const auto uniforms = [] (const scene::PointLight& scene, PointLight& uniform)
    {
        uniform.position    = util::toGLM (scene.getPosition());
        uniform.range       = scene.getRange();
        uniform.intensity   = util::toGLM (scene.getIntensity());
    };

    const auto transforms = [] (const scene::PointLight& scene, glm::mat4x3& transform)
    {
        const auto pos      = scene.getPosition();
        const auto range    = scene.getRange();
        transform = 
        {
            range,  0.f,    0.f,
            0.f,    range,  0.f,
            0.f,    0.f,    range,
            pos.x,  pos.y,  pos.z
        };
    };

    // Only construct transforms if we're going to be using them for deferred rendering.
    auto block = m_uniforms.getWritablePointLightData();

    if (m_deferredRender)
    {
        return processLightVolumes (block, lights, transformOffset, uniforms, transforms);
    }

    return { processLightUniforms (block, lights, uniforms), { 0, 0 } };
}


Renderer::ModifiedLightVolumeRanges Renderer::updateSpotlights (const std::vector<scene::SpotLight>& lights, 
            const size_t transformOffset) noexcept
{
    // We need lambdas for translating scene to uniform information.
    const auto uniforms = [] (const scene::SpotLight& scene, Spotlight& uniform)
    {
        uniform.position    = util::toGLM (scene.getPosition());
        uniform.coneAngle   = scene.getConeAngleDegrees();
        uniform.direction   = util::toGLM (scene.getDirection());
        uniform.range       = scene.getRange();
        uniform.intensity   = util::toGLM (scene.getIntensity());
    };

    const auto up = util::toGLM (m_scene->getUpDirection());
    const auto transforms = [=] (const scene::SpotLight& scene, glm::mat4x3& transform)
    {
        const auto pos      = util::toGLM (scene.getPosition());
        const auto dir      = util::toGLM (scene.getDirection());
        const auto angle    = scene.getConeAngleDegrees();
        const auto height   = scene.getRange();
        const auto radius   = height * std::tanf (angle / 2.f);
        const auto rotation = glm::lookAt (pos, pos + dir, up);
        const auto model    = glm::mat4x3
        {
            radius, 0.f,    0.f,
            0.f,    height, 0.f,
            0.f,    0.f,    radius,
            pos.x,  pos.y,  pos.z
        };

        transform = model * rotation;
    };

    // Only construct transforms if we're going to be using them for deferred rendering.
    auto block = m_uniforms.getWritableSpotlightData();

    if (m_deferredRender)
    {
        return processLightVolumes (block, lights, transformOffset, uniforms, transforms);
    }

    return { processLightUniforms (block, lights, uniforms), { 0, 0 } };
}