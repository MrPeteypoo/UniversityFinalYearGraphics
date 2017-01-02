#pragma once

#if !defined    _RENDERING_OBJECTS_FRAMEBUFFER_
#define         _RENDERING_OBJECTS_FRAMEBUFFER_

// STL headers
#include <vector>


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
        void attachRenderbuffer (const Renderbuffer& renderbuffer, GLenum attachment) noexcept;

        /// <summary> Attaches a texture to the framebuffer. </summary>
        /// <param name="texture"> The texture to be attached. </param>
        /// <param name="attachment"> 
        /// The location where the texture should be attached, e.g GL_COLOR_ATTACHMENT0,
        /// GL_DEPTH_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT.
        /// </param>
        /// <param name="level"> The mipmap level to attach. </param>
        template <GLenum target>
        void attachTexture (const Texture<target>& texture, GLenum attachment, GLint level = 0) noexcept;

        /// <summary> 
        /// Attempts to complete the buffer by specifying draw targets.
        /// </summary>
        /// <returns> Whether the framebuffer is complete and therefore can be used as a framebuffer target. </returns>
        bool complete() noexcept;

        /// <summary> Invalidates every attachment causing their contents to become undefined. </summary>
        void invalidateAllAttachments() noexcept;

    private:

        GLuint              m_buffer        { 0 };  //!< The OpenGL ID representing the framebuffer object.
        std::vector<GLenum> m_attachPoints  { };    //!< The points at which attachments have been attached, e.g. GL_COLOR_ATTACHMENT0.
};


// Required declarations.
#include <Rendering/Objects/Texture.hpp>


template <GLenum target>
void Framebuffer::attachTexture (const Texture<target>& texture, GLenum attachment, GLint level) noexcept
{
    // Add the texture as an attachment.
    glNamedFramebufferTexture (m_buffer, attachment, texture.getID(), level);
    m_attachPoints.push_back (attachment);
}

#endif // _RENDERING_OBJECTS_FRAMEBUFFER_