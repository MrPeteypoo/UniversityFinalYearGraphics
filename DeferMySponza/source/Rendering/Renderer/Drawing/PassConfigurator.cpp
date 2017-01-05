#include "PassConfigurator.hpp"


void PassConfigurator::geometryPass() noexcept
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

    // Clear the stored depth and stencil values.
    glClearDepth (GLdouble { 1 });
    glClearStencil (skyStencilValue);
    glClear (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void PassConfigurator::globalLightPass() noexcept
{
    // We don't need the depth test for global light.
    glDisable (GL_DEPTH_TEST);
    glDepthMask (GL_FALSE);

    // We should ignore the background and only shade geometry.
    glStencilFunc (GL_NOTEQUAL, skyStencilValue, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

    // Ensure we clear the previously stored colour data.
    glClearColor (0.f, 0.f, 0.25f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT);
}


void PassConfigurator::lightVolumePass() noexcept
{
    // We use blending to add the extra lighting to the scene.
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation (GL_FUNC_ADD);
}