#pragma once

#if !defined    _RENDERING_OBJECTS_RENDERBUFFER_BINDER_
#define         _RENDERING_OBJECTS_RENDERBUFFER_BINDER_

// Personal headers.
#include <Rendering/Objects/Renderbuffer.hpp>


/// <summary>
/// A simple RAII utility to bind a given renderbuffer on construction and then unbind it when going out of scope.
/// </summary>
template <GLenum Target>
struct RenderbufferBinder final
{
    static_assert (Target == GL_RENDERBUFFER, "RenderbufferBinder can only be used for GL_RENDERBUFFER.");

    inline RenderbufferBinder() noexcept = default;
    
    inline RenderbufferBinder (const Renderbuffer& buffer) noexcept
    {
        bind (buffer);
    }

    inline RenderbufferBinder (const GLuint buffer) noexcept
    {
        bind (buffer);
    }

    inline ~RenderbufferBinder()
    {
        unbind();
    }

    inline void bind (const Renderbuffer& buffer) const noexcept
    {
        glBindRenderbuffer (Target, buffer.getID());
    }

    inline void bind (const GLuint buffer) const noexcept
    {
        glBindRenderbuffer (Target, buffer);
    }

    inline void unbind() const noexcept
    {
        glBindRenderbuffer (Target, 0);
    }
};

#endif // _RENDERING_OBJECTS_RENDERBUFFER_BINDER_