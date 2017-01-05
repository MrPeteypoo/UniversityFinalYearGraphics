#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_
#define         _RENDERING_RENDERER_GEOMETRY_

// STL headers.
#include <memory>
#include <unordered_map>


// Engine headers.
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Renderer/Geometry/Mesh.hpp>
#include <Rendering/Renderer/Geometry/SceneVAO.hpp>


/// <summary>
/// Contains every piece of geometry in the scene. Static batching is supported with static instances having their
/// transforms permanently stored in the transforms buffer.
/// </summary>
class Geometry final
{
    public:

        Geometry() noexcept                         = default;
        Geometry (Geometry&&) noexcept              = default;
        Geometry& operator= (Geometry&&) noexcept   = default;
        ~Geometry()                                 = default;

        Geometry (const Geometry&)                  = delete;
        Geometry& operator= (const Geometry&)       = delete;

        
        inline const SceneVAO& getSceneVAO() const noexcept      { return m_scene; }

        inline SceneVAO& getSceneVAO() noexcept                  { return m_scene; }

        inline const VertexArray& getLightingVAO() const noexcept   { return m_lighting; }

        inline VertexArray& getLightingVAO() noexcept               { return m_lighting; }




        /// <summary> 
        /// Constructs geometry from scene::GeometryBuilde
        /// </summary>
        /// <param name="scene"> A context to use for determining the number of spotlight meshes to build. </param>
        /// <param name="staticInstanceCount"> The total number of static instances in the scene. </param>
        /// <param name="staticInstances"> Contains every static instance which will be loaded into memory. </param> 
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (const size_t staticInstanceCount, 
            const std::unordered_map<scene::MeshId, scene::Instance>& staticInstances) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;

    private:

        struct Internals;
        using Pimpl = std::unique_ptr<Internals>;

        SceneVAO    m_scene         { };    //!< Used for drawing all scene geometry.
        Buffer      m_transforms    { };    //!< Stores model transforms for all static objects in the scene.
        Buffer      m_materialIDs   { };    //!< Contains material IDs for all static objects in the scene.
        
        VertexArray m_lighting      { };    //!< Used for applying light using quads, spheres and cones.
        Mesh        m_quad          { };    //!< The mesh data required for drawing a full-screen quad.
        Mesh        m_pointLight    { };    //!< The mesh data required for drawing a sphere.
        Mesh        m_spotlight     { };    //!< The mesh data required for drawing a cone.

        Pimpl       m_pimpl         { };    //!< Stores less important internal data.

};

#endif // _RENDERING_RENDERER_GEOMETRY_