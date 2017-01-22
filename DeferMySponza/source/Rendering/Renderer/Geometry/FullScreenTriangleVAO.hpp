#pragma once

#if !defined    _RENDERING_RENDERER_GEOMETRY_FULL_SCREEN_TRIANGLE_VAO_
#define         _RENDERING_RENDERER_GEOMETRY_FULL_SCREEN_TRIANGLE_VAO_

// Personal headers.
#include <Rendering/Objects/VertexArray.hpp>


/// <summary> 
/// A VAO used for storing oversized triangles which cover the entire screen. This avoids the issue of overshading on
/// the diagonal of a quad.
/// </summary>
struct FullScreenTriangleVAO final
{
    constexpr static auto positionIndex = GLuint { 0 }; //!< The attribute and buffer index for position data.
    constexpr static auto vertexCount   = GLuint { 3 }; //!< How many vertices make up the triangle.

    VertexArray vao { }; //!< A VAO containing the configuration of an oversized triangle.


    FullScreenTriangleVAO() noexcept                                            = default;
    FullScreenTriangleVAO (FullScreenTriangleVAO&&) noexcept                    = default;
    FullScreenTriangleVAO (const FullScreenTriangleVAO&) noexcept               = default;
    FullScreenTriangleVAO& operator= (const FullScreenTriangleVAO&) noexcept    = default;
    FullScreenTriangleVAO& operator= (FullScreenTriangleVAO&&) noexcept         = default;
    ~FullScreenTriangleVAO()                                                    = default;


    /// <summary> Attachs the given buffer containing vertex data for the oversized triangle. </summary>
    void attachVertexBuffers (const Buffer& vertices) noexcept;

    /// <summary> Sets the binding points and formatting of attributes in the VAO. </summary>
    void configureAttributes() noexcept;
};

#endif // _RENDERING_RENDERER_GEOMETRY_FULL_SCREEN_TRIANGLE_VAO_