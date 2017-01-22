#pragma once

#if !defined    _RENDERING_OBJECTS_FRAMEBUFFER_
#define         _RENDERING_OBJECTS_FRAMEBUFFER_

// STL headers
#include <vector>


// Engine headers.
#include <tgl/tgl.h>


// Forward declarations.
class Renderbuffer;
class Texture;


/// <summary>
/// An RAII encapsulation of an OpenGL framebuffer object. Framebuffers can have multiple drawing attachments such as
/// renderbuffers and textures.
/// </summary>
class Framebuffer final
{
    public:

        Framebuffer() noexcept                      = default;

        Framebuffer (Framebuffer&& move) noexcept;
        Framebuffer& operator= (Framebuffer&& move) noexcept;

        Framebuffer (const Framebuffer&)            = delete;
        Framebuffer& operator= (const Framebuffer&) = delete;

        ~Framebuffer() { clean(); }


        /// <summary> Check if the buffer has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_buffer != 0U; }
        
        /// <summary> Gets the OpenGL ID of framebuffer object. </summary>
        inline GLuint getID() const noexcept        { return m_buffer; }


        /// <summary> 
        /// Attempt to initialise the framebuffer object. Upon successful construction, objects can be attached as
        /// colour attachments or depth/stencil attachments. Upon failure the object won't be modified.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> Deletes the buffer. </summary>
        void clean() noexcept;
        

        /// <summary> Attaches a renderbuffer to the framebuffer. </summary>
        /// <param name="renderbuffer"> The renderbuffer to be attached. </param>
        /// <param name="attachment"> 
        /// The location where the renderbuffer should be attached, e.g GL_COLOR_ATTACHMENT0,
        /// GL_DEPTH_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT.
        /// </param>
        /// <param name="asDrawBuffer"> Whether the attachment should be added as a draw buffer. </param>
        void attachRenderbuffer (const Renderbuffer& renderbuffer, GLenum attachment, bool asDrawBuffer = true) noexcept;

        /// <summary> Attaches a texture to the framebuffer. </summary>
        /// <param name="texture"> The texture to be attached. </param>
        /// <param name="attachment"> 
        /// The location where the texture should be attached, e.g GL_COLOR_ATTACHMENT0,
        /// GL_DEPTH_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT.
        /// </param>
        /// <param name="asDrawBuffer"> Whether the attachment should be added as a draw buffer. </param>
        /// <param name="level"> The mipmap level to attach. </param>
        void attachTexture (const Texture& texture, GLenum attachment, bool asDrawBuffer = true, GLint level = 0) noexcept;

        /// <summary> 
        /// Attempts to complete the buffer by specifying draw targets.
        /// </summary>
        /// <returns> Whether the framebuffer is complete and therefore can be used as a framebuffer target. </returns>
        bool complete() noexcept;

        /// <summary> Invalidates every attachment causing their contents to become undefined. </summary>
        void invalidateAllAttachments() noexcept;

    private:

        GLuint              m_buffer        { 0 };  //!< The OpenGL ID representing the framebuffer object.
        std::vector<GLenum> m_attachments   { };    //!< The points at which attachments have been attached, e.g. GL_COLOR_ATTACHMENT0.
        std::vector<GLenum> m_drawBuffers   { };    //!< The draw buffers that should be applied to the framebuffer.
};

#endif // _RENDERING_OBJECTS_FRAMEBUFFER_