#pragma once

#if !defined    _RENDERING_OBJECTS_VERTEX_ARRAY_BINDER_
#define         _RENDERING_OBJECTS_VERTEX_ARRAY_BINDER_

// Personal headers.
#include <Rendering/Objects/VertexArray.hpp>


/// <summary>
/// A simple RAII utility to bind a given vertex array object on construction and unbind it on destruction.
/// </summary>
struct VertexArrayBinder final
{
    inline VertexArrayBinder() noexcept = default;
    
    inline VertexArrayBinder (const VertexArray& array) noexcept
    {
        bind (array);
    }

    inline VertexArrayBinder (const GLuint array) noexcept
    {
        bind (array);
    }

    inline ~VertexArrayBinder()
    {
        unbind();
    }

    static inline void bind (const VertexArray& array) noexcept
    {
        glBindVertexArray (array.getID());
    }

    static inline void bind (const GLuint array) noexcept
    {
        glBindVertexArray (array);
    }

    static inline void unbind() noexcept
    {
        glBindVertexArray (0);
    }
};

#endif // _RENDERING_OBJECTS_VERTEX_ARRAY_BINDER_