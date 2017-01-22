#pragma once

#if !defined    _RENDERING_SHADERS_
#define	        _RENDERING_SHADERS_

// STL headers.
#include <string>
#include <unordered_map>


// Personal headers.
#include <Rendering/Objects/Shader.hpp>


/// <summary>
/// Contains the OpenGL shaders required to construct OpenGL programs.
/// </summary>
class Shaders final
{
    public:
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

        /// <summary> Checks whether the shader at the given location has been compiled. </summary>
        inline bool isCompiled (const std::string& fileLocation) const noexcept 
        { 
            return compiled.find (fileLocation) != std::end (compiled); 
        }


        /// <summary> 
        /// Initialise available shaders. This is currently loaded using hard coded filenames.
        /// </summary>
        /// <returns> Whether the initialisation was successful. </returns>
        bool initialise() noexcept;

        /// <summary> Discards and marks all shaders for deletion. They won't be deleted until detached from all programs. </summary>
        inline void clean() noexcept { compiled.clear(); }


        /// <summary> Attempts to compile a shader from the given file location. Doesn't recompile a shader. </summary>
        /// <param name="fileLocation"> Where to look for the shader. </param>
        /// <param name="type"> The type of shader being compiled, e.g. GL_VERTEX_SHADER. </param>
        /// <returns> Whether the compilation was succesful or not. </returns>
        bool compile (const std::string& fileLocation, const GLenum type) noexcept;

        /// <summary> Attempts to find a compiled shader with at the given file location. </summary>
        /// <param name="fileLocation"> The file location of the compiled shader. </param>
        /// <returns> If the shader exists then the compiled shader, otherwise an uninitialised shader. </returns>
        const Shader& find (const std::string& fileLocation) const noexcept;

    private:
    
        using CompiledShaders = std::unordered_map<std::string, Shader>;

        const static Shader default;        //!< A default, uninitialised shader.
        CompiledShaders     compiled { };   //!< A collection of successfully compiled shaders mapped by their filename.
};

#endif // _RENDERING_SHADERS_