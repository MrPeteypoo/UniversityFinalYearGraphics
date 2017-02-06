#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_
#define         _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_

// Engine headers.
#include <glm/vec3.hpp>


// Personal headers.
#include <Rendering/Renderer/Uniforms/Components/AlignedItem.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A directional light as it appears in a uniform block.
/// </summary>
struct DirectionalLight
{
    AlignedItem<glm::vec3>  direction;  //!< The direction of the light in world-space.
    AlignedItem<glm::vec3>  intensity;  //!< The colour/intensity of the light.
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_
