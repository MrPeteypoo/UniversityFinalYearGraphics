#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_BINDER_
#define         _RENDERING_OBJECTS_TEXTURE_BINDER_

// Personal headers.
#include <Rendering/Objects/Texture.hpp>


/// <summary>
/// A simple RAII utility to bind the desired texture object to the desired texture unit. When the binder goes out
/// of scope the texture will be unbound.
/// </summary>
template <GLenum Target>
struct TextureBinder final
{
    inline TextureBinder() noexcept = default;
    
    inline TextureBinder (const Texture<Target>& texture) noexcept 
        : m_unit (texture.getDesiredTextureUnit())
    {
        bind (texture.getID());
    }

    inline TextureBinder (const GLuint texture) noexcept
    {
        bind (texture);
    }
    
    inline TextureBinder (const Texture<Target>& texture, const GLenum textureUnitOverride) noexcept 
        : m_unit (textureUnitOverride)
    {
        bind (texture.getID());
    }

    inline TextureBinder (const GLuint texture, const GLenum textureUnitOverride) noexcept
        : m_unit (textureUnitOverride)
    {
        bind (texture);
    }

    inline ~TextureBinder()
    {
        unbind();
    }

    inline void bind (const Texture<Target>& texture) const noexcept
    {
        bind (texture.getID());
    }

    inline void bind (const GLuint texture) const noexcept
    {
        glActiveTexture (m_unit);
        glBindTexture (Target, texture);
    }

    inline void unbind() const noexcept
    {
        glActiveTexture (m_unit);
        glBindTexture (Target, 0);
    }

    private:

       const GLenum m_unit { GL_TEXTURE0 }; //!< The texture unit being bound to.
};

#endif // _RENDERING_OBJECTS_TEXTURE_BINDER_