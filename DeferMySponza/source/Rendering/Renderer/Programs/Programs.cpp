#include "Programs.hpp"


// Personal headers.
#include <Rendering/Renderer/Programs/HardCodedShaders.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>


bool Programs::initialise (const Shaders& shaders) noexcept
{
    // Create temporary objects.
    Program geo, light, forward;

    // Initialise each temporary object.
    if (!(geo.initialise() && light.initialise() && forward.initialise()))
    {
        return false;
    }

    // Attach the requires shaders to each program.
    geo.attachShader (shaders.find (geometryVS));
    geo.attachShader (shaders.find (geometryFS));
    
    light.attachShader (shaders.find (lightVolumeVS));
    light.attachShader (shaders.find (lightingPassFS));
    light.attachShader (shaders.find (lightsFS));
    light.attachShader (shaders.find (materialFetcherFS));
    light.attachShader (shaders.find (reflectionModelsFS));
    
    forward.attachShader (shaders.find (geometryVS));
    forward.attachShader (shaders.find (forwardRenderFS));
    forward.attachShader (shaders.find (lightsFS));
    forward.attachShader (shaders.find (materialFetcherFS));
    forward.attachShader (shaders.find (reflectionModelsFS));

    // Check they link properly.
    if (!(geo.link() && light.link() && forward.link()))
    {
        return false;
    }

    // We've successfully compiled each program.
    geometryPass    = std::move (geo);
    lightingPass    = std::move (light);
    forward         = std::move (forward);

    return true;
}


void Programs::clean() noexcept
{
    geometryPass.clean();
    lightingPass.clean();
    forwardRender.clean();
}