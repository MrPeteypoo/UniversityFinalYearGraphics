#pragma once

#if !defined    _RENDERING_GEOMETRY_INTERNALS_
#define         _RENDERING_GEOMETRY_INTERNALS_

// STL headers.
#include <unordered_map>


// Personal headers.
#include <Rendering/Renderer/Geometry/Geometry.hpp>


/// <summary>
/// Contains internal data that doesn't need to be accessed externally but needs to be stored nonetheless.
/// </summary>
struct Geometry::Internals final
{
    using Meshes = std::unordered_map<scene::MeshId, Mesh>;

    Buffer  m_sceneVertices { };    //!< A vertex buffer containing scene geometry.
    Buffer  m_sceneElements { };    //!< A element buffer containing elements for rendering scene geometry.
    Buffer  m_lightVertices { };    //!< A vertex buffer containing shapes such as quads, spheres and cones.
    Buffer  m_lightElements { };    //!< A element buffer containing element for rendering light shape geometry.
    Meshes  m_sceneMeshes   { };    //!< A list of mesh data for buffered scene meshes.
    
    Internals()                                         = default;
    Internals (Internals&&)                             = default;
    Internals (const Internals&) noexcept               = default;
    Internals& operator= (const Internals&) noexcept    = default;
    Internals& operator= (Internals&&) noexcept         = default;
    ~Internals()                                        = default;
};

#endif // _RENDERING_GEOMETRY_INTERNALS_