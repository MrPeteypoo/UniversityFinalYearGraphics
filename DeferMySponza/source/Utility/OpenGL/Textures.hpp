#pragma once

#if !defined    _UTILITY_OPENGL_TEXTURES_
#define         _UTILITY_OPENGL_TEXTURES_

// Personal headers.
#include <tgl/tgl.h>


namespace util
{
    /// <summary> Gets the internal format for the given number of components. </summary>
    GLenum internalFormat (const size_t components) noexcept;
}

#endif // _UTILITY_OPENGL_TEXTURES_