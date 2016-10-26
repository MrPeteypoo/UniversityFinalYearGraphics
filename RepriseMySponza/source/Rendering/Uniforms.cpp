#include "Uniforms.hpp"


// Engine headers.
#include <scene/scene.hpp>


// Personal headers.
#include <Rendering/PassConfigurator/Programs.hpp>
#include <Utility/OpenGL/Buffers.hpp>


bool Uniforms::initialise (const Programs& programs) noexcept
{
    if (!m_ubo.initialise())
    {
        return false;
    }

    m_ubo.allocate (sizeof (UniformBlocks), GL_UNIFORM_BUFFER, GL_STREAM_DRAW);

    return  bindToProgram (programs.sceneConstruction.getID()) &&
            bindToProgram (programs.directionalLighting.getID()) &&
            bindToProgram (programs.pointLighting.getID()) &&
            bindToProgram (programs.spotlighting.getID());
}


void Uniforms::clean() noexcept
{
    m_ubo.clean();
    m_data = UniformBlocks { };
}


void Uniforms::update (const scene::Context* const context) noexcept
{
    m_ubo.placeInside (m_data, GL_UNIFORM_BUFFER, 0);
}


bool Uniforms::bindToProgram (const GLuint program) const noexcept
{
    auto success = util::bindBlockToProgram 
    (
        m_ubo, 
        program, 
        m_data.sceneName, 
        m_data.sceneBlock, 
        m_data.sceneOffset, 
        sizeof (Scene)
    );
     
    success = success && util::bindBlockToProgram 
    ( 
        m_ubo, 
        program, 
        m_data.directionalLightName,
        m_data.directionalLightBlock, 
        m_data.directionalLightOffset, 
        sizeof (DirectionalLight)
    );
     
    success = success && util::bindBlockToProgram 
    ( 
        m_ubo, 
        program, 
        m_data.pointLightName,
        m_data.pointLightBlock, 
        m_data.pointLightOffset, 
        sizeof (PointLight)
    );
     
    success = success && util::bindBlockToProgram 
    ( 
        m_ubo, 
        program, 
        m_data.spotlightName,
        m_data.spotlightBlock, 
        m_data.spotlightOffset, 
        sizeof (Spotlight)
    );
    
    return success;
}