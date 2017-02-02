#pragma once

#if !defined    _RENDERING_PASS_CONFIGURATOR_
#define	        _RENDERING_PASS_CONFIGURATOR_

// Engine headers.
#include <glm/vec4.hpp>
#include <tgl/tgl.h>


/// <summary>
/// Enables the easy configuration of the multiple passes required for the deferred shading technique.
/// </summary>
class PassConfigurator final
{
    public:

        /// <summary> Prepares OpenGL for a forward rendering an entire frame. </summary>
        static void forwardRender() noexcept;

        /// <summary> Prepares OpenGL for shadow mapping. </summary>
        static void shadowMapPass() noexcept;

        /// <summary> Prepares OpenGL for the geometry pass. </summary>
        static void geometryPass() noexcept;

        /// <summary> Prepares OpenGL to apply global lighting after the geometry pass. </summary>
        static void globalLightPass() noexcept;

        /// <summary> Prepares OpenGL to apply lighting via light volumes after applying global light. </summary>
        static void lightVolumePass() noexcept;

    private:

        constexpr static GLuint     skyStencilValue { 128 };    //!< The stencil value representing the sky.
        constexpr static GLfloat    tyroneBlue      { 0.25f };  //!< Ensure we use Tyrone blue for clearing! He's loves his blue!
};

#endif // _RENDERING_RENDERING_PASS_CONFIGURATOR_