#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_
#define         _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_

// Engine headers.
#include <glm/vec3.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A spotlight as it appears in a uniform block.
/// </summary>
struct Spotlight
{
    glm::vec3   position        { 0.f };    //!< The position of the light in world-space.
    float       coneAngle       { 90.f };   //!< The angle of the cone.
    
    glm::vec3   direction       { 0.f };    //!< The direction of the light in world-space.
    float       range           { 10.f };   //!< The range of the spotlight.

    glm::vec3   intensity       { 0.f };    //!< The colour/intensity of the light.
    float       concentration   { 2.f };    //!< How concentrated the beam of a spot light is.

    float       aConstant       { 0.75f };  //!< The constant co-efficient for the attenutation formula.
    float       aLinear         { 0.f };    //!< The linear co-efficient for the attenutation formula.
    float       aQuadratic      { 0.001f }; //!< The quadratic co-efficient for the attenuation formula.

};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_SPOTLIGHT_
