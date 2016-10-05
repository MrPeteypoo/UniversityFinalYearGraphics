#include "UniformData.hpp"



// Engine headers.
#include <scene/DirectionalLight.hpp>
#include <scene/PointLight.hpp>
#include <scene/SpotLight.hpp>



// Personal headers.
#include <Utility/Maths.hpp>
#include <Utility/Scene.hpp>



#pragma region Setters

void MyView::UniformData::setLightCount (const int count)
{
    m_numLights = util::clamp (count, 0, MAX_LIGHTS);
}

void MyView::UniformData::setLight (const int index, const scene::DirectionalLight& light)
{
    // Pre-condition: Index is valid.
    if (index < MAX_LIGHTS && index >= 0)
    {
        // Cache the light to be modified.
        auto& shaderLight = m_lights[index];

        // Move the data across.
        shaderLight.setType (LightType::Directional);
        shaderLight.direction = util::toGLM(light.getDirection());
        shaderLight.colour    = util::toGLM(light.getIntensity());
    }
}

void MyView::UniformData::setLight (const int index, const scene::PointLight& light)
{
    // Pre-condition: Index is valid.
    if (index < MAX_LIGHTS && index >= 0)
    {
        // Cache the light to be modified.
        auto& shaderLight = m_lights[index];

        // Move the data across.
        shaderLight.setType (LightType::Point);
        shaderLight.position = util::toGLM(light.getPosition());
        shaderLight.colour   = util::toGLM(light.getIntensity());
    }
}

void MyView::UniformData::setLight (const int index, const scene::SpotLight& light)
{
    // Pre-condition: Index is valid.
    if (index < MAX_LIGHTS && index >= 0)
    {
        // Cache the light to be modified.
        auto& shaderLight = m_lights[index];

        // Move the data across.
        shaderLight.setType (LightType::Spot);
        shaderLight.position  = util::toGLM(light.getPosition());
        shaderLight.direction = util::toGLM(light.getDirection());
        shaderLight.coneAngle = light.getConeAngleDegrees();
        shaderLight.colour    = util::toGLM(light.getIntensity());
    }
}

void MyView::UniformData::setLight (const int index, const Light& light)
{
    // Pre-condition: Index is valid.
    if (index < MAX_LIGHTS && index >= 0)
    {
        m_lights[index] = light;   
    }
}

#pragma endregion