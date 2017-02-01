#include "Shaders.hpp"


// STL headers.
#include <iostream>


// Personal headers.
#include <Rendering/Renderer/Programs/HardCodedShaders.hpp>


// Initialise the static variable.
const Shader Shaders::default = Shader { };


bool Shaders::initialise (const bool usePhysicallyBasedShaders) noexcept
{
    // TODO: Load shaders from configuration file.
    bool success = true;
    const auto compileShader = [&] (const auto shaderType, const auto& main, auto&&... strings)
    {
        success = compile (shaderType, main, std::forward<decltype (strings)> (strings)...) && success;
    };
    
    compileShader (GL_VERTEX_SHADER, geometryVS);
    compileShader (GL_VERTEX_SHADER, fullScreenTriangleVS);
    compileShader (GL_VERTEX_SHADER, lightVolumeVS);
    
    compileShader (GL_FRAGMENT_SHADER, forwardRenderFS);
    compileShader (GL_FRAGMENT_SHADER, geometryFS);
    compileShader (GL_FRAGMENT_SHADER, lightingPassFS);
    compileShader (GL_FRAGMENT_SHADER, lightsFS);
    compileShader (GL_FRAGMENT_SHADER, materialFetcherFS);
    
    if (usePhysicallyBasedShaders)
    {
        compileShader (GL_FRAGMENT_SHADER, reflectionModelsFS, pbsDefines);
    }

    else
    {
        compileShader (GL_FRAGMENT_SHADER, reflectionModelsFS);
    }

    return success;
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