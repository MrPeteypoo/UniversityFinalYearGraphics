#include "LightBuffer.hpp"


bool LightBuffer::initialise (const Texture& depthStencilTexture, GLenum internalFormat,
    GLsizei width, GLsizei height, GLsizei samples) noexcept
{
    // Ensure the object is in a stable state upon failure by using temporary objects.
    auto fbo    = Framebuffer { };
    auto colour = Renderbuffer { };

    // Initialise the objects.
    if (!(fbo.initialise() && colour.initialise()))
    {
        return false;
    }

    // Allocate enough memory in the renderbuffer.
    colour.allocate (internalFormat, width, height, samples);

    // Set up the framebuffer and check the validity.
    fbo.attachRenderbuffer  (colour, GL_COLOR_ATTACHMENT0);
    fbo.attachTexture (depthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT, false);

    if (!fbo.complete())
    {
        return false;
    }

    m_fbo       = std::move (fbo);
    m_colour    = std::move (colour);

    return true;
}


void LightBuffer::clean() noexcept
{
    m_fbo.clean();
    m_colour.clean();
}
