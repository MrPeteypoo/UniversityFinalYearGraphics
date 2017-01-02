#include "PassConfigurator.hpp"


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

    glEnable (GL_CULL_FACE);
    glDepthMask (GL_TRUE);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glClearColor (0.f, 0.f, 0.25f, 0.f);    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Finally use the correct program.
    glUseProgram (m_programs.geometry.getID());
}


void PassConfigurator::globalLightPass (const GeometryBuffer& gbuffer, const LightBuffer& lbuffer) const noexcept
{
}


void PassConfigurator::pointLightPass() const noexcept
{
}


void PassConfigurator::spotlightPass() const noexcept
{
}


void useConstructionConfiguration (const GLuint program) noexcept
{
    // Switch to the given program.
    glUseProgram (program);

    // Ensure the depth test writes to the depth buffer.
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glDepthMask (GL_TRUE);

    // Also ensure ambient lighting can be applied but disable blending.
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable (GL_BLEND);
}


void useLightingConfiguration (const GLuint program) noexcept
{
    // Switch to the given program.
    glUseProgram (program);

    // Ensure the depth test is enabled but we only need to confirm what has already been computed.
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_EQUAL);
    glDepthMask (GL_TRUE);

    // Ensure blending occurs and lighting can be added.
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable (GL_BLEND);
    glBlendEquation (GL_FUNC_ADD);
    glBlendFunc (GL_ONE, GL_ONE);
}