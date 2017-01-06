#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_MESH_
#define         _RENDERING_RENDERER_GEOMETRY_MESH_

// Engine headers.
#include <tgl/tgl.h>


/// <summary> 
/// A basic mesh structure used to hold the required data for rendering a mesh using OpenGL. 
/// </summary>
struct Mesh final
{
    GLintptr    verticesIndex   { 0 };  //!< The index of a VBO where the vertices for the mesh begin.
    void*       elementsOffset  { 0 };  //!< An offset in bytes used to draw the mesh in the scene.
    GLsizei     elementCount    { 0 };  //!< Indicates how many elements there are.
    
    Mesh() noexcept                         = default;
    Mesh (Mesh&&) noexcept                  = default;
    Mesh (const Mesh&) noexcept             = default;
    Mesh& operator= (const Mesh&) noexcept  = default;
    Mesh& operator= (Mesh&&) noexcept       = default;
    ~Mesh()                                 = default;
};

#endif // _RENDERING_RENDERER_GEOMETRY_MESH_