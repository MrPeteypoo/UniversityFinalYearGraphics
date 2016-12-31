#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_
#define         _RENDERING_OBJECTS_TEXTURE_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// An RAII encapsulation of an OpenGL texture object. Textures contain images of the same format, it could represent 
/// a single image, an array of images or even plain shader-accessible data.
/// </summary>
class Texture
{
    public:

        Texture() noexcept                  = default;

        Texture (Texture&& move) noexcept;
        Texture& operator= (Texture&& move) noexcept;

        Texture (const Texture&)            = delete;
        Texture& operator= (const Texture&) = delete;

        //~Texture() { clean(); }


        /// <summary> Check if the buffer has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_texture != 0U; }
        
        /// <summary> Gets the OpenGL ID of render buffer object. </summary>
        inline GLuint getID() const noexcept        { return m_texture; }


        /// <summary> 
        /// Attempt to initialise the framebuffer object. Upon successful construction, objects can be attached as
        /// colour attachments or depth/stencil attachments.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        //bool initialise() noexcept;

        /// <summary> Deletes the buffer but does not attach/detach the renderbuffer from framebuffers. </summary>
        //void clean() noexcept;

    private:

        GLuint m_texture { 0 }; //!< The OpenGL ID representing the texture object.
};

#endif // _RENDERING_OBJECTS_TEXTURE_