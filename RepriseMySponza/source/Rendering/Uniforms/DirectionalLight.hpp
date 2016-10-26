#pragma once

#if !defined    _RENDERING_UNIFORMS_DIRECTIONAL_LIGHT_
#define         _RENDERING_UNIFORMS_DIRECTIONAL_LIGHT_

// Engine headers.
#include <glm/vec4.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block containing information required to render a directional light.
/// </summary>
struct DirectionalLight final
{
    glm::vec4 direction { 0.f };    //!< The direction of the light in world-space. The 'w' component is alignment padding.
    glm::vec4 intensity { 0.f };    //!< The colour/intensity of the light. The 'w' component is alignment padding.

    private:

        /// <summary>
        /// Ensure the block is padded to 256-byte alignment as required by OpenGL UBO bindings.
        /// </summary>
        float unused[56];
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_DIRECTIONAL_LIGHT_
