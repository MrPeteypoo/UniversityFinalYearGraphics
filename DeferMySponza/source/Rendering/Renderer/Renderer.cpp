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
#include <Rendering/Binders/ProgramBinder.hpp>
#include <Rendering/Binders/TextureBinder.hpp>
#include <Rendering/Binders/VertexArrayBinder.hpp>
#include <Rendering/Renderer/Drawing/PassConfigurator.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>
#include <Rendering/Renderer/Uniforms/Blocks/Scene.hpp>
#include <Rendering/Renderer/Uniforms/Blocks/FullBlock.hpp>
#include <Rendering/Renderer/Uniforms/Components/DirectionalLight.hpp>
#include <Rendering/Renderer/Uniforms/Components/PointLight.hpp>
#include <Rendering/Renderer/Uniforms/Components/Spotlight.hpp>
#include <Utility/Algorithm.hpp>
#include <Utility/Scene.hpp>


// Namespaces.
using namespace std::chrono_literals;
using namespace types;


struct Renderer::ASyncActions final
{
    using Action                = std::future<ModifiedRange>;
    using DynamicObjectAction   = std::future<ModifiedDynamicObjectRanges>;
    using LightVolumeAction     = std::future<ModifiedLightVolumeRanges>;

    Action              sceneUniforms, lightDrawCommands, directionalLights;
    DynamicObjectAction dynamicObjects;
    LightVolumeAction   pointLights, spotLights;
    
    ASyncActions() noexcept                                 = default;
    ASyncActions (ASyncActions&&) noexcept                  = default;
    ASyncActions& operator= (ASyncActions&&) noexcept       = default;
    ASyncActions (const ASyncActions&) noexcept             = delete;
    ASyncActions& operator= (const ASyncActions&) noexcept  = delete;
    ~ASyncActions()
    {
        const auto waitIfValid = [] (const auto& action)
        {
            if (action.valid())
            {
                action.wait();
            }
        };
        
        waitIfValid (sceneUniforms);
        waitIfValid (lightDrawCommands);
        waitIfValid (directionalLights);
        waitIfValid (dynamicObjects);
        waitIfValid (pointLights);
        waitIfValid (spotLights);
    }
};


void Renderer::setShadingMode (bool usePhysicallyBasedShading) noexcept
{
    // Do nothing if we're already in the correct mode.
    if (usePhysicallyBasedShading != m_pbs)
    {
        // Set the new value of the flag.
        m_pbs = usePhysicallyBasedShading;
        
        // Rebuild the programs and rebind the uniforms.
        buildPrograms();
        m_uniforms.bindUniformsToPrograms (m_programs);
    }
}


void Renderer::setInternalResolution (const glm::ivec2& resolution) noexcept
{
    // Only change the resolution if it's different from the current value.
    if (m_resolution.internalWidth != resolution.x || m_resolution.internalHeight != resolution.y)
    {
        m_resolution.internalWidth  = resolution.x;
        m_resolution.internalHeight = resolution.y;

        // We'll need to rebuild the framebuffers due to a resolution changes.
        buildFramebuffers();
        buildUniforms();
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
    m_scene                     = nullptr;
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
    
    if (!shaders.initialise (m_pbs))
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
    const auto materialIDSize   = static_cast<GLsizeiptr> (instanceCount * sizeof (MaterialID));
    const auto transformSize    = static_cast<GLsizeiptr> (instanceCount * sizeof (ModelTransform));

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
    
    // Finally determine the size of the buffers.
    constexpr auto lightVolumeCount = size_t { 2 }; 
    const auto count                = point.size() + spot.size();
    const auto transformSize        = static_cast<GLsizeiptr> (count * sizeof (ModelTransform));
    const auto drawCommandSize      = static_cast<GLsizeiptr> (lightVolumeCount * sizeof (MultiDrawElementsIndirectCommand));
    
    // Now we can initialise the buffers
    if (!(m_lightDrawing.buffer.initialise (drawCommandSize, false, false) && 
        m_lightTransforms.initialise (transformSize, false, false)))
    {
        return false;
    }

    // Finally set up the draw buffer.
    m_lightDrawing.capacity = static_cast<GLsizei> (lightVolumeCount);
    m_lightDrawing.count    = 1;
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
            m_lbuffer.initialise (m_gbuffer.getDepthStencilTexture(), GL_RGBA8, width, height);
}


bool Renderer::buildUniforms() noexcept
{
    // Make sure the uniforms build correctly.
    if (!m_uniforms.initialise (m_gbuffer, m_materials))
    {
        return false;
    }

    // Now we can bind the uniform blocks to each program and we're done!
    m_uniforms.bindUniformsToPrograms (m_programs);
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

    // We can safely multithread the data streaming operations.
    auto actions        = ASyncActions { };
    const auto policy   = m_multiThreaded ? std::launch::async : std::launch::deferred;

    // Now execute the asynchonous tasks.
    actions.sceneUniforms       = std::async (policy, [&]() { return updateSceneUniforms(); });
    actions.dynamicObjects      = std::async (policy, [&]() { return updateDynamicObjects(); });
    actions.directionalLights   = std::async (policy, [&]() { return updateDirectionalLights (directional); });
    actions.pointLights         = std::async (policy, [&]() { return updatePointLights (point); });
    actions.spotLights          = std::async (policy, [&]() { return updateSpotlights (spot, point.size()); });

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
        deferredRender (sceneVAO, actions);
    }

    else
    {
        forwardRender (sceneVAO, actions);
    }

    // Render to the screen.
    glBlitNamedFramebuffer (m_lbuffer.getFramebuffer().getID(), 0,
        0, 0, m_resolution.internalWidth, m_resolution.internalHeight,
        0, 0, m_resolution.displayWidth, m_resolution.displayHeight, 
        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Cleanup.
    m_materials.unbindTextures();

    // Prepare for the next frame, the fence sync only allows the given parameters.
    if (!m_syncs[m_partition].initialise())
    {
        assert (false);
    }

    ++m_partition %= multiBuffering;
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
            const auto result = sync.waitForSignal (true, oneSecond);
            assert (result);
        }
    }
}


