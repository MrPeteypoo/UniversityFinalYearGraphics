#pragma once

#if !defined    _RENDERING_SHADERS_
#define	        _RENDERING_SHADERS_


// STL headers.
#include <string>
#include <unordered_map>


// Personal headers.
#include <Rendering/PassConfigurator/Shader.hpp>


// Forward declarations.
using GLenum = unsigned int;


/// <summary>
/// Contains the OpenGL shaders required to construct OpenGL programs.
/// </summary>
struct Shaders final
{
    std::unordered_map<std::string, Shader> compiled {}; //!< A collection of successfully compiled shaders mapped by their filename.
        
    Shaders()                           = default;
    Shaders (Shaders&&)                 = default;
    Shaders& operator= (Shaders&&)      = default;
    ~Shaders()                          = default;

    Shaders (const Shaders&)            = delete;
    Shaders& operator= (const Shaders&) = delete;
    


    /// <summary> Checks whether the core programs have been loaded. </summary>
    inline bool isInitialised() const noexcept 
    { 
        return !compiled.empty();
    }


    /// <summary> 
    /// Initialise available shaders. This is currently loaded using hard coded filenames.
    /// </summary>
    /// <returns> Whether the initialisation was successful. </returns>
    bool initialise() noexcept;

    /// <summary> Discards and marks all shaders for deletion. They won't be deleted until detached from all programs. </summary>
    inline void clean() noexcept { compiled.clear(); }


    /// <summary> Attempts to compile a shader from the given file location, if successful it'll be added. </summary>
    /// <param name="fileLocation"> Where to look for the shader. </param>
    /// <param name="type"> The type of shader being compiled, e.g. GL_VERTEX_SHADER. </param>
    /// <returns> Whether the compilation was succesful or not. If it already existed then true will be returned. </returns>
    bool compileShader (const std::string& fileLocation, const GLenum type) noexcept;
};

#endif // _RENDERING_SHADERS_