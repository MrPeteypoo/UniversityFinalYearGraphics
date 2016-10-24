#pragma once

#if !defined    _RENDERING_RENDERING_PASS_CONFIGURATOR_
#define	        _RENDERING_RENDERING_PASS_CONFIGURATOR_


// STL headers.
#include <string>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/Programs.hpp>
#include <Rendering/PassConfigurator/Shaders.hpp>


/// <summary>
/// Enables the easy configuration of rendering passes, also stores the OpenGL programs and shaders used in the renderer.
/// </summary>
class PassConfigurator final
{
    public:        
        GLuint get() const noexcept { return m_programs.sceneConstruction.getID(); }
        PassConfigurator()                                      = default;
        PassConfigurator (PassConfigurator&&)                   = default;
        PassConfigurator& operator= (PassConfigurator&&)        = default;
        ~PassConfigurator()                                     = default;

        PassConfigurator (const PassConfigurator&)              = delete;
        PassConfigurator& operator= (const PassConfigurator&)   = delete;


        /// <summary> Checks if the programs and shaders have been loaded correctly. </summary>
        inline bool isInitialised() const noexcept { return m_programs.isInitialised() && m_shaders.isInitialised(); }


        /// <summary> Initialise the shaders and programs required for rendering. </summary>
        /// <returns> Whether the initialisation was successful. </returns>
        bool initialise() noexcept;

        /// <summary> Removes and deletes all shaders and programs. </summary>
        void clean() noexcept;

        /// <summary> Unbind the current program. </summary>
        inline void unbind() const noexcept { m_programs.unbind(); }


        /// <summary> Initialises the OpenGL context, independent of any enabled mode. </summary>
        void prepareDraw() const noexcept;
        
        /// <summary> Prepares OpenGL for a scene construction rendering pass. </summary>
        inline void switchToSceneConstructionMode() const noexcept;

        /// <summary> Prepares OpenGL for a lighting pass using directional light shaders. </summary>
        inline void switchToDirectionalLightMode() const noexcept;

        /// <summary> Prepares OpenGL for a lighting pass using point light shaders. </summary>
        inline void switchToPointLightMode() const noexcept;

        /// <summary> Prepares OpenGL for a lighting pass using spotlight shaders. </summary>
        inline void switchToSpotlightMode() const noexcept;

    private:

        /// <summary> Enables depth testing and writing, disables blending but enables ambient lighting. </summary>
        void useConstructionConfiguration (const GLuint program) const noexcept;

        /// <summary> Disables writing to the depth buffer, enables blending and uses equality for depth testing. </summary>
        void useLightingConfiguration (const GLuint program) const noexcept;
        
        Programs    m_programs  { };    //!< Stores the programs used in different rendering passes.
        Shaders     m_shaders   { };    //!< Stores the shaders available to programs.
        //Uniforms    m_uniforms  { };    //!< Stores the uniform data
};


void PassConfigurator::switchToSceneConstructionMode() const noexcept
{
    useConstructionConfiguration (m_programs.sceneConstruction.getID());
}


void PassConfigurator::switchToDirectionalLightMode() const noexcept
{
    useLightingConfiguration (m_programs.directionalLighting.getID());
}


void PassConfigurator::switchToPointLightMode() const noexcept
{
    useLightingConfiguration (m_programs.pointLighting.getID());
}


void PassConfigurator::switchToSpotlightMode() const noexcept
{
    useLightingConfiguration (m_programs.spotlighting.getID());
}

#endif // _RENDERING_RENDERING_PASS_CONFIGURATOR_