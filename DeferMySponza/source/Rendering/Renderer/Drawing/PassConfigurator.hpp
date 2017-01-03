#pragma once

#if !defined    _RENDERING_PASS_CONFIGURATOR_
#define	        _RENDERING_PASS_CONFIGURATOR_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Enables the easy configuration of the multiple passes required for the deferred shading technique.
/// </summary>
class PassConfigurator final
{
    public:

        /// <summary> Prepares OpenGL for the geometry pass. </summary>
        static void geometryPass() noexcept;

        /// <summary> Prepares OpenGL to apply global lighting after the geometry pass. </summary>
        static void globalLightPass() noexcept;

        /// <summary> Prepares OpenGL to apply lighting via light volumes after applying global light. </summary>
        static void lightVolumePass() noexcept;

    private:

        constexpr static GLuint skyStencilValue { 128 }; //!< The stencil value representing the sky.
};

#endif // _RENDERING_RENDERING_PASS_CONFIGURATOR_