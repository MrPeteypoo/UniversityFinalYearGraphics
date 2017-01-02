#include "Buffer.hpp"


// STL headers.
#include <utility>


Buffer::Buffer (Buffer&& move) noexcept
{
    *this = std::move (move);
}


Buffer& Buffer::operator= (Buffer&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak.
        clean();
        
        m_buffer        = move.m_buffer;
        move.m_buffer   = 0U;
    }

    return *this;
}


bool Buffer::initialise() noexcept
{
    // Generate an object.
    auto buffer = GLuint { 0 };
    glCreateBuffers (1, &buffer);

    // Check the validity before using it.
    if (buffer == 0U)
    {
        return false;
    }

    // Ensure we don't leak.
    clean();
    m_buffer = buffer;

    return true;
}


void Buffer::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteBuffers (1, &m_buffer);
        m_buffer = 0U;
    }
}