void Renderer::deferredRender (SceneVAO& sceneVAO, ASyncActions& actions) noexcept
{
    // We need to perform a geometry pass to collect the position, normal and material data of every object that's 
    // visible on-screen.
    const auto activeProgram        = ProgramBinder { m_programs.geometryPass };
    const auto activeFramebuffer    = FramebufferBinder<GL_DRAW_FRAMEBUFFER> { m_gbuffer.getFramebuffer() };

    // Prepare the fresh frame.
    PassConfigurator::geometryPass();

    // We only need to update the scene uniforms at this stage.
    m_uniforms.notifyModifiedDataRange (actions.sceneUniforms.get());

    // Now we can render static objects.
    const auto& staticObjects       = m_geometry.getStaticGeometryCommands();
    const auto activeIndirectBuffer = BufferBinder<GL_DRAW_INDIRECT_BUFFER> { staticObjects.buffer };

    staticObjects.drawWithoutBinding();

    // We must prepare for drawing dynamic objects.
    sceneVAO.useDynamicBuffers<multiBuffering> (m_partition);

    const auto objectRanges = actions.dynamicObjects.get();
    m_objectDrawing.buffer.notifyModifiedDataRange (objectRanges.drawCommands);
    m_objectMaterialIDs.notifyModifiedDataRange (objectRanges.materialIDs);
    m_objectTransforms.notifyModifiedDataRange (objectRanges.transforms);

    // Now we can draw the rest of the geometry!
    activeIndirectBuffer.bind (m_objectDrawing.buffer.getID());
    m_objectDrawing.drawWithoutBinding();

    // The geometry pass has completed. We need to prepare for a global lighting pass, this will require using an 
    // oversized triangle to perform a full-screen lighting pass.
    activeProgram.bind (m_programs.globalLightPass);
    activeFramebuffer.bind (m_lbuffer.getFramebuffer());
    VertexArrayBinder::bind (m_geometry.getTriangleVAO().vao);

    // Prepare OpenGL, the light buffer and the program for a global light pass program.
    PassConfigurator::globalLightPass();
    Programs::setActiveProgramSubroutine (GL_FRAGMENT_SHADER, Programs::globalLightSubroutine);

    // Don't forget to bind the gbuffer textures.
    const auto gbufferPosition  = TextureBinder (m_gbuffer.getPositionTexture());
    const auto gbufferNormals   = TextureBinder (m_gbuffer.getNormalTexture());
    const auto gbufferMaterials = TextureBinder (m_gbuffer.getMaterialTexture());

    // Now all we need is for the directional light data thread to complete.
    m_uniforms.notifyModifiedDataRange (actions.directionalLights.get());

    // Finally draw a full-screen triangle and global lighting will be applied.
    glDrawArrays (GL_TRIANGLES, 0, FullScreenTriangleVAO::vertexCount);

    // Move on to point llights. This will require binding a different program, VAO and indirect buffer.
    activeProgram.bind (m_programs.lightingPass);
    activeIndirectBuffer.bind (m_lightDrawing.buffer.getID());

    auto& lightingVAO = m_geometry.getLightingVAO();
    VertexArrayBinder::bind (lightingVAO.vao);
    lightingVAO.useTransformPartition (m_partition);

    // Configure OpenGL and the new program for usage.
    PassConfigurator::lightVolumePass();
    Programs::setActiveProgramSubroutine (GL_FRAGMENT_SHADER, Programs::pointLightSubroutine);

    // Update the draw commands, uniforms and transforms.
    m_lightDrawing.buffer.notifyModifiedDataRange (actions.lightDrawCommands.get());

    const auto pointLightData = actions.pointLights.get();
    m_uniforms.notifyModifiedDataRange (pointLightData.uniforms);
    m_lightTransforms.notifyModifiedDataRange (pointLightData.transforms);

    // Now draw the point lights.
    m_lightDrawing.drawWithoutBinding();

    // And finally spotlights.
    Programs::setActiveProgramSubroutine (GL_FRAGMENT_SHADER, Programs::spotlightSubrotuing); 

    const auto spotlightData = actions.spotLights.get();
    m_uniforms.notifyModifiedDataRange (spotlightData.uniforms);
    m_lightTransforms.notifyModifiedDataRange (spotlightData.transforms);

    // Draw the spotlights.
    m_lightDrawing.incrementOffset();
    m_lightDrawing.drawWithoutBinding();
}


