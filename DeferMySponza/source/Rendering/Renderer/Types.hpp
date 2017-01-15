#pragma once

#if !defined    _RENDERING_RENDERER_TYPES_
#define         _RENDERING_RENDERER_TYPES_

// Engine headers.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <tgl/tgl.h>


// Forward declarations.
template <size_t Partitions>
class PersistentMappedBuffer;


namespace types
{
    constexpr static auto multiBuffering = size_t { 2 }; //!< How much multi-buffering should be performed on dynamic buffers.

    using PMB               = PersistentMappedBuffer<multiBuffering>;
    using VertexPosition    = glm::vec3;
    using VertexNormal      = glm::vec3;
    using VertexUV          = glm::vec2;
    using MaterialID        = GLuint;
    using ModelTransform    = glm::mat4;
    using Element           = GLuint;
}

#endif // _RENDERING_RENDERER_TYPES_