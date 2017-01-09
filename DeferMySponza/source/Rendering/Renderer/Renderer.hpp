#pragma once

#if !defined    _RENDERING_RENDERER_
#define         _RENDERING_RENDERER_

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

        constexpr static auto gbufferStartingTextureUnit    = GLuint { 0 }; //!< The starting texture unit for the gbuffer, the gbuffer occupies three units.
        constexpr static auto materialsStartingTextureUnit  = GLuint { 4 }; //!< The starting texture unit for the material data.
        constexpr static auto lightVolumeCount              = size_t { 3 }; //!< How many different light volumes exist.

        struct MeshInstances final
        {
            using Instances = std::vector<scene::InstanceId>;

            Mesh        mesh        { };    //!< Rendering data for a particular scene mesh.
            Instances   instances   { };    //!< A list of instances requiring the stored mesh.
        };
        
        using DrawCommands      = MultiDrawCommands<Buffer>;
        using DrawableObjects   = std::vector<MeshInstances>;

        scene::Context*     m_scene             { };    //!< Used to render the scene from the correct viewpoint.        
        Programs            m_programs          { };    //!< Stores the programs used in different rendering passes.

        DrawableObjects     m_dynamics          { };    //!< A collection of dynamic mesh instances that need drawing.
        Materials           m_materials         { };    //!< Contains every material in the scene, used for filling instancing data for dynamic objects.
        
        DrawCommands        m_objectDrawing     { };    //!< Draw commands for dynamic objects.
        SinglePMB           m_materialIDs       { };    //!< Material ID instancing data for dynamic objects.
        SinglePMB           m_objectTransforms  { };    //!< Model transforms for dynamic objects.

        DrawCommands        m_lightDrawing      { };    //!< Draw commands for light volumes.
        SinglePMB           m_lightTransforms   { };    //!< Model transforms for light volumes.

        GeometryBuffer      m_gbuffer           { };    //!< The initial framebuffer where geometry is drawn to.
        LightBuffer         m_lbuffer           { };    //!< A colour buffer where lighting is applied using data stored in the gbuffer.
        Resolution          m_resolution        { };    //!< The internal and display resolution of drawing operations.

        Uniforms            m_uniforms          { };    //!< Uniform data which is accessible to any program that requests it.
        Geometry            m_geometry          { };    //!< A collection of OpenGL objects which store the scene geometry.

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
};

#endif // _RENDERING_RENDERER_