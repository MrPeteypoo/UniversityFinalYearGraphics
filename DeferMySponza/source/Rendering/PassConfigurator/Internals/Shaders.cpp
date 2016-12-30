#include "Shaders.hpp"


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/Internals/HardCodedShaders.hpp>


bool Shaders::initialise() noexcept
{
    // TODO: Load shaders from configuration file.
    return  compileShader (geometryVS, GL_VERTEX_SHADER) &&
            compileShader (geometryFS, GL_FRAGMENT_SHADER) &&
            compileShader (directionalLightFS, GL_FRAGMENT_SHADER) &&
            compileShader (pointLightFS, GL_FRAGMENT_SHADER) &&
            compileShader (spotlightFS, GL_FRAGMENT_SHADER);
}


bool Shaders::compileShader (const std::string& fileLocation, const GLenum type) noexcept
{
    // Make sure we haven't already compiled the file.
    if (compiled.find (fileLocation) != end (compiled))
    {
        return true;
    }

    // Make sure the shader compiles before adding it to the map.
    auto shader = Shader {};
    if (shader.initialise (fileLocation, type))
    {
        compiled.emplace (fileLocation, std::move (shader));
        return true;
    }

    // Unsuccessful.
    return false;
}