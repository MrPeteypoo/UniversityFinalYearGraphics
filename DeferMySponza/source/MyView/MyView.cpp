#include "MyView.hpp"


// STL headers.
#include <iostream>


void MyView::setThreadingMode (bool useMultipleThreads) noexcept
{
    m_renderer.setThreadingMode (useMultipleThreads);
}


void MyView::setRenderingMode (bool useDeferredRendering) noexcept
{
    m_renderer.setRenderingMode (useDeferredRendering);
}

        
void MyView::syncResolutions (bool shouldSyncResolutions) noexcept
{
    if (shouldSyncResolutions)
    {
        // Change the resolution if we need to sync.
        if (!m_syncResolutions)
        {
            m_renderer.setInternalResolution ({ m_displayWidth, m_displayHeight });
        }
    }

    m_syncResolutions = shouldSyncResolutions;
}


void MyView::setInternalResolution (int width, int height) noexcept
{
    m_syncResolutions = false;
    m_renderer.setInternalResolution ({ width, height });
}


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
    if (m_syncResolutions)
    {
        m_renderer.setInternalResolution ({ width, height });
    }
    
    m_renderer.setDisplayResolution ({ width, height });
    
    // Keep track of the display resolution.
    m_displayWidth  = width;
    m_displayHeight = height;
}


void MyView::windowViewRender (tygra::Window*) noexcept
{
    m_renderer.render();
}