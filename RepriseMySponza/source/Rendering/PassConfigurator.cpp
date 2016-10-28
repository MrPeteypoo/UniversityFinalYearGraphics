#include "PassConfigurator.hpp"


bool PassConfigurator::initialise() noexcept
{
    if (m_shaders.initialise())
    {
        return m_programs.initialise (m_shaders);
    }

    return false;
}


void PassConfigurator::clean() noexcept
{
    m_programs.clean();
    m_shaders.clean();
}


void PassConfigurator::prepareDraw() const noexcept
{
    glEnable (GL_CULL_FACE);
    glClearColor (0.f, 0.f, 0.25f, 0.f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void PassConfigurator::useConstructionConfiguration (const GLuint program) const noexcept
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


void PassConfigurator::useLightingConfiguration (const GLuint program) const noexcept
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