void Renderer::forwardRender (SceneVAO& sceneVAO, ASyncActions& actions) noexcept
{
    // We need to use the purpose-made forward render program.
    const auto activeProgram = ProgramBinder { m_programs.forwardRender };

    // Ensure we bind the off-screen light buffer.
    const auto activeFramebuffer = FramebufferBinder<GL_DRAW_FRAMEBUFFER> { m_lbuffer.getFramebuffer() };

    // Prepare the fresh frame.
    PassConfigurator::forwardRender();

    // Unfortunately forward rendering doesn't benefit from multi-threading too much so we have to synchronise early.
    m_uniforms.notifyModifiedDataRange (actions.sceneUniforms.get());
    m_uniforms.notifyModifiedDataRange (actions.directionalLights.get());
    m_uniforms.notifyModifiedDataRange (actions.pointLights.get().uniforms);
    m_uniforms.notifyModifiedDataRange (actions.spotLights.get().uniforms);
    
    // Now we can render static objects.
    const auto& staticObjects       = m_geometry.getStaticGeometryCommands();
    const auto activeIndirectBuffer = BufferBinder<GL_DRAW_INDIRECT_BUFFER> { staticObjects.buffer };

    staticObjects.drawWithoutBinding();

    // Prepare for dynamic objects.
    sceneVAO.useDynamicBuffers<multiBuffering> (m_partition);

    const auto objectRanges = actions.dynamicObjects.get();
    m_objectDrawing.buffer.notifyModifiedDataRange (objectRanges.drawCommands);
    m_objectMaterialIDs.notifyModifiedDataRange (objectRanges.materialIDs);
    m_objectTransforms.notifyModifiedDataRange (objectRanges.transforms);

    // Now we can draw!
    activeIndirectBuffer.bind (m_objectDrawing.buffer.getID());
    m_objectDrawing.drawWithoutBinding();
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

    scene.data->view        = glm::lookAt (camPosition, camPosition + camDirection, upDirection);
    scene.data->camera      = camPosition;
    scene.data->ambience    = util::toGLM (m_scene->getAmbientLightIntensity());

    return { scene.offset, sizeof (Scene) };
}


Renderer::ModifiedDynamicObjectRanges Renderer::updateDynamicObjects() noexcept
{
    // Retrieve the necessary pointers. We also need to keep track of how many instances there are.
    auto drawCommandBuffer  = (MultiDrawElementsIndirectCommand*) m_objectDrawing.buffer.pointer (m_partition);
    auto transformBuffer    = (ModelTransform*) m_objectTransforms.pointer (m_partition);
    auto materialIDBuffer   = (MaterialID*) m_objectMaterialIDs.pointer (m_partition);
    auto instanceCount      = GLuint { 0 };

    // Create lambda functions to update the data.
    const auto addDrawCommand = [&] (const auto index, const Mesh& mesh, const MeshInstances::Instances& instances)
    {
        const auto count = static_cast<GLuint> (instances.size());
        drawCommandBuffer[index] = { mesh.elementCount, count, mesh.elementsIndex, mesh.verticesIndex, instanceCount };

        // Ensure we increment the base instance.
        instanceCount += count;
    };

    const auto addTransform = [&] (const auto index, const scene::Instance& instance)
    {
        transformBuffer[index] = ModelTransform (util::toGLM (instance.getTransformationMatrix()));
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
    const auto drawingOffset    = m_objectDrawing.buffer.partitionOffset (m_partition);
    m_objectDrawing.start       = drawingOffset;
    m_objectDrawing.count       = static_cast<GLsizei> (m_dynamics.size());
    return 
    { 
        { drawingOffset,                                        static_cast<GLsizeiptr> (sizeof (MultiDrawElementsIndirectCommand) * m_objectDrawing.count) },
        { m_objectTransforms.partitionOffset (m_partition),     static_cast<GLsizeiptr> (sizeof (ModelTransform) * instanceCount) },
        { m_objectMaterialIDs.partitionOffset (m_partition),    static_cast<GLsizeiptr> (sizeof (MaterialID) * instanceCount) }
    };
}


ModifiedRange Renderer::updateLightDrawCommands (const GLuint pointLights, const GLuint spotlights) noexcept
{
    // We need the pointer to write to the buffer.
    const auto bufferOffset = m_lightDrawing.buffer.partitionOffset (m_partition);
    auto lightCommands      = (MultiDrawElementsIndirectCommand*) m_lightDrawing.buffer.pointer (m_partition);

    // Cache the shape meshes.
    const auto& sphere  = m_geometry.getSphere();
    const auto& cone    = m_geometry.getCone();

    // Finally add each draw command for the light volumes.
    lightCommands[0] = { sphere.elementCount, pointLights, sphere.elementsIndex, sphere.verticesIndex, 0 };
    lightCommands[1] = { cone.elementCount, spotlights, cone.elementsIndex, cone.verticesIndex, pointLights };

    // Now return the modified range.
    const auto modifiedCommands = 2;
    m_lightDrawing.start = bufferOffset;
    return { bufferOffset, static_cast<GLsizeiptr> (sizeof (MultiDrawElementsIndirectCommand) * modifiedCommands) };
}


ModifiedRange Renderer::updateDirectionalLights (const std::vector<scene::DirectionalLight>& lights) noexcept
{
    auto uniforms = m_uniforms.getWritableDirectionalLightData();
    return processLightUniforms (uniforms, lights, [] (const scene::DirectionalLight& scene)
    {
        auto light      = DirectionalLight {};
        light.direction = util::toGLM (scene.getDirection());
        light.intensity = util::toGLM (scene.getIntensity());

        return light;
    });
}


Renderer::ModifiedLightVolumeRanges Renderer::updatePointLights (const std::vector<scene::PointLight>& lights) noexcept
{
    // We need lambdas for translating scene to uniform information.
    const auto uniforms = [] (const scene::PointLight& scene)
    {
        auto light      = PointLight { };
        light.position  = util::toGLM (scene.getPosition());
        light.range     = scene.getRange();
        light.intensity = util::toGLM (scene.getIntensity());

        return light;
    };

    const auto transforms = [] (const scene::PointLight& scene)
    {
        const auto pos      = scene.getPosition();
        const auto range    = scene.getRange();
        return ModelTransform
        {
            range,  0.f,    0.f,
            0.f,    range,  0.f,
            0.f,    0.f,    range,
            pos.x,  pos.y,  pos.z,
        };
    };

    // Only construct transforms if we're going to be using them for deferred rendering.
    auto block = m_uniforms.getWritablePointLightData();

    if (m_deferredRender)
    {
        return processLightVolumes (block, lights, 0, uniforms, transforms);
    }

    return { processLightUniforms (block, lights, uniforms), { 0, 0 } };
}


Renderer::ModifiedLightVolumeRanges Renderer::updateSpotlights (const std::vector<scene::SpotLight>& lights, 
            const size_t transformOffset) noexcept
{
    // We need lambdas for translating scene to uniform information.
    const auto uniforms = [] (const scene::SpotLight& scene)
    {
        auto light      = Spotlight { };
        light.position  = util::toGLM (scene.getPosition());
        light.coneAngle = scene.getConeAngleDegrees();
        light.direction = util::toGLM (scene.getDirection());
        light.range     = scene.getRange();
        light.intensity = util::toGLM (scene.getIntensity());

        return light;
    };

    const auto up = util::toGLM (m_scene->getUpDirection());
    const auto transforms = [=] (const scene::SpotLight& scene)
    {
        const auto pos      = util::toGLM (scene.getPosition());
        const auto dir      = util::toGLM (scene.getDirection());
        const auto angle    = glm::radians (scene.getConeAngleDegrees());
        const auto height   = scene.getRange();
        const auto radius   = height * std::tanf (angle / 2.f);
        const auto rotation = glm::inverse (glm::lookAt (pos, pos + dir, up));

        return ModelTransform
        {
            glm::scale (rotation, glm::vec3 (radius, radius, height))
        };
    };

    // Only construct transforms if we're going to be using them for deferred rendering.
    auto block = m_uniforms.getWritableSpotlightData();

    if (m_deferredRender)
    {
        return processLightVolumes (block, lights, transformOffset, uniforms, transforms);
    }

    return { processLightUniforms (block, lights, uniforms), { 0, 0 } };
}