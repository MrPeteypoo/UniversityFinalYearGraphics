#pragma once

#if !defined    _MY_VIEW_MATERIAL_
#define         _MY_VIEW_MATERIAL_


// Engine headers.
#include <glm/vec3.hpp>


/// <summary> 
/// A basic material structure which stores the diffuse and specular properties of an instance as stored in a texture buffer.
/// </summary>
struct Material final
{
    glm::vec3   diffuseColour   { 1.f };    //!< The diffuse colour of the material.
    float       textureID       { -1.f };   //!< The index of the texture in the 2D texture array. -1 indicates no texture.
    glm::vec3   specularColour  { 1.f };    //!< The specular colour of the material.
    float       shininess       { 0.f };    //!< The shininess factor of the specular colour.
};

#endif // _MY_VIEW_MATERIAL_