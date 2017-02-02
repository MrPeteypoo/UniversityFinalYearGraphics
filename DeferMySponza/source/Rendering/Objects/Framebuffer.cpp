#include "Framebuffer.hpp"


// STL headers.
#include <utility>


// Personal headers.
#include <Rendering/Objects/Renderbuffer.hpp>
#include <Rendering/Objects/Texture.hpp>


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
        m_drawBuffers   = std::move (move.m_drawBuffers);
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
    m_drawBuffers.reserve (8);

    return true;
}


void Framebuffer::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteFramebuffers (1, &m_buffer);
        m_buffer = 0U;
        m_drawBuffers.clear();
    }
}


void Framebuffer::attachRenderbuffer (const Renderbuffer& renderbuffer, GLenum attachment, bool asDrawBuffer) noexcept
{
    glNamedFramebufferRenderbuffer (m_buffer, attachment, GL_RENDERBUFFER, renderbuffer.getID());
    
    if (asDrawBuffer)
    {
        addDrawBuffer (attachment);
    }
}


void Framebuffer::attachTexture (const Texture& texture, GLenum attachment, bool asDrawBuffer, GLint level) noexcept
{
    // Add the texture as an attachment.
    glNamedFramebufferTexture (m_buffer, attachment, texture.getID(), level);
    
    if (asDrawBuffer)
    {
        addDrawBuffer (attachment);
    }
}


void Framebuffer::attachTextureLayer (const Texture& texture, GLenum attachment, GLint layer, 
    bool asDrawBuffer, GLint level) noexcept
{
    // Add the texture as an attachment.
    glNamedFramebufferTextureLayer (m_buffer, attachment, texture.getID(), level, layer);
    
    if (asDrawBuffer)
    {
        addDrawBuffer (attachment);
    }
}


bool Framebuffer::complete() noexcept
{
    // Specify the draw targets.
    glNamedFramebufferDrawBuffers (m_buffer, static_cast<GLsizei> (m_drawBuffers.size()), m_drawBuffers.data());

    // Ensure the status is valid.
    return glCheckNamedFramebufferStatus (m_buffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}