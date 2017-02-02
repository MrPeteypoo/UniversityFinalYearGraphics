#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_
#define         _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_

// Engine headers.
#include <glm/vec3.hpp>
#include <tgl/tgl.h>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A spotlight as it appears in a uniform block.
/// </summary>
struct Spotlight
{
    glm::vec3   position        { 0.f };    //!< The position of the light in world-space.
    GLfloat     coneAngle       { 90.f };   //!< The angle of the cone.
    
    glm::vec3   direction       { 0.f };    //!< The direction of the light in world-space.
    GLfloat     range           { 10.f };   //!< The range of the spotlight.

    glm::vec3   intensity       { 0.f };    //!< The colour/intensity of the light.
    GLfloat     concentration   { 2.f };    //!< How concentrated the beam of a spot light is.

    GLfloat     aConstant       { 1.f };    //!< The constant co-efficient for the attenutation formula.
    GLfloat     aLinear         { 0.f };    //!< The linear co-efficient for the attenutation formula.
    GLfloat     aQuadratic      { 0.001f }; //!< The quadratic co-efficient for the attenuation formula.
    GLint       viewIndex       { -1 };     //!< The index of the view transform of the light.
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_
