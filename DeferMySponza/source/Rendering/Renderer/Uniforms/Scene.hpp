#pragma once

#if !defined    _RENDERING_UNIFORMS_SCENE_
#define         _RENDERING_UNIFORMS_SCENE_

// Engine headers.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block representing information about the scene being rendered.
/// </summary>
struct Scene final
{
    glm::mat4   projection          { 1.f };    //!< The projection matrix used during the rendering of the entire frame.
    glm::mat4   view                { 1.f };    //!< The view matrix from the current cameras position and direction.

    glm::vec3   cameraPosition      { 0.f };    //!< The world-space position of the camera.
    float       _unused1            { 0.f };    //!< Acts as padding for cameraPosition.

    glm::vec3   ambience            { 0.f };    //!< The ambient colour of the scene.
    float       _unused2            { 0.f };    //!< Acts as padding for ambience.
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_SCENE_
