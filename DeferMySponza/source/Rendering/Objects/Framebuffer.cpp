#include "Framebuffer.hpp"


// STL headers.
#include <utility>


// Personal headers.
#include <Rendering/Binders/RenderbufferBinder.hpp>


Framebuffer::Framebuffer (Framebuffer&& move) noexcept
{
    *this = std::move (move);
}


Framebuffer& Framebuffer::operator= (Framebuffer&& move) noexcept
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


bool Framebuffer::initialise() noexcept
{
    // Generate an object.
    auto buffer = GLuint { 0 };
    glCreateFramebuffers (1, &buffer);

    // Check the validity before using it.
    if (buffer == 0U)
    {
        return false;
    }

    // Ensure we don't leak.
    clean();
    m_buffer = buffer;
    m_attachPoints.reserve (8);

    return true;
}


void Framebuffer::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteFramebuffers (1, &m_buffer);
        m_buffer = 0U;
        m_attachPoints.clear();
    }
}


void Framebuffer::attachRenderbuffer (const Renderbuffer& renderbuffer, GLenum attachment) noexcept
{
    glNamedFramebufferRenderbuffer (m_buffer, attachment, GL_RENDERBUFFER, renderbuffer.getID());
    m_attachPoints.push_back (attachment);
}


bool Framebuffer::complete() noexcept
{
    // Specify the draw targets.
    glNamedFramebufferDrawBuffers (m_buffer, static_cast<GLsizei> (m_attachPoints.size()), m_attachPoints.data());

    // Ensure the status is valid.
    return glCheckNamedFramebufferStatus (m_buffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}


void Framebuffer::invalidateAllAttachments() noexcept
{
    glInvalidateNamedFramebufferData (m_buffer, static_cast<GLsizei> (m_attachPoints.size()), m_attachPoints.data());
}