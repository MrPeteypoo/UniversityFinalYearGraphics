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
        /// <param name="usePhysicallyBasedShader"> Determines how the reflection model shader is compiled. </param>
        /// <returns> Whether the initialisation was successful. </returns>
        bool initialise (const bool usePhysicallyBasedShaders) noexcept;

        /// <summary> Discards and marks all shaders for deletion. They won't be deleted until detached from all programs. </summary>
        inline void clean() noexcept { compiled.clear(); }


        /// <summary> Attempts to compile a shader from the given file location. Doesn't recompile a shader. </summary>
        /// <param name="type"> The type of shader being compiled, e.g. GL_VERTEX_SHADER. </param>
        /// <param name="mainSource"> Where to look for the main source file. </param>
        /// <param name="preProcessorSources"> Any extra source files to be used for the preprocessor. </param>
        /// <returns> Whether the compilation was succesful or not. </returns>
        template <typename Source, typename... Args>
        bool compile (const GLenum type, Source&& mainSource, Args&& ...preProcessorSources) noexcept;

        /// <summary> Attempts to find a compiled shader with at the given file location. </summary>
        /// <param name="fileLocation"> The file location of the compiled shader. </param>
        /// <returns> If the shader exists then the compiled shader, otherwise an uninitialised shader. </returns>
        const Shader& find (const std::string& fileLocation) const noexcept;

    private:
    
        using CompiledShaders = std::unordered_map<std::string, Shader>;

        const static Shader default;        //!< A default, uninitialised shader.
        CompiledShaders     compiled { };   //!< A collection of successfully compiled shaders mapped by their filename.

    private:

        /// <summary> Attaches each given source file location to the given shader. </summary>
        template <typename Source, typename ExtraSource, typename... Args>
        bool attachShaderSource (Shader& shader, Source&& mainSource, 
            ExtraSource&& extraSource, Args&& ...preProcessorSources) noexcept
        {
            return shader.attachSource (std::forward<ExtraSource> (extraSource)) ?
                attachShaderSource (shader, std::forward<Source> (mainSource), std::forward<Args> (preProcessorSources)...) :
                false;
        }

        /// <summary> Attaches each given source file location to the given shader. </summary>
        template <typename Source>
        bool attachShaderSource (Shader& shader, Source&& mainSource) noexcept
        {
            return shader.attachSource (std::forward<Source> (mainSource));
        }
};


// STL headers.
#include <iostream>


template <typename Source, typename... Args>
bool Shaders::compile (const GLenum type, Source&& mainSource, Args&& ...preProcessorSources) noexcept
{
    // Make sure we haven't already compiled the file.
    if (isCompiled (mainSource))
    {
        return true;
    }
    std::cout << "Compiling '" << mainSource << "'..." << std::endl;

    // Make sure we can actually create a shader.
    auto shader = Shader {};
    if (!shader.initialise (type))
    {
        return false;
    }

    // Attach all the specified source files
    if (!attachShaderSource (shader, std::forward<Source> (mainSource), std::forward<Args> (preProcessorSources)...))
    {
        return false;
    }

    // Finally ensure the shader compiles before adding it.
    if (!shader.compile())
    {
        return false;
    }

    // Success!
    compiled.emplace (mainSource, std::move (shader));
    return true;
}

#endif // _RENDERING_SHADERS_