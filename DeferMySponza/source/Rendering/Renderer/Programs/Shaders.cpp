#include "Shaders.hpp"


// Personal headers.
#include <Rendering/Renderer/Programs/HardCodedShaders.hpp>


// Initialise the static variable.
const Shader Shaders::default = Shader { };


bool Shaders::initialise() noexcept
{
    // TODO: Load shaders from configuration file.
    return  compile (geometryVS,            GL_VERTEX_SHADER) &&
            compile (lightVolumeVS,         GL_VERTEX_SHADER) &&
            compile (forwardRenderFS,       GL_FRAGMENT_SHADER) &&
            compile (geometryFS,            GL_FRAGMENT_SHADER) &&
            compile (lightingPassFS,        GL_FRAGMENT_SHADER) &&
            compile (lightsFS,              GL_FRAGMENT_SHADER) &&
            compile (materialFetcherFS,     GL_FRAGMENT_SHADER) &&
            compile (reflectionModelsFS,    GL_FRAGMENT_SHADER);
}


bool Shaders::compile (const std::string& fileLocation, const GLenum type) noexcept
{
    // Make sure we haven't already compiled the file.
    if (isCompiled (fileLocation))
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


const Shader& Shaders::find (const std::string& fileLocation) const noexcept
{
    const auto iterator = compiled.find (fileLocation);

    if (iterator != std::end (compiled))
    {
        return iterator->second;
    }

    return default;
}