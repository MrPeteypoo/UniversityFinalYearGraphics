#include "PassConfigurator.hpp"


// Personal headers
#include <Rendering/Renderer/Internals/GeometryBuffer.hpp>
#include <Rendering/Renderer/Internals/LightBuffer.hpp>


bool PassConfigurator::initialise() noexcept
{
    auto shaders    = Shaders { };
    auto programs   = Programs { };

    // Attempt to initialise the shaders and programs.
    if (!(shaders.initialise() && programs.initialise (shaders)))
    {
        return false;
    }

    m_shaders   = std::move (shaders);
    m_programs  = std::move (programs); 

    return true;
}


void PassConfigurator::clean() noexcept
{
    m_programs.clean();
    m_shaders.clean();
}


void PassConfigurator::geometryPass (const GeometryBuffer& gbuffer) const noexcept
{
    // We need to perform the depth test and write the data.
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDepthFunc (GL_LEQUAL);

    // Ensure we always draw.
    glEnable (GL_STENCIL_TEST);
    glStencilFunc (GL_ALWAYS, 0, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);

    // Disable blending but allow Gbuffer data to be written.
    glDisable (GL_BLEND);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Cull the back faces of rendered geometry.
    glEnable (GL_CULL_FACE);
    glCullFace (GL_BACK);

    // Bind the gbuffer.
    glBindFramebuffer (GL_FRAMEBUFFER, gbuffer.getFramebuffer().getID());

    // Clear the stored depth and stencil values.
    glClearDepth (GLdouble { 1 });
    glClearStencil (~0);
    glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Finally use the correct program.
    glUseProgram (m_programs.geometry.getID());
}


void PassConfigurator::globalLightPass (const GeometryBuffer& gbuffer, const LightBuffer& lbuffer) const noexcept
{
    // We don't need the depth test for global light.
    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);

    // We should ignore the background and only shade geometry.
    glStencilFunc (GL_NOTEQUAL, 0, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

    // We're reading from the gbuffer, which was previously bound, and writing to the lbuffer.
    glBindFramebuffer (GL_DRAW_FRAMEBUFFER, lbuffer.getFramebuffer().getID());

    // Ensure we clear the previously stored colour data.
    glClearColor (0.f, 0.f, 0.25f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT);

    // Finally use the correct program.
    glUseProgram (m_programs.globalLight.getID());
}


void PassConfigurator::pointLightPass() const noexcept
{
    // We use blending to add the extra lighting to the scene.
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation (GL_FUNC_ADD);

    // Finally use the correct program.
    glUseProgram (m_programs.pointLight.getID());
}


void PassConfigurator::spotlightPass() const noexcept
{
    // Nothing to do but change the program.
    glUseProgram (m_programs.spotlight.getID());
}