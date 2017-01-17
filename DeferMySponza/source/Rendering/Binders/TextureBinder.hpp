#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_BINDER_
#define         _RENDERING_OBJECTS_TEXTURE_BINDER_

// Personal headers.
#include <Rendering/Objects/Texture.hpp>


/// <summary>
/// A simple RAII utility to bind the desired texture object to the desired texture unit. When the binder goes out
/// of scope the texture will be unbound.
/// </summary>
struct TextureBinder final
{
    inline TextureBinder() noexcept = default;
    
    inline TextureBinder (const Texture& texture) noexcept 
        : m_unit (texture.getDesiredTextureUnit())
    {
        bind (texture.getID());
    }

    inline TextureBinder (const GLuint texture) noexcept
    {
        bind (texture);
    }
    
    inline TextureBinder (const Texture& texture, const GLuint textureUnitOverride) noexcept 
        : m_unit (textureUnitOverride)
    {
        bind (texture.getID());
    }

    inline TextureBinder (const GLuint texture, const GLuint textureUnitOverride) noexcept
        : m_unit (textureUnitOverride)
    {
        bind (texture);
    }

    inline ~TextureBinder()
    {
        unbind();
    }

    inline void bind (const Texture& texture) const noexcept
    {
        bind (texture.getID());
    }

    inline void bind (const GLuint texture) const noexcept
    {
        glBindTextureUnit (m_unit, texture);
    }

    inline void unbind() const noexcept
    {
        //glBindTextureUnit (m_unit, 0);
    }

    private:

       const GLuint m_unit { 0 }; //!< The texture unit being bound to.
};

#endif // _RENDERING_OBJECTS_TEXTURE_BINDER_