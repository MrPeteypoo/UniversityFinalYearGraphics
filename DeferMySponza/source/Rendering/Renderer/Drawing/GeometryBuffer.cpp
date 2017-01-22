#include "GeometryBuffer.hpp"


bool GeometryBuffer::isInitialised() const noexcept
{
    return m_fbo.isInitialised() && m_positions.isInitialised() && m_normals.isInitialised() && 
        m_materials.isInitialised() && m_depthStencil.isInitialised();
}


bool GeometryBuffer::initialise (const GLsizei width, const GLsizei height, const GLuint startingTextureUnit) noexcept
{
    // Ensure we don't modify this object if initialisation fails.
    auto fbo            = Framebuffer { };
    auto positions      = TextureRectangle { };
    auto normals        = TextureRectangle { };
    auto materials      = TextureRectangle { };
    auto depthStencil   = TextureRectangle { };

    // Attempt to initialise each object.
    if (!(fbo.initialise() && positions.initialise (startingTextureUnit) && normals.initialise (startingTextureUnit + 1) &&
        materials.initialise (startingTextureUnit + 2) && depthStencil.initialise (startingTextureUnit + 3)))
    {
        return false;
    }

    // Allocate memory for each texture.
    positions.allocateImmutableStorage      (GL_RGB32F,             width, height);
    normals.allocateImmutableStorage        (GL_RGB32F,             width, height);
    materials.allocateImmutableStorage      (GL_RGB32F,             width, height);
    depthStencil.allocateImmutableStorage   (GL_DEPTH24_STENCIL8,   width, height);

    // Attach the textures to the framebuffer.
    fbo.attachTexture (positions,       GL_COLOR_ATTACHMENT0 + positionLocation);
    fbo.attachTexture (normals,         GL_COLOR_ATTACHMENT0 + normalLocation);
    fbo.attachTexture (materials,       GL_COLOR_ATTACHMENT0 + materialLocation);
    fbo.attachTexture (depthStencil,    GL_DEPTH_STENCIL_ATTACHMENT, false);

    // Check whether we've succeeded to construct the Gbuffer.
    if (!fbo.complete())
    {
        return false;
    }

    m_fbo           = std::move (fbo);
    m_positions     = std::move (positions);
    m_normals       = std::move (normals);
    m_materials     = std::move (materials);
    m_depthStencil  = std::move (depthStencil);

    return true;
}


void GeometryBuffer::clean() noexcept
{
    m_fbo.clean();
    m_positions.clean();
    m_normals.clean();
    m_materials.clean();
    m_depthStencil.clean();
}