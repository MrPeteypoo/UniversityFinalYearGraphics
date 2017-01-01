#include "LightBuffer.hpp"


bool LightBuffer::initialise (const TextureRectangle& depthStencilTexture, GLenum internalFormat,
    GLsizei width, GLsizei height, GLsizei samples) noexcept
{
    // Ensure we don't leak.
    clean();

    // Ensure the object is in a stable state upon failure by using temporary objects.
    auto fbo    = Framebuffer { };
    auto colour = Renderbuffer { };

    // Initialise the objects.
    if (!fbo.initialise() && !colour.initialise (internalFormat, width, height, samples))
    {
        return false;
    }

    // Set up the framebuffer and check the validity.
    fbo.attachRenderbuffer  (colour, GL_COLOR_ATTACHMENT0);
    fbo.attachTexture (depthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT);

    if (!fbo.validate())
    {
        return false;
    }

    m_fbo       = std::move (fbo);
    m_colour    = std::move (colour);

    return true;
}


void LightBuffer::clean() noexcept
{
    if (isInitialised())
    {
        m_fbo.clean();
        m_colour.clean();
    }
}
