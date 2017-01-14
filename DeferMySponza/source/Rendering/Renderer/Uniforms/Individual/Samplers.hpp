#pragma once

#if !defined    _RENDERING_UNIFORMS_SAMPLERS_
#define         _RENDERING_UNIFORMS_SAMPLERS_

// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/Renderer/Uniforms/Components/ArrayItem.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A collection of uniform samplers which are accessible to programs.
/// </summary>
struct Samplers final
{
    struct Sampler final
    {
        GLint       unit;   //!< The texture unit the sampler is bound to.
        const char* name;   //!< The name to search for the sampler with.

        Sampler (const GLint unit = 0, const char* name = "") noexcept
            : unit (unit), name (name)
        {
        }
    };

    Sampler gbufferPositions    { 0, "gbufferPositions" };  //!< A texture rectangle containing positions of objects.
    Sampler gbufferNormals      { 0, "gbufferNormals" };    //!< A texture rectangle containing world normals of objects.
    Sampler gbufferMaterials    { 0, "gbufferMaterials" };  //!< A texture rectangle containing texture co-ordinates and material IDs of objects.

    Sampler materials           { 0, "materials" };         //!< A texture buffer containing every material in the scene.
    Sampler textures            { 0, "textures" };          //!< An array of textures containing texture maps.
    GLsizei textureSamplerCount { 0 };                      //!< The number of texture arrays in the "textures" sampler.

    Samplers() noexcept                             = default;
    Samplers (Samplers&&) noexcept                  = default;
    Samplers (const Samplers&) noexcept             = default;
    Samplers& operator= (const Samplers&) noexcept  = default;
    Samplers& operator= (Samplers&&) noexcept       = default;
    ~Samplers()                                     = default;
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_SAMPLERS_
