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


void* Buffer::mapRange (const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const noexcept
{
    // Only attempt to map the buffer if read operations have been specified.
    if (access & GL_MAP_READ_BIT)
    {
        // Ignore disallowed access parameters.
        const auto readAccess = access & (GL_MAP_READ_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
 
        // Attempt to map the buffer.
        const auto pointer = glMapNamedBufferRange (m_buffer, offset, length, access);

        // If it returns NULL then we should return the more universal nullptr.
        return pointer == NULL ? nullptr : pointer;
    }

    return nullptr;
}


void* Buffer::mapRange (const GLintptr offset, const GLsizeiptr length, const GLbitfield access) noexcept
{
    // Attempt to map the buffer.
    const auto pointer = glMapNamedBufferRange (m_buffer, offset, length, access);

    // If it returns NULL then we should return the more universal nullptr.
    return pointer == NULL ? nullptr : pointer;
}


bool Buffer::unmap() const noexcept
{
    // Unmapping returns a GLboolean which is a byte, not a bool.
    const auto result = glUnmapNamedBuffer (m_buffer);

    // Manually return a bool value instead of a potentially slow cast.
    return result ? true : false;
}


void Buffer::invalidate() noexcept
{
    glInvalidateBufferData (m_buffer);
}


void Buffer::invalidateRange (const GLintptr offset, const GLsizeiptr length) noexcept
{
    glInvalidateBufferSubData (m_buffer, offset, length);
}