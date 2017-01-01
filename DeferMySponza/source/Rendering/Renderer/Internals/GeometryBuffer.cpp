#include "GeometryBuffer.hpp"


// Personal headers.
#include <Utility/OpenGL/Textures.hpp>


bool GeometryBuffer::isInitialised() const noexcept
{
    return m_fbo.isInitialised() && m_positions.isInitialised() && m_normals.isInitialised() && 
        m_materials.isInitialised() && m_depthStencil.isInitialised();
}


bool GeometryBuffer::initialise (const GLsizei width, const GLsizei height) noexcept
{
    // Ensure we don't leak any data.
    clean();

    // Ensure we don't modify this object if initialisation fails.
    auto fbo            = Framebuffer { };
    auto positions      = TextureRectangle { };
    auto normals        = TextureRectangle { };
    auto materials      = TextureRectangle { };
    auto depthStencil   = TextureRectangle { };

    // Attempt to initialise each object.
    if (!(fbo.initialise() && positions.initialise (positionTextureUnit) && normals.initialise (normalTextureUnit) &&
        materials.initialise (materialTextureUnit) && depthStencil.initialise (depthTextureUnit)))
    {
        return false;
    }

    // Allocate memory for each texture.
    util::allocateImmutableStorage (positions,      GL_RGB32F,              width, height);
    util::allocateImmutableStorage (normals,        GL_RGB32F,              width, height);
    util::allocateImmutableStorage (materials,      GL_RGB32F,              width, height);
    util::allocateImmutableStorage (depthStencil,   GL_DEPTH24_STENCIL8,    width, height);

    // Attach the textures to the framebuffer.
    fbo.attachTexture (positions,       GL_COLOR_ATTACHMENT0 + positionLocation);
    fbo.attachTexture (normals,         GL_COLOR_ATTACHMENT0 + normalLocation);
    fbo.attachTexture (materials,       GL_COLOR_ATTACHMENT0 + materialLocation);
    fbo.attachTexture (depthStencil,    GL_DEPTH_STENCIL_ATTACHMENT);

    // Check whether we've succeeded to construct the Gbuffer.
    if (!fbo.validate())
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
    if (isInitialised())
    {
        m_fbo.clean();
        m_positions.clean();
        m_normals.clean();
        m_materials.clean();
        m_depthStencil.clean();
    }
}