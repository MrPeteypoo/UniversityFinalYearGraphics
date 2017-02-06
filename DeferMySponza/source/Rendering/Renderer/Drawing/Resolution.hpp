#pragma once

#if !defined    _RENDERING_RENDERER_RESOLUTION
#define	        _RENDERING_RENDERER_RESOLUTION

// Engine headers.
#include <tgl/tgl.h>


/// <summary> Contains the internal and display resolution of the renderer. </summary>
struct Resolution final
{
    Resolution() noexcept                               = default;
    Resolution (Resolution&&) noexcept                  = default;
    Resolution (const Resolution&) noexcept             = default;
    Resolution& operator= (const Resolution&) noexcept  = default;
    Resolution& operator= (Resolution&&) noexcept       = default;
    ~Resolution()                                       = default;

    GLsizei internalWidth   { 0 };  //!< How many pixels wide off-screen buffers should be.
    GLsizei internalHeight  { 0 };  //!< How many pixels tall the off-screen buffers should be.
    GLsizei displayWidth    { 0 };  //!< The target width of the display resolution.
    GLsizei displayHeight   { 0 };  //!< The target height of the display resolution.
};

#endif // _RENDERING_RENDERER_RESOLUTION

