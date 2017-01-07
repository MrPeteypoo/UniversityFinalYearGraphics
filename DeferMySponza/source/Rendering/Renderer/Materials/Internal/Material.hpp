#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_
#define         _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Contains the sampler index and physics properties, albedo and normal map of a material.
/// </summary>
struct Material final
{
    GLuint  samplerIndex    { 0 };  //!< The index of the samplers to use when retrieving material data.
    GLuint  properties      { 0 };  //!< The texture containing the physical properties of the mesh.
    GLuint  albedo          { 0 };  //!< Contains the albedo and transparency of the material.
    GLuint  normalMap       { 0 };  //!< The normal map to apply to the surface.
    
    Material()                                      = default;
    Material (Material&&)                           = default;
    Material (const Material&) noexcept             = default;
    Material& operator= (const Material&) noexcept  = default;
    Material& operator= (Material&&) noexcept       = default;
    ~Material()                                     = default;
};

#endif // _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_