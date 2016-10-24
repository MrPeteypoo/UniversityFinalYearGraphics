#include "Programs.hpp"


// STL headers.
#include <string>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/HardCodedShaders.hpp>
#include <Rendering/PassConfigurator/Shaders.hpp>


bool Programs::initialise (const Shaders& shaders) noexcept
{
    sceneConstruction.initialise();
    directionalLighting.initialise();
    pointLighting.initialise();
    spotlighting.initialise();

    sceneConstruction.attachShader (shaders.compiled.find (sponzaVS)->second);
    sceneConstruction.attachShader (shaders.compiled.find (sponzaFS)->second);
    
    directionalLighting.attachShader (shaders.compiled.find (sponzaVS)->second);
    directionalLighting.attachShader (shaders.compiled.find (sponzaFS)->second);
    
    pointLighting.attachShader (shaders.compiled.find (sponzaVS)->second);
    pointLighting.attachShader (shaders.compiled.find (sponzaFS)->second);
    
    spotlighting.attachShader (shaders.compiled.find (sponzaVS)->second);
    spotlighting.attachShader (shaders.compiled.find (sponzaFS)->second);

    return sceneConstruction.link() && directionalLighting.link() && pointLighting.link() && spotlighting.link();
}


void Programs::clean() noexcept
{
    sceneConstruction.clean();
    directionalLighting.clean();
    pointLighting.clean();
    spotlighting.clean();
    unbind();
}


void Programs::unbind() const noexcept
{
    glUseProgram (0);
}