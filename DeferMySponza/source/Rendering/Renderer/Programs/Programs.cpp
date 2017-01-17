#include "Programs.hpp"


// STL headers.
#include <iostream>


// Personal headers.
#include <Rendering/Renderer/Programs/HardCodedShaders.hpp>
#include <Rendering/Renderer/Programs/Shaders.hpp>


bool Programs::initialise (const Shaders& shaders) noexcept
{
    // Create temporary objects.
    Program geo, global, light, forward;

    // Initialise each temporary object.
    if (!(geo.initialise() && global.initialise() && light.initialise() && forward.initialise()))
    {
        return false;
    }

    // Attach the requires shaders to each program.
    geo.attachShader (shaders.find (geometryVS));
    geo.attachShader (shaders.find (geometryFS));

    global.attachShader (shaders.find (fullScreenTriangleVS));
    global.attachShader (shaders.find (lightingPassFS));
    global.attachShader (shaders.find (lightsFS));
    global.attachShader (shaders.find (materialFetcherFS));
    global.attachShader (shaders.find (reflectionModelsFS));
    
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

    // Track the success of linking each program.
    auto success = true;

    const auto linkProgram = [&success] (const Program& program, const std::string& name)
    {
        std::cout << "Linking '" << name << "'..." << std::endl;
        success = program.link() && success;
    };

    // Check they link properly.
    linkProgram (geo, "GeometryPass");
    linkProgram (global, "GlobalLightPass");
    linkProgram (light, "LightingPass");
    linkProgram (forward, "ForwardRender");

    if (!success)
    {
        return false;
    }

    // We've successfully compiled each program.
    geometryPass    = std::move (geo);
    globalLightPass = std::move (global);
    lightingPass    = std::move (light);
    forwardRender   = std::move (forward);

    return true;
}


void Programs::clean() noexcept
{
    performActionOnPrograms ([] (Program& program) { program.clean(); });
}