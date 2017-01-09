#pragma once

#if !defined    _RENDERING_UNIFORMS_TEXTURES_
#define         _RENDERING_UNIFORMS_TEXTURES_

// Engine headers.
#include <tgl/tgl.h>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A uniform block linking the textures in the scene to shader accessible samplers.
/// </summary>
struct Textures final
{
    struct Gbuffer final
    {
        GLint positions { 0 };  //!< A texture rectangle containing the positions of objects.
        GLint normals   { 0 };  //!< A texture rectangle containing the normals of objects.
        GLint materials { 0 };  //!< A texture rectangle containing the texture co-ordinates and material IDs of objects.
    };

    /// <summary> 
    /// Assumes 8 arrays for GL_RGB and GL_RGBA.
    /// </summary>
    struct Arrays final
    {
        GLint arrays[16];   //!< We use 8 arrays for GL_RGB and 8 arrays for GL_RGBA.
    };
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_TEXTURES_
