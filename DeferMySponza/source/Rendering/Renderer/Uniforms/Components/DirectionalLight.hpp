#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_
#define         _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_

// Engine headers.
#include <glm/vec3.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A directional light as it appears in a uniform block.
/// </summary>
struct DirectionalLight
{
    public:     glm::vec3 direction { 0.f };    //!< The direction of the light in world-space.
    private:    float padding1;
    public:     glm::vec3 intensity { 0.f };    //!< The colour/intensity of the light.
    private:    float padding2;
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_DIRECTIONAL_LIGHT_
