#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_
#define         _RENDERING_OBJECTS_TEXTURE_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// An RAII encapsulation of an OpenGL texture object. Textures contain images of the same format, it could represent 
/// a single image, an array of images or even plain shader-accessible data. The texture target is the type of images
/// that the texture stores, e.g. GL_TEXTURE_BUFFER, GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, etc.
/// </summary>
template <GLenum Target>
class Texture final
{

    public:

        constexpr static auto target = Target; //!< The target binding of the texture.

    public:

        Texture() noexcept                  = default;

        Texture (Texture&& move) noexcept;
        Texture& operator= (Texture&& move) noexcept;

        Texture (const Texture&)            = delete;
        Texture& operator= (const Texture&) = delete;

        ~Texture() { clean(); }


        /// <summary> Check if the texture has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept              { return m_texture != 0U; }
        
        /// <summary> Gets the OpenGL ID of texture object. </summary>
        inline GLuint getID() const noexcept                    { return m_texture; }

        /// <summary> Gets the enum representing the desired texture unit to bind the texture to. </summary>
        inline GLenum getDesiredTextureUnit() const noexcept    { return m_unit; }

        /// <sumamry> Sets the desired texture unit that the texture should be bound to. </summary>
        /// <param name="unit"> The index of the unit to bind to, the limit is at least 80. </param>
        void setDesiredTextureUnit (const GLuint unit) noexcept;


        /// <summary> 
        /// Attempt to initialise the texture object. Successive calls with delete the currently managed object.
        /// </summary>
        /// <param name="unit"> The desired texture unit to bind the texture to. </param>
        /// <returns> Whether the texture was successfully created or not. </returns>
        bool initialise (const GLuint unit) noexcept;

        /// <summary> Deletes the texture, allowing it to be initialised again. </summary>
        void clean() noexcept;

    private:

        GLuint m_texture { 0 }; //!< The OpenGL ID representing the texture object.
        GLenum m_unit    { 0 }; //!< The desired texture unit to bind the texture too.
};


// Aliases.
using Texture1DArray    = Texture<GL_TEXTURE_1D_ARRAY>;
using Texture2D         = Texture<GL_TEXTURE_2D>;
using TextureCubeMap    = Texture<GL_TEXTURE_CUBE_MAP>;
using TextureRectangle  = Texture<GL_TEXTURE_RECTANGLE>;


// STL headers.
#include <utility>


template <GLenum Target>
Texture<Target>::Texture (Texture<Target>&& move) noexcept
{
    *this = std::move (move);
}


template <GLenum Target>
Texture<Target>& Texture<Target>::operator= (Texture<Target>&& move) noexcept
{
    if (this != &move)
    {
        clean();

        m_texture       = move.m_texture;
        move.m_texture  = 0U;
    }

    return *this;
}


template <GLenum Target>
void Texture<Target>::setDesiredTextureUnit (const GLuint unit) noexcept
{
    m_unit = GL_TEXTURE0 + unit;
}


template <GLenum Target>
bool Texture<Target>::initialise (const GLuint unit) noexcept
{
    clean();
    glGenTextures (1, &m_texture);

    if (m_texture != 0U)
    {
        setDesiredTextureUnit (unit);
        return true;
    }
    
    return false;
}


template <GLenum Target>
void Texture<Target>::clean() noexcept
{
    if (isInitialised())
    {
        glDeleteTextures (1, &m_texture);
        m_texture = 0U;
    }
}

#endif // _RENDERING_OBJECTS_TEXTURE_