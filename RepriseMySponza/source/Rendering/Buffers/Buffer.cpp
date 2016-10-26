#include "Buffer.hpp"


// STL headers.
#include <utility>


// Engine headers.
#include <tgl/tgl.h>


Buffer::Buffer (Buffer&& move) noexcept
{
    *this = std::move (move);
}


Buffer& Buffer::operator= (Buffer&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak.
        if (isInitialised())
        {
            clean();
        }

        m_buffer        = move.m_buffer;
        move.m_buffer   = 0U;
    }

    return *this;
}


bool Buffer::initialise() noexcept
{
    if (isInitialised())
    {
        clean();
    }

    glGenBuffers (1, &m_buffer);
    
    return m_buffer != 0U;
}


void Buffer::clean() noexcept
{
    glDeleteBuffers (1, &m_buffer);
    m_buffer = 0U;
}


void Buffer::allocate (const GLsizeiptr size, const GLenum target, const GLenum usage) const noexcept
{
    glBindBuffer (target, m_buffer);
    glBufferData (target, size, nullptr, usage);
    glBindBuffer (target, 0);
}