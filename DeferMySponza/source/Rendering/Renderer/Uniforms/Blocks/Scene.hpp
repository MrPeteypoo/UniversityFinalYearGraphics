#pragma once

#if !defined    _RENDERING_UNIFORMS_SCENE_
#define         _RENDERING_UNIFORMS_SCENE_

// Engine headers.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


// Personal headers.
#include <Rendering/Renderer/Uniforms/Components/AlignedItem.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block representing information about the scene being rendered.
/// </summary>
struct Scene final
{
    glm::mat4               projection; //!< The projection matrix used during the rendering of the entire frame.
    glm::mat4               view;       //!< The view matrix from the current cameras position and direction.

    AlignedItem<glm::vec3>  camera;     //!< The world-space position of the camera.
    AlignedItem<glm::vec3>  ambience;   //!< The ambient colour of the scene.
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_SCENE_
