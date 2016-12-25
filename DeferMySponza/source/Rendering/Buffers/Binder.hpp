#pragma once

#if !defined    _RENDERING_BUFFERS_BINDER_
#define         _RENDERING_BUFFERS_BINDER_


// Personal headers.
#include <Rendering/Buffers/Buffer.hpp>


/// <summary>
/// A simple RAII utility to bind a given buffer on construction and then unbind it when going out of scope.
/// </summary>
template <GLenum target>
struct Binder final
{
    inline Binder() noexcept = default;
    
    inline Binder (const Buffer& buffer) noexcept
    {
        glBindBuffer (target, buffer.getID());
    }

    inline Binder (const GLuint buffer) noexcept
    {
        glBindBuffer (target, buffer);
    }

    inline ~Binder()
    {
        glBindBuffer (target, 0);
    }
};


#endif // _RENDERING_BUFFERS_BINDER_