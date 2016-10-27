#pragma once

#if !defined    _RENDERING_UNIFORMS_POINT_LIGHT_
#define         _RENDERING_UNIFORMS_POINT_LIGHT_

// Engine headers.
#include <glm/vec4.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block containing information required to render a point light.
/// </summary>
struct PointLight final
{
    glm::vec4   position    { 0.f };    //!< The position of the light in world-space. The 'w' component is alignment padding.
    glm::vec4   intensity   { 0.f };    //!< The colour/intensity of the light. The 'w' component is alignment padding.

    float       aConstant   { 1.f };    //!< The constant co-efficient for the attenutation formula.
    float       aLinear     { 0.f };    //!< The linear co-efficient for the attenutation formula.
    float       aQuadratic  { 0.001f }; //!< The quadratic co-efficient for the attenuation formula.

    private:

        /// <summary>
        /// Ensure the block is padded to 256-byte alignment as required by OpenGL UBO bindings.
        /// </summary>
        //float unused[53];
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_POINT_LIGHT_
