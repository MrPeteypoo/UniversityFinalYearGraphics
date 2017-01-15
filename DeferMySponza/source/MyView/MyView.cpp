#include "MyView.hpp"


// STL headers.
#include <iostream>


void MyView::windowViewWillStart (tygra::Window*) noexcept
{
    assert (m_scene != nullptr);

    if (!m_renderer.initialise (m_scene, { 1280, 720 }, { 1280, 720 }))
    {
        std::cerr << "Renderer failed to initialise." << std::endl;
    }

    GLint test;
    glGetIntegerv (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &test);
    std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << test << std::endl;
    
    glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS, &test);
    std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << test << std::endl;
}


void MyView::windowViewDidStop (tygra::Window*) noexcept
{
    m_renderer.clean();
}


void MyView::windowViewDidReset (tygra::Window*, int width, int height) noexcept
{
    // Reset the display resolution of the renderer.
    m_renderer.setInternalResolution ({ width, height });
    m_renderer.setDisplayResolution ({ width, height });
}


void MyView::windowViewRender (tygra::Window*) noexcept
{
    m_renderer.render();
}