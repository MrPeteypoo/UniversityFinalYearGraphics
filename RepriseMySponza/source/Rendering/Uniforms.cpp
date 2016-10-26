#include "Uniforms.hpp"


// Engine headers.
#include <scene/scene.hpp>


// Personal headers.
#include <Rendering/PassConfigurator/Programs.hpp>
#include <Utility/OpenGL/Buffers.hpp>
#include <Utility/Scene.hpp>


bool Uniforms::initialise() noexcept
{
    if (!m_ubo.initialise())
    {
        return false;
    }

    m_ubo.allocate (sizeof (UniformBlocks), GL_UNIFORM_BUFFER, GL_STREAM_DRAW);
    return true;
}


void Uniforms::clean() noexcept
{
    m_ubo.clean();
    m_data = UniformBlocks { };
}


void Uniforms::updateScene (const scene::Context* const scene, const glm::mat4& projection, const glm::mat4& view) noexcept
{
    m_data.scene.projection = projection;
    m_data.scene.view = view;
    m_data.scene.cameraPosition = glm::vec4 (util::toGLM (scene->getCamera().getPosition()), 0.f);
    m_data.scene.ambience = glm::vec4 (util::toGLM (scene->getAmbientLightIntensity()), 0.f);

    m_ubo.placeInside (m_data.scene, GL_UNIFORM_BUFFER, 0);
}


void Uniforms::updateDirectionalLight (const scene::DirectionalLight& light) noexcept
{
    m_data.directionalLight.direction = glm::vec4 (util::toGLM (light.getDirection()), 0.f);
    m_data.directionalLight.intensity = glm::vec4 (util::toGLM (light.getIntensity()), 0.f);

    m_ubo.placeInside (m_data.directionalLight, GL_UNIFORM_BUFFER, m_data.directionalLightOffset);
}


void Uniforms::updatePointLight (const scene::PointLight& light) noexcept
{
    m_data.pointLight.position  = glm::vec4 (util::toGLM (light.getPosition()), 0.f);
    m_data.pointLight.intensity = glm::vec4 (util::toGLM (light.getIntensity()), 0.f);

    m_ubo.placeInside (m_data.pointLight, GL_UNIFORM_BUFFER, m_data.pointLightOffset);
}


void Uniforms::updateSpotlight (const scene::SpotLight& light) noexcept
{
    m_data.spotlight.position   = glm::vec4 (util::toGLM (light.getPosition()), 0.f);
    m_data.spotlight.direction  = util::toGLM (light.getDirection());
    m_data.spotlight.coneAngle  = light.getConeAngleDegrees();
    m_data.spotlight.intensity  = util::toGLM (light.getIntensity());

    m_ubo.placeInside (m_data.spotlight, GL_UNIFORM_BUFFER, m_data.spotlightOffset);
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
     
    success = success || util::bindBlockToProgram 
    ( 
        m_ubo, 
        program, 
        m_data.directionalLightName,
        m_data.directionalLightBlock, 
        m_data.directionalLightOffset, 
        sizeof (DirectionalLight)
    );
     
    success = success || util::bindBlockToProgram 
    ( 
        m_ubo, 
        program, 
        m_data.pointLightName,
        m_data.pointLightBlock, 
        m_data.pointLightOffset, 
        sizeof (PointLight)
    );
     
    success = success || util::bindBlockToProgram 
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