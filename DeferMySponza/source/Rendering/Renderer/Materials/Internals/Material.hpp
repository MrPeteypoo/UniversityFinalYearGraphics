#pragma once

#if !defined    _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_
#define         _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_

// Engine headers.
#include <glm/vec2.hpp>


/// <summary>
/// Contains the sampler index and physics properties, albedo and normal map of a material. The default material
/// properties should link to default texture maps.
/// </summary>
struct Material final
{
    glm::uvec2  properties  { 0, 0 };   //!< The sampler index and depth to use when looking up the physical properties of the material.
    glm::uvec2  albedo      { 0, 0 };   //!< The sampler index and depth to use when looking up the albedo of the material.
    glm::uvec2  normal      { 0, 1 };   //!< The sampler index and depth to use when looking up the normal map of the material.
    glm::uvec2  unused      { 0 };      //!< Currently unused.
    
    Material() noexcept                             = default;
    Material (Material&&) noexcept                  = default;
    Material (const Material&) noexcept             = default;
    Material& operator= (const Material&) noexcept  = default;
    Material& operator= (Material&&) noexcept       = default;
    ~Material()                                     = default;
};

#endif // _RENDERING_RENDERER_MATERIALS_INTERNAL_MATERIAL_