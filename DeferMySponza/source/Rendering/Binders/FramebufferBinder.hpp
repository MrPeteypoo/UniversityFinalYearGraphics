#pragma once

#if !defined    _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_
#define         _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_


// Personal headers.
#include <Rendering/Objects/Framebuffer.hpp>


/// <summary>
/// A simple RAII utility to bind a given framebuffer on construction and bind the screen when going out of scope.
/// </summary>
template <GLenum target>
struct FramebufferBinder final
{
    static_assert (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER, 
        "FramebufferBinder can only be used for GL_FRAMEBUFFER, GL_READ_FRAMEBUFFER and GL_DRAW_FRAMEBUFFER.");

    inline FramebufferBinder() noexcept = default;
    
    inline FramebufferBinder (const Framebuffer& buffer) noexcept
    {
        bind (buffer);
    }

    inline FramebufferBinder (const GLuint buffer) noexcept
    {
        bind (buffer);
    }

    inline ~FramebufferBinder()
    {
        unbind();
    }

    inline void bind (const Framebuffer& buffer) const noexcept
    {
        glBindFramebuffer (target, buffer.getID());
    }

    inline void bind (const GLuint buffer) const noexcept
    {
        glBindFramebuffer (target, buffer);
    }

    inline void unbind() const noexcept
    {
        glBindFramebuffer (target, 0);
    }
};

#endif // _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_