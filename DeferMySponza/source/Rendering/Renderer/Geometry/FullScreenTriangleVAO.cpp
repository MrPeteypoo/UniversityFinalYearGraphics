#include "FullScreenTriangleVAO.hpp"


// Engine headers.
#include <glm/vec2.hpp>


void FullScreenTriangleVAO::attachVertexBuffers (const Buffer& vertices) noexcept
{
    // We need to calculate our strides.
    constexpr auto vertexStride = GLuint { sizeof (glm::vec2) };

    // Attach static buffers.
    vao.attachVertexBuffer (vertices, positionIndex, 0, vertexStride);
}


void FullScreenTriangleVAO::configureAttributes() noexcept
{
    // Enable the position attribute.
    vao.setAttributeStatus (positionIndex, true);
    vao.setAttributeBufferBinding (positionIndex, positionIndex);
    vao.setAttributeFormat (positionIndex, VertexArray::AttributeLayout::Float32,
                            2, GL_FLOAT, 0);
}