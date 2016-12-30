#include "Programs.hpp"


// STL headers.
#include <string>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/Internals/HardCodedShaders.hpp>
#include <Rendering/PassConfigurator/Internals/Shaders.hpp>


bool Programs::initialise (const Shaders& shaders) noexcept
{
    sceneConstruction.initialise();
    directionalLighting.initialise();
    pointLighting.initialise();
    spotlighting.initialise();
    // TODO: Shader attachment is disgusting.
    sceneConstruction.attachShader (shaders.compiled.find (geometryVS)->second);
    sceneConstruction.attachShader (shaders.compiled.find (geometryFS)->second);
    
    directionalLighting.attachShader (shaders.compiled.find (geometryVS)->second);
    directionalLighting.attachShader (shaders.compiled.find (directionalLightFS)->second);
    
    pointLighting.attachShader (shaders.compiled.find (geometryVS)->second);
    pointLighting.attachShader (shaders.compiled.find (pointLightFS)->second);
    
    spotlighting.attachShader (shaders.compiled.find (geometryVS)->second);
    spotlighting.attachShader (shaders.compiled.find (spotlightFS)->second);

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