#pragma once

#if !defined    _RENDERING_RENDERER_
#define         _RENDERING_RENDERER_

// STL headers.
#include <utility>


// Engine headers.
#include <glm/fwd.hpp>
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Composites/DrawCommands.hpp>
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Renderer/Drawing/GeometryBuffer.hpp>
#include <Rendering/Renderer/Drawing/LightBuffer.hpp>
#include <Rendering/Renderer/Drawing/Resolution.hpp>
#include <Rendering/Renderer/Geometry/Geometry.hpp>
#include <Rendering/Renderer/Materials/Materials.hpp>
#include <Rendering/Renderer/Programs/Programs.hpp>
#include <Rendering/Renderer/Uniforms/Uniforms.hpp>


/// <summary>
/// An OpenGL 4.5 deferred renderer which maintains geometric, material and uniform data, along with the shaders and
/// programs required to render a given scene.
/// </summary>
class Renderer final
{
    public:

        Renderer() noexcept {}
        Renderer (Renderer&&) noexcept              = default;
        Renderer& operator= (Renderer&&) noexcept   = default;
        ~Renderer()                                 = default;

        Renderer (const Renderer&)                  = delete;
        Renderer& operator= (const Renderer&)       = delete;


        /// <summary> Sets the resolution of the off-screen rendering buffers. </summary>
        void setInternalResolution (const glm::ivec2& resolution) noexcept;

        /// <summary> 
        /// Sets the resolution of the display, the internal resolution will be upscaled or downscaled to this value.
        /// </summary>
        void setDisplayResolution (const glm::ivec2& resolution) noexcept;


        /// <summary> 
        /// Attempt to initialise the renderer, building all mesh and material data, preparing the renderer for 
        /// rendering. Successive calls will completely rebuild the entire scene. Upon failure the object will
        /// clean itself to an uninitialised state.
        /// </summary>
        /// <param name="scene"> A context to use for rendering a scene. </param>
        /// <param name="internalRes"> The internal resolution for the framebuffers. </param>
        /// <param name="displayRes"> The resolution of the display to output to. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (scene::Context* scene, const glm::ivec2& internalRes, const glm::ivec2& displayRes) noexcept;

        /// <summary> Cleans all resources, putting the renderer in a state where it can be safely initialised. </summary>
        void clean() noexcept;


        /// <summary> Causes the renderer to render a frame to the display. </summary>
        void render() noexcept;

    private:

        constexpr static auto multiBuffering                = size_t { 1 }; //!< How much multi-buffering should be performed on dynamic buffers.
        constexpr static auto gbufferStartingTextureUnit    = GLuint { 0 }; //!< The starting texture unit for the gbuffer, the gbuffer occupies three units.
        constexpr static auto materialsStartingTextureUnit  = GLuint { 4 }; //!< The starting texture unit for the material data.
        constexpr static auto lightVolumeCount              = size_t { 3 }; //!< How many different light volumes exist.

        struct MeshInstances final
        {
            using Instances = std::vector<scene::InstanceId>;

            Mesh        mesh        { };    //!< Rendering data for a particular scene mesh.
            Instances   instances   { };    //!< A list of instances requiring the stored mesh.

            MeshInstances (const Mesh& mesh, Instances&& instances) noexcept
                : mesh (std::move (mesh)), instances (std::move (instances)) {}
        };

        struct ModifiedRanges final
        {
            ModifiedRange uniforms, transforms;
        };

        struct ASyncActions;
        
        using PMB               = PersistentMappedBuffer<multiBuffering>;
        using DrawCommands      = MultiDrawCommands<PMB>;
        using SyncObjects       = std::array<GLsync, multiBuffering>;
        using DrawableObjects   = std::vector<MeshInstances>;
                
        scene::Context*     m_scene             { };        //!< Used to render the scene from the correct viewpoint.
        Uniforms            m_uniforms          { };        //!< Uniform data which is accessible to any program that requests it.
        Programs            m_programs          { };        //!< Stores the programs used in different rendering passes.

        DrawableObjects     m_dynamics          { };        //!< A collection of dynamic mesh instances that need drawing.
        Materials           m_materials         { };        //!< Contains every material in the scene, used for filling instancing data for dynamic objects.
        
        DrawCommands        m_objectDrawing     { };        //!< Draw commands for dynamic objects.
        PMB                 m_objectMaterialIDs { };        //!< Material ID instancing data for dynamic objects.
        PMB                 m_objectTransforms  { };        //!< Model transforms for dynamic objects.

        DrawCommands        m_lightDrawing      { };        //!< Draw commands for light volumes.
        PMB                 m_lightTransforms   { };        //!< Model transforms for light volumes.

        GeometryBuffer      m_gbuffer           { };        //!< The initial framebuffer where geometry is drawn to.
        LightBuffer         m_lbuffer           { };        //!< A colour buffer where lighting is applied using data stored in the gbuffer.
        Resolution          m_resolution        { };        //!< The internal and display resolution of drawing operations.

        Geometry            m_geometry          { };        //!< A collection of OpenGL objects which store the scene geometry.
        
        size_t              m_partition         { 0 };      //!< The buffer partition to use when rendering the current frame.
        SyncObjects         m_syncs             { };        //!< Contains sync objects for each level of buffering, allows us to manually synchronise with the GPU if needed.
        bool                m_deferredRender    { true };   //!< Whether a deferred or forward render should be performed.
        bool                m_multiThreaded     { true };   //!< Whether the renderer should be multi-threaded or not.

    private:

        /// <summary>
        /// Attempts to build the OpenGL programs. 
        /// </summary>
        bool buildPrograms() noexcept;

        /// <summary>
        /// Attempts to load the texture and material data of every object in the scene.
        /// </summary>
        bool buildMaterials() noexcept;

        /// <summary>
        /// Attempts to build the dynamic object command and instancing buffers. This parses the instances container
        /// in the given scene to determine how many dynamic instances exist and allocates enough memory to accomodate
        /// that.
        /// </summary> 
        bool buildDynamicObjectBuffers() noexcept;

        /// <summary>
        /// Attempts to build the light command and transform buffers. This counts the total number of rendering passes
        /// that will occur and allocates enough memory to perform each pass.
        /// </summary> 
        bool buildLightBuffers() noexcept;

        /// <summary>
        /// Attempts to build the geometry data for every mesh in the scene.
        /// </summary> 
        bool buildGeometry() noexcept;

        /// <summary> 
        /// Attempt to build the geometry and light buffers according to the current internal resolution. 
        /// </summary>
        bool buildFramebuffers() noexcept;

        /// <summary>
        /// Attempts to build the uniform data. This will attempt to bind each block to every loaded program, material
        /// texture units will be registered, frambuffer texture units will be registered and light counts will be
        /// specified.
        /// </summary> 
        bool buildUniforms() noexcept;

        /// <summary>
        /// Fills the drawable instances container with non-static instances found in the given scene
        /// </summary> 
        void fillDynamicInstances() noexcept;

        /// <summary> Checks the sync object of the current partition and waits if it hasn't already fired. </summary>
        void syncWithGPUIfNecessary() const noexcept;

        /// <summary> Performs a forward render of the entire scene. </summary>
        void forwardRender (SceneVAO& sceneVAO, const ASyncActions& actions) noexcept;

        /// <summary> Performs a deferred render of the entire scene. </summary>
        void deferredRender (SceneVAO& sceneVAO, const ASyncActions& actions, const size_t directionalLights,
            const size_t pointLights, const size_t spotlights) noexcept;

        /// <summary> Updates the scene uniforms such as the camera position, ambient lighting and matrices. </summary>
        ModifiedRange updateSceneUniforms() noexcept;

        /// <summary> Updates the dynamic object draw command buffer so every instance can be drawn. </summary>
        ModifiedRange updateDynamicObjectDrawCommands() noexcept;

        /// <summary> Updates the dynamic object transform buffer with the transform of every dynamic object. </summary>
        ModifiedRange updateDynamicObjectTransforms() noexcept;

        /// <summary> Updates the material IDs buffer with material IDs of every dynamic object. </summary>
        ModifiedRange updateDynamicObjectMaterialIDs() noexcept;

        /// <summary> Adds a draw command for a full-screen quad and every point and spotlight in the scene. </summary>
        ModifiedRange updateLightDrawCommands (const size_t pointLights, const size_t spotlights) noexcept;

        /// <summary> Updates the directional light uniform data with the given light data. </summary>
        ModifiedRange updateDirectionalLights (const std::vector<scene::DirectionalLight>& lights) noexcept;

        /// <summary> Updates the transform and uniform data for every given point light. </summary>
        ModifiedRanges updatePointLights (const std::vector<scene::PointLight>& lights, 
            const size_t transformOffset) noexcept;

        /// <summary> Updates the transform and uniform data for every given spot light. </summary>
        ModifiedRanges updateSpotlights (const std::vector<scene::SpotLight>& lights, 
            const size_t transformOffset) noexcept;
};

#endif // _RENDERING_RENDERER_