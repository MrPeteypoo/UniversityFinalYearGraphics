#pragma once

#if !defined    _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_
#define         _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_

// Personal headers.
#include <Rendering/Objects/Framebuffer.hpp>


/// <summary>
/// A simple RAII utility to bind a given framebuffer on construction and bind the screen when going out of scope.
/// </summary>
template <GLenum Target>
struct FramebufferBinder final
{
    static_assert (Target == GL_FRAMEBUFFER || Target == GL_READ_FRAMEBUFFER || Target == GL_DRAW_FRAMEBUFFER, 
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

    static inline void bind (const Framebuffer& buffer) noexcept
    {
        glBindFramebuffer (Target, buffer.getID());
    }

    static inline void bind (const GLuint buffer) noexcept
    {
        glBindFramebuffer (Target, buffer);
    }

    static inline void unbind() noexcept
    {
        glBindFramebuffer (Target, 0);
    }
};

#endif // _RENDERING_OBJECTS_FRAMEBUFFER_BINDER_