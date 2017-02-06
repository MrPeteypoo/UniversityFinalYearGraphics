#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_POINT_LIGHT_
#define         _RENDERING_UNIFORMS_COMPONENTS_POINT_LIGHT_

// Engine headers.
#include <glm/vec3.hpp>
#include <tgl/tgl.h>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary> 
/// A point light as it appears in a uniform block.
/// </summary>
struct PointLight
{
    glm::vec3   position    { 0.f };    //!< The position of the light in world-space.
    GLfloat     range       { 0.f };    //!< The range of the point light.

    glm::vec3   intensity   { 0.f };    //!< The colour/intensity of the light.
    GLfloat     aConstant   { 1.f };    //!< The constant co-efficient for the attenutation formula.

    GLfloat     aLinear     { 0.22f };  //!< The linear co-efficient for the attenutation formula.
    GLfloat     aQuadratic  { 0.20f };  //!< The quadratic co-efficient for the attenuation formula.
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_POINT_LIGHT_
