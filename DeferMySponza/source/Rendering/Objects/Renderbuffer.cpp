#include "Renderbuffer.hpp"


// STL headers.
#include <utility>


// Personal headers.
#include <Rendering/Binders/RenderbufferBinder.hpp>


Renderbuffer::Renderbuffer (Renderbuffer&& move) noexcept
{
    *this = std::move (move);
}


Renderbuffer& Renderbuffer::operator= (Renderbuffer&& move) noexcept
{
    if (this != &move)
    {
        clean();

        m_buffer        = move.m_buffer;
        move.m_buffer   = 0U;
    }

    return *this;
}


bool Renderbuffer::initialise (GLenum internalFormat, GLsizei width, GLsizei height, GLsizei samples) noexcept
{
    // Don't leak!
    clean();

    // Attempts to create and initialise the buffer.
    glGenRenderbuffers (1, &m_buffer);

    // Check whether we were allocated a buffer.
    if (m_buffer == 0U)
    {
        return false;
    }

    // Attempt to configure the buffer accordingly.
    const auto binder = RenderbufferBinder<GL_RENDERBUFFER> { m_buffer };
    glRenderbufferStorageMultisample (GL_RENDERBUFFER, samples, internalFormat, width, height);

    // Check for any errors.
    if (glGetError())
    {
        clean();
        return false;
    }

    return true;
}


void Renderbuffer::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteRenderbuffers (1, &m_buffer);
        m_buffer = 0U;
    }
}