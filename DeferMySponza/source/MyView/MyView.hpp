#pragma once

#if !defined    _MY_VIEW_
#define         _MY_VIEW_

// STL headers.
#include <chrono>


// Engine headers.
#include <scene/scene_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>


// Personal headers.
#include <Rendering/Renderer/Renderer.hpp>


/// <summary>
/// Used in creating and rendering of a scene using the Sponza graphics data.
/// </summary>
class MyView final : public tygra::WindowViewDelegate
{
    public:

        MyView() noexcept                               = default;
        MyView (MyView&& move) noexcept                 = default;
        MyView& operator= (MyView&& move) noexcept      = default;
        ~MyView()                                       = default;
        
        MyView (const MyView& copy)                     = delete;
        MyView& operator= (const MyView& copy) noexcept = delete;


        /// <summary> Sets the scene::Context to use for rendering. </summary>
        void setScene (scene::Context* scene) noexcept { m_scene = scene; }

        /// <summary> Sets whether the renderer should use multi-threading or not. </summary>
        void setThreadingMode (bool useMultipleThreads) noexcept;

        /// <summary> Sets whether the renderer should perform forward or deferred rendering. </summary>
        void setRenderingMode (bool useDeferredRendering) noexcept;

        /// <summary> Sets whether the renderer should perform physically-based shading. </summary>
        void setShadingMode (bool usePhysicallyBasedShading) noexcept;

        /// <summary> Sets whether the internal resolution should be synchronised with the display. </summary>
        void syncResolutions (bool shouldSyncResolutions) noexcept;

        /// <summary> Sets the quality setting of the antialiasing to be performed. </summary>
        void setAntiAliasingMode (SMAA::Quality quality) noexcept;

        /// <summary> Sets the internal resolution of the renderer, independent of the display window. </summary>
        void setInternalResolution (int width, int height) noexcept;

        /// <summary> Toggles the display of frame timings. </summary>
        void toggleFPSDisplay () noexcept { m_displayFPS = !m_displayFPS; }
		
    private:

        using Time = std::chrono::high_resolution_clock::time_point;

        scene::Context* m_scene             { nullptr };    //!< The currently used scene pointer.
        Renderer        m_renderer          { };            //!< Renders the scene using OpenGL 4.5.
        bool            m_displayFPS        { false };      //!< Whether the FPS should be reported.
        bool            m_syncResolutions   { true };       //!< Synchronise the internal and display resolutions.
        Time            m_lastFPSDisplay    { };            //!< When the FPS was last displayed.
        int             m_displayWidth      { 640 };        //!< The amount of pixels wide for the display resolution.
        int             m_displayHeight     { 480 };        //!< The amount of pixels tall for the display resolution.

    private:
		
        /// <summary> Causes objects to initialise, constructing the geometry in the scene. </summary>
        void windowViewWillStart (tygra::Window* window) noexcept override final;

        /// <summary> Cleans every object, freeing CPU and GPU resources. </summary>
        void windowViewDidStop (tygra::Window* window) noexcept override final;

        /// <summary> Changes the resolution settings of the renderer to match the new width/height. </summary>
        void windowViewDidReset (tygra::Window* window, int width, int height) noexcept override final;

        /// <summary> Renders the scene according to the current rendering configuration.  </summary>
        void windowViewRender (tygra::Window* window) noexcept override final;

};

#endif // _MY_VIEW_