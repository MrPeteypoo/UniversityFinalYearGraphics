#pragma once

#if !defined    _RENDERING_RENDERER_
#define         _RENDERING_RENDERER_

// Engine headers.
#include <glm/fwd.hpp>
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Renderer/Drawing/GeometryBuffer.hpp>
#include <Rendering/Renderer/Drawing/LightBuffer.hpp>
#include <Rendering/Renderer/Drawing/Resolution.hpp>
#include <Rendering/Renderer/Geometry/Geometry.hpp>
#include <Rendering/Renderer/Programs/Programs.hpp>
#include <Rendering/Renderer/Uniforms/Uniforms.hpp>


/// <summary>
/// An OpenGL 4.5 deferred renderer which maintains geometric, material and uniform data, along with the shaders and
/// programs required to render a given scene.
/// </summary>
class Renderer final
{
    public:

        Renderer() noexcept                         = default;
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
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (scene::Context* scene, const glm::ivec2& internalRes, const glm::ivec2& displayRes) noexcept;

        /// <summary> 
        /// Causes all shaders and programs to be completely rebuilt. Upon failure the old shaders will be lost.
        /// </summary>
        /// <returns> Whether the shaders were successfully built. </returns>
        bool rebuildShaders() noexcept;

        /// <summary> Cleans all resources, putting the renderer in a state where it can be safely initialised. </summary>
        void clean() noexcept;


        /// <summary> Causes the renderer to render a frame to the display. </summary>
        void render() noexcept;

    private:
        
        struct MeshInstances final
        {
            using Instances = std::vector<scene::Instance>;

            Mesh        mesh        { };    //!< Rendering data for a particular scene mesh.
            Instances   instances   { };    //!< A list of instances requiring the stored mesh.
        };

        using DynamicObjects = std::vector<MeshInstances>;

        DynamicObjects  m_dynamics      { };    //!< A collection of dynamic mesh instances that need drawing.
        Geometry        m_geometry      { };    //!< A collection of OpenGL objects which store the scene geometry.
        Programs        m_programs      { };    //!< Stores the programs used in different rendering passes.
        GeometryBuffer  m_gbuffer       { };    //!< The initial framebuffer where geometry is drawn to.
        LightBuffer     m_lbuffer       { };    //!< A colour buffer where lighting is applied using data stored in the gbuffer.
        Resolution      m_resolution    { };    //!< The internal and display resolution of drawing operations.
        Uniforms        m_uniforms      { };    //!< Uniform data which is accessible to any program that requests it.
};

#endif // _RENDERING_RENDERER_