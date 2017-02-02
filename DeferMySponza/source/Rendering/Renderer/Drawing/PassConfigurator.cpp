#include "PassConfigurator.hpp"


void PassConfigurator::forwardRender() noexcept
{
    // We need to perform the depth test and write the result to the buffer.
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDepthFunc (GL_LEQUAL);

    // We don't need the stencil test at all.
    glDisable (GL_STENCIL_TEST);

    // We don't need blending at all.
    glDisable (GL_BLEND);

    // Ensure we only draw the front faces of objects.
    glEnable (GL_CULL_FACE);
    glCullFace (GL_BACK);

    // Finally clear the frame.
    glClearDepth (GLdouble { 1 });
    glClearColor (0.f, 0.f, tyroneBlue, 0.f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void PassConfigurator::shadowMapPass() noexcept
{
    // We need to perform the depth test and write the data.
    glEnable (GL_DEPTH_TEST);
    glDepthMask (GL_TRUE);
    glDepthFunc (GL_LEQUAL);
    glClearDepth (GLdouble { 1 });

    // Cull the back faces of rendered geometry.
    glEnable (GL_CULL_FACE);
    glCullFace (GL_BACK);

    // We don't need anything else.
    glDisable (GL_STENCIL_TEST);
    glDisable (GL_BLEND);
    glColorMask (GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
}


void PassConfigurator::geometryPass() noexcept
{
    // Ensure we always draw.
    glEnable (GL_STENCIL_TEST);
    glStencilFunc (GL_ALWAYS, 0, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);

    // Disable blending but allow Gbuffer data to be written.
    glDisable (GL_BLEND);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Clear the stored depth and stencil values.
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
    glClearColor (0.f, 0.f, tyroneBlue, 0.f);
    glClear (GL_COLOR_BUFFER_BIT);
}


void PassConfigurator::lightVolumePass() noexcept
{
    // We need to enable the depth test again but keep writing false.
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_GREATER);

    // We need culling again for the light volumes.
    glCullFace (GL_FRONT);

    // We use blending to add the extra lighting to the scene.
    glEnable (GL_BLEND);
    glBlendFunc (GL_ONE, GL_ONE);
    glBlendEquation (GL_FUNC_ADD);
}