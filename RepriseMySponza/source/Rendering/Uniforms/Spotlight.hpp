#pragma once

#if !defined    _RENDERING_UNIFORMS_SPOTLIGHT_
#define         _RENDERING_UNIFORMS_SPOTLIGHT_

// Engine headers.
#include <glm/vec4.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block containing information required to render a spotlight.
/// </summary>
struct Spotlight final
{
    glm::vec4   position        { 0.f };    //!< The position of the light in world-space. The 'w' component is alignment padding.
    
    glm::vec3   direction       { 0.f };    //!< The direction of the light in world-space.
    float       coneAngle       { 90.f };    //!< The angle of the cone.

    glm::vec3   intensity       { 0.f };    //!< The colour/intensity of the light.
    float       concentration   { 2.f };    //!< How concentrated the beam of a spot light is.

    float       aConstant       { 1.f };    //!< The constant co-efficient for the attenutation formula.
    float       aLinear         { 0.f };    //!< The linear co-efficient for the attenutation formula.
    float       aQuadratic      { 0.001f }; //!< The quadratic co-efficient for the attenuation formula.

    private:
    
        /// <summary>
        /// Ensure the block is padded to 256-byte alignment as required by OpenGL UBO bindings.
        /// </summary>
        float unused[49];
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_SPOTLIGHT_
