#include "VertexArray.hpp"


// STL headers.
#include <utility>


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>


VertexArray::VertexArray (VertexArray&& move) noexcept
{
    *this = std::move (move);
}


VertexArray& VertexArray::operator= (VertexArray&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak.
        clean();
        
        m_array         = move.m_array;
        move.m_array    = 0U;
    }

    return *this;
}


bool VertexArray::initialise() noexcept
{
    // Generate an object.
    auto array = GLuint { 0 };
    glCreateVertexArrays (1, &array);

    // Check the validity before using it.
    if (array == 0U)
    {
        return false;
    }

    // Ensure we don't leak.
    clean();
    m_array = array;

    return true;
}


void VertexArray::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteVertexArrays (1, &m_array);
        m_array = 0U;
    }
}


void VertexArray::attachVertexBuffer (const Buffer& buffer, GLuint bufferIndex, 
    GLintptr offset, GLsizei stride, GLuint divisor) noexcept
{
    glVertexArrayVertexBuffer (m_array, bufferIndex, buffer.getID(), offset, stride);

    if (divisor != 0)
    {
        glVertexArrayBindingDivisor (m_array, bufferIndex, divisor);
    }
}


void VertexArray::setAttributeStatus (GLuint attributeIndex, bool isEnabled) noexcept
{
    if (isEnabled)
    {
        glEnableVertexArrayAttrib (m_array, attributeIndex);
    }

    else
    {
        glDisableVertexArrayAttrib (m_array, attributeIndex);
    }
}


void VertexArray::setAttributeBufferBinding (GLuint attributeIndex, GLuint bufferIndex) noexcept
{
    glVertexArrayAttribBinding (m_array, attributeIndex, bufferIndex);
}


void VertexArray::setAttributeFormat (GLuint attributeIndex, AttributeLayout layout,
            GLint size, GLenum type, GLuint relativeOffset, GLboolean isNormalised) noexcept
{
    switch (layout)
    {
        case AttributeLayout::Float32:
            glVertexArrayAttribFormat (m_array, attributeIndex, size, type, isNormalised, relativeOffset);
            break;

        case AttributeLayout::Float64:
            glVertexArrayAttribLFormat (m_array, attributeIndex, size, type, relativeOffset);
            break;

        case AttributeLayout::Integer:
            glVertexArrayAttribIFormat (m_array, attributeIndex, size, type, relativeOffset);
            break;
    }
}


void VertexArray::setElementBuffer (const Buffer& elementArrayBuffer) noexcept
{
    glVertexArrayElementBuffer (m_array, elementArrayBuffer.getID());
}