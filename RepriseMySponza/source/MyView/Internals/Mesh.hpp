#pragma once

#if !defined    _MY_VIEW_MESH_
#define         _MY_VIEW_MESH_


// Personal headers.
#include <MyView/MyView.hpp>


// Using declarations.
using GLint  = int;
using GLuint = unsigned int;


/// <summary> 
/// A basic mesh structure used to hold the required data for rendering a mesh using OpenGL. 
/// </summary>
struct Mesh final
{
    GLint   verticesIndex   { 0 };  //!< The index of a VBO where the vertices for the mesh begin.
    GLint   elementsOffset  { 0 };  //!< An offset in bytes used to draw the mesh in the scene.
    size_t  elementCount    { 0 };  //!< Indicates how many elements there are.
};

#endif // _MY_VIEW_MESH_