#include "LightBuffer.hpp"


bool LightBuffer::initialise (const Texture& depthStencilTexture, GLenum internalFormat,
    GLsizei width, GLsizei height, GLuint colourTextureUnit) noexcept
{
    // Ensure the object is in a stable state upon failure by using temporary objects.
    auto fbo    = Framebuffer { };
    auto colour = Texture2D { };

    // Initialise the objects.
    if (!(fbo.initialise() && colour.initialise (colourTextureUnit)))
    {
        return false;
    }

    // Allocate enough memory in the renderbuffer.
    colour.allocateImmutableStorage (internalFormat, width, height);

    // Set up the framebuffer and check the validity.
    fbo.attachTexture  (colour, GL_COLOR_ATTACHMENT0);
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
