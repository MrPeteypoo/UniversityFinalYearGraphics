#include "MyView.hpp"


// STL headers.
#include <iostream>


// Namespaces
using namespace std::chrono;


void MyView::setThreadingMode (bool useMultipleThreads) noexcept
{
    m_renderer.setThreadingMode (useMultipleThreads);
    m_lastFPSDisplay = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
}


void MyView::setRenderingMode (bool useDeferredRendering) noexcept
{
    m_renderer.setRenderingMode (useDeferredRendering);
    m_lastFPSDisplay = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
}


void MyView::setShadingMode (bool usePhysicallyBasedShading) noexcept
{
    m_renderer.setShadingMode (usePhysicallyBasedShading);
    m_lastFPSDisplay = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
}


void MyView::setAntiAliasingMode (SMAA::Quality quality) noexcept
{
    m_renderer.setAntiAliasingMode (quality);
    m_lastFPSDisplay = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
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

    m_syncResolutions   = shouldSyncResolutions;
    m_lastFPSDisplay    = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
}


void MyView::setInternalResolution (int width, int height) noexcept
{
    m_syncResolutions = false;
    m_renderer.setInternalResolution ({ width, height });
    m_lastFPSDisplay = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
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
    if (width == 0 || height == 0)
    {
        width   = 1;
        height  = 1;
    }

    if (m_syncResolutions)
    {
        m_renderer.setInternalResolution ({ width, height });
    }
    
    m_renderer.setDisplayResolution ({ width, height });
    
    // Keep track of the display resolution.
    m_displayWidth      = width;
    m_displayHeight     = height;
    m_lastFPSDisplay    = std::chrono::high_resolution_clock::now();
    m_renderer.resetFrameTimings();
}


void MyView::windowViewRender (tygra::Window*) noexcept
{
    // Lolrandom render.
    m_renderer.render();

    // Check if we should display the FPS.
    const auto now          = std::chrono::high_resolution_clock::now();
    const auto difference   = std::chrono::duration_cast<std::chrono::seconds> (now - m_lastFPSDisplay);

    if (m_displayFPS && difference >= 5s)
    {
        std::cout << "Flush Count: " << m_renderer.getSyncCount() << std::endl;
        std::cout << "Frame Count: " << m_renderer.getFrameCount() << std::endl;
        std::cout << "Min FPS:     " << 1000 / m_renderer.getMaxFrameTime() << std::endl;
        std::cout << "Mean FPS:    " << 1000 / (m_renderer.getTotalFrameTime() / m_renderer.getFrameCount()) << std::endl;
        std::cout << "Max FPS:     " << 1000 / m_renderer.getMinFrameTime() << std::endl;
        std::cout << "Min Time:    " << m_renderer.getMinFrameTime() << "ms" << std::endl;
        std::cout << "Mean Time:   " << m_renderer.getTotalFrameTime() / m_renderer.getFrameCount() << "ms" << std::endl;
        std::cout << "Max Time:    " << m_renderer.getMaxFrameTime() << "ms" << std::endl;
        std::cout << std::endl;
        m_lastFPSDisplay = now;
    }
}