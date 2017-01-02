#pragma once

#if !defined    _RENDERING_RENDERING_PASS_CONFIGURATOR_
#define	        _RENDERING_RENDERING_PASS_CONFIGURATOR_

// STL headers.
#include <string>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/Internals/Programs.hpp>
#include <Rendering/PassConfigurator/Internals/Shaders.hpp>


// Forward declarations.
class GeometryBuffer;
class LightBuffer;


/// <summary>
/// Enables the easy configuration of the multiple passes required for the deferred shading technique. The 
/// PassConfigurator also stores the programs used in the scene.
/// </summary>
class PassConfigurator final
{
    public:

        PassConfigurator()                                      = default;

        PassConfigurator (PassConfigurator&&)                   = default;
        PassConfigurator& operator= (PassConfigurator&&)        = default;

        PassConfigurator (const PassConfigurator&)              = delete;
        PassConfigurator& operator= (const PassConfigurator&)   = delete;

        ~PassConfigurator()                                     = default;


        /// <summary> Checks if the programs and shaders have been loaded correctly. </summary>
        inline bool isInitialised() const noexcept          { return m_programs.isInitialised() && m_shaders.isInitialised(); }

        /// <summary> Retrieves the collection of programs loaded by the configurator. </summary>
        inline const Programs& getPrograms() const noexcept { return m_programs; }

        /// <summary> Retrieves the collection of shaders loaded by the configurator. </summary>
        inline const Shaders& getShaders() const noexcept   { return m_shaders; }


        /// <summary> Initialise the shaders and programs required for rendering. </summary>
        /// <returns> Whether the initialisation was successful. </returns>
        bool initialise() noexcept;

        /// <summary> Removes and deletes all shaders and programs. </summary>
        void clean() noexcept;

        
        /// <summary> Prepares OpenGL for the geometry pass. </summary>
        void geometryPass (const GeometryBuffer& gbuffer) const noexcept;

        /// <summary> Prepares OpenGL to apply global lighting after the geometry pass. </summary>
        void globalLightPass (const LightBuffer& lbuffer) const noexcept;

        /// <summary> Prepares OpenGL to apply point lighting after applying global light. </summary>
        void pointLightPass() const noexcept;

        /// <summary> Prepares OpenGL to apply spotlighting after a point lighting pass. </summary>
        void spotlightPass() const noexcept;

    private:

        Programs    m_programs  { };    //!< Stores the programs used in different rendering passes.
        Shaders     m_shaders   { };    //!< Stores the shaders available to programs.
};

#endif // _RENDERING_RENDERING_PASS_CONFIGURATOR_