#include "Programs.hpp"


// Personal headers.
#include <Rendering/PassConfigurator/Internals/HardCodedShaders.hpp>
#include <Rendering/PassConfigurator/Internals/Shaders.hpp>


bool Programs::initialise (const Shaders& shaders) noexcept
{
    // Create temporary objects.
    Program geo, global, point, spot;

    // Initialise each temporary object.
    if (!(geo.initialise(), global.initialise(), point.initialise(), spot.initialise()))
    {
        return false;
    }

    // Attach the requires shaders to each program.
    // TODO: Shader attachment is disgusting.
    geo.attachShader (shaders.compiled.find (geometryVS)->second);
    geo.attachShader (shaders.compiled.find (geometryFS)->second);
    
    global.attachShader (shaders.compiled.find (fullScreenQuadVS)->second);
    global.attachShader (shaders.compiled.find (globalLightFS)->second);
    
    point.attachShader (shaders.compiled.find (geometryVS)->second);
    point.attachShader (shaders.compiled.find (pointLightFS)->second);
    
    spot.attachShader (shaders.compiled.find (geometryVS)->second);
    spot.attachShader (shaders.compiled.find (spotlightFS)->second);

    // Check they link properly.
    if (!(geo.link() && global.link() && point.link() && spot.link()))
    {
        return false;
    }

    // We've successfully compiled each program.
    geometry    = std::move (geo);
    globalLight = std::move (global);
    pointLight  = std::move (point);
    spotlight   = std::move (spot);

    return true;
}


void Programs::clean() noexcept
{
    geometry.clean();
    globalLight.clean();
    pointLight.clean();
    spotlight.clean();
}