#pragma once

#if !defined    _MY_VIEW_MESH_
#define         _MY_VIEW_MESH_


// Personal headers.
#include <MyView/MyView.hpp>


// Using declarations.
using GLintptr  = ptrdiff_t;
using GLsizei   = int;


/// <summary> 
/// A basic mesh structure used to hold the required data for rendering a mesh using OpenGL. 
/// </summary>
struct Mesh final
{
    GLintptr    verticesIndex   { 0 };  //!< The index of a VBO where the vertices for the mesh begin.
    GLintptr    elementsOffset  { 0 };  //!< An offset in bytes used to draw the mesh in the scene.
    GLsizei     elementCount    { 0 };  //!< Indicates how many elements there are.
};

#endif // _MY_VIEW_MESH_