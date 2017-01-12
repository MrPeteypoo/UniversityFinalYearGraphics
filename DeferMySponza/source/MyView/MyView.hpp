#pragma once

#if !defined    _MY_VIEW_
#define         _MY_VIEW_

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
		
    private:

        scene::Context* m_scene     { nullptr };    //!< The currently used scene pointer.
        Renderer        m_renderer  { };            //!< Renders the scene using OpenGL 4.5.

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