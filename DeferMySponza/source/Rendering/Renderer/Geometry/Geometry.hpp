#pragma once

#if !defined    _RENDERING_GEOMETRY_
#define         _RENDERING_GEOMETRY_

// STL headers.
#include <memory>


// Engine headers.
#include <scene/scene_fwd.hpp>


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>
#include <Rendering/Objects/VertexArray.hpp>
#include <Rendering/Renderer/Geometry/Mesh.hpp>


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


        /// <summary> 
        /// Constructs geometry from scene::GeometryBuilder as well as constructing the required light sources, and
        /// preparing static objects for static batching.
        /// </summary>
        /// <param name="scene"> A context to use for determining static objects. </param>
        /// <returns> Whether initialisation was successful or not. </returns>
        bool initialise (scene::Context* scene) noexcept;

        /// <summary> Destroys every stored object and returns to a clean state. </summary>
        void clean() noexcept;

    private:

        struct Internals;
        using Pimpl = std::unique_ptr<Internals>;

        VertexArray m_scene         { };    //!< Used for drawing all scene geometry.
        Buffer      m_transforms    { };    //!< Stores a PVM and model transform for every rendered instance.
        Buffer      m_materialIDs   { };    //!< Stores the material IDs of every rendered instance.
        
        VertexArray m_light         { };    //!< Used for applying light using quads, spheres and cones.
        Mesh        m_quad          { };    //!< The mesh data required for drawing a full-screen quad.
        Mesh        m_sphere        { };    //!< The mesh data required for drawing a sphere.

        Pimpl       m_pimpl         { };    //!< Stores less important internal data.

};

#endif // _RENDERING_GEOMETRY_