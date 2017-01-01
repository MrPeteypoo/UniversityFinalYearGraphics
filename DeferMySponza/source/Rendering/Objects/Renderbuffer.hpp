#pragma once

#if !defined    _RENDERING_OBJECTS_RENDERBUFFER_
#define         _RENDERING_OBJECTS_RENDERBUFFER_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// An RAII encapsulation of an OpenGL renderbuffer object. It can be attached to framebuffer objects and drawn to.
/// </summary>
class Renderbuffer final
{
    public:

        Renderbuffer() noexcept                         = default;

        Renderbuffer (Renderbuffer&& move) noexcept;
        Renderbuffer& operator= (Renderbuffer&& move) noexcept;

        Renderbuffer (const Renderbuffer&)              = delete;
        Renderbuffer& operator= (const Renderbuffer&)   = delete;

        ~Renderbuffer() { clean(); }


        /// <summary> Check if the buffer has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_buffer != 0U; }
        
        /// <summary> Gets the OpenGL ID of render buffer object. </summary>
        inline GLuint getID() const noexcept        { return m_buffer; }


        /// <summary> 
        /// Attempt to initialise the render buffer object. Successive calls to this function will cause the stored 
        /// buffer to be deleted and a fresh buffer will take its place.
        /// </summary>
        /// <param name="internalFormat"> The data structure of each pixel contained by the buffer. </param>
        /// <param name="width"> How many pixels wide the buffer should be. </param>
        /// <param name="height"> How many pixels tall the buffer should be. </param>
        /// <param name="samples"> How much multi-sampling should be performed, 0 with deferred shading. </param>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise (GLenum internalFormat, GLsizei width, GLsizei height, GLsizei samples = 0) noexcept;

        /// <summary> Deletes the buffer but does not attach/detach the renderbuffer from framebuffers. </summary>
        void clean() noexcept;

    private:

        GLuint m_buffer { 0 }; //!< The OpenGL ID representing the renderbuffer object.
};

#endif // _RENDERING_OBJECTS_RENDERBUFFER_