#pragma once

#if !defined    _RENDERING_OBJECTS_FRAMEBUFFER_
#define         _RENDERING_OBJECTS_FRAMEBUFFER_

// Engine headers.
#include <tgl/tgl.h>


// Forward declarations.
class Renderbuffer;
template <GLenum> class Texture;


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
        /// colour attachments or depth/stencil attachments.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> Deletes the buffer. </summary>
        void clean() noexcept;
        

        /// <summary> Attaches a renderbuffer to the framebuffer, overriding the bound GL_FRAMEBUFFER. </summary>
        /// <param name="renderbuffer"> The renderbuffer to be attached. </param>
        /// <param name="attachment"> 
        /// The location where the renderbuffer should be attached, e.g GL_COLOR_ATTACHMENT0,
        /// GL_DEPTH_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT.
        /// </param>
        void attachRenderbuffer (const Renderbuffer& renderbuffer, GLenum attachment) noexcept;

        /// <summary> Attaches a texture to the framebuffer, overriding the bound GL_FRAMEBUFFER. </summary>
        /// <param name="texture"> The texture to be attached. </param>
        /// <param name="attachment"> 
        /// The location where the texture should be attached, e.g GL_COLOR_ATTACHMENT0,
        /// GL_DEPTH_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT.
        /// </param>
        /// <param name="level"> The mipmap level to attach. </param>
        template <GLenum target>
        void attachTexture (const Texture<target>& texture, GLenum attachment, GLint level = 0) noexcept;

        /// <summary> 
        /// Attempts to validate the framebuffer, this will check if the buffer is considered "complete" and
        /// therefore whether it can be used as a framebuffer target. Overrides the bound GL_FRAMEBUFFER.
        /// </summary>
        /// <returns> Whether the framebuffer is "complete" and usable. </returns>
        bool validate() noexcept;

    private:

        GLuint m_buffer { 0 }; //!< The OpenGL ID representing the framebuffer object.
};


// Required declarations.
#include <Rendering/Binders/FramebufferBinder.hpp>
#include <Rendering/Objects/Texture.hpp>


template <GLenum target>
void Framebuffer::attachTexture (const Texture<target>& texture, GLenum attachment, GLint level) noexcept
{
    // We need to bind the current framebuffer.
    const auto binder = FramebufferBinder<GL_FRAMEBUFFER> { m_buffer };

    // Add the texture as an attachment.
    glFramebufferTexture (GL_FRAMEBUFFER, attachment, texture.getID(), level);
}

#endif // _RENDERING_OBJECTS_FRAMEBUFFER_