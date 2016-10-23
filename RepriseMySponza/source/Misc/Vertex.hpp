#pragma once

#if !defined    _VERTEX_
#define         _VERTEX_


// Engine headers.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


/// <summary> 
/// An object with a position, normal vector and texture co-ordinate to represent a single vertex.
/// </summary>
struct Vertex final
{
    glm::vec3   position        { 0 },  //!< The position vector of the vertex.
                normal          { 0 };  //!< The normal vector for the vertex.
    glm::vec2   texturePoint    { 0 };  //!< The texture co-ordinate of the vertex.
    
    
    Vertex()                                = default;
    Vertex (Vertex&& move)                  = default;
    Vertex (const Vertex& copy)             = default;
    Vertex& operator= (Vertex&& move)       = default;
    Vertex& operator= (const Vertex& copy)  = default;
    ~Vertex()                               = default;

    Vertex (const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& texture)  
        : position (pos), normal (norm), texturePoint (texture) { }
};

#endif // _VERTEX_