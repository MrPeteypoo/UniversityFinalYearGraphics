#include "Shaders.hpp"


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/PassConfigurator/HardCodedShaders.hpp>


bool Shaders::initialise() noexcept
{
    return  compileShader (sponzaVS, GL_VERTEX_SHADER) &&
            compileShader (sponzaFS, GL_FRAGMENT_SHADER);
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
    if (shader.compileFromFile (fileLocation, type))
    {
        compiled.emplace (fileLocation, std::move (shader));
        return true;
    }

    // Unsuccessful.
    return false;
}