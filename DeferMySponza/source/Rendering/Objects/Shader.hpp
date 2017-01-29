#pragma once

#if !defined    _RENDERING_SHADER_
#define         _RENDERING_SHADER_

// STL headers.
#include <string>
#include <vector>


// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Encapsulates an OpenGL shader, allows for easy construction and destruction.
/// </summary>
class Shader final
{
    public:

        Shader() noexcept                   = default;

        Shader (Shader&&) noexcept;
        Shader& operator= (Shader&&) noexcept;

        Shader (const Shader&)              = delete;
        Shader& operator= (const Shader&)   = delete;

        ~Shader() { clean(); }
        

        /// <summary> Check if the shader has been successfully compiled. </summary>
        inline bool isInitialised() const noexcept  { return m_shader != 0U; }
        
        /// <summary> Gets the OpenGL ID of the stored shader. </summary>
        inline GLuint getID() const noexcept        { return m_shader; }

        /// <summary> Gets the GLenum representing the type of the shader. </summary>
        inline GLenum getType() const noexcept      { return m_type; }


        /// <summary>
        /// Creates a blank, uncompiled shader. This will replace the currently stored shader unless initialisation
        /// fails. The currently stored shader will be flagged for deletion but this may not occur until it has been
        /// detached from all programs.
        /// </summary>
        /// <param name="type"> Describes the type of shader, e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER. </param>
        /// <returns> Whether the shader could be initialised. </returns>
        bool initialise (const GLenum type) noexcept;

        /// <summary> Flags the shader for deletion and restores this object to a clean state. </summary>
        void clean() noexcept;

        /// <summary> Attempts to attach the file at the given file location as source code. </summary>
        /// <param name="fileLocation"> The file location of the source code. </param>
        /// <returns> Whether the file at the given location could be read. </returns>
        bool attachSourceFile (const std::string& fileLocation) noexcept;

        /// <summary> Compile the shader as the given shader type with the attached source code files. </summary>
        /// <returns> Whether the shader was successfully compiled. </returns>
        bool compile() noexcept;

    private:

        using Source = std::vector<std::string>;

        GLuint m_shader { 0 };  //!< The OpenGL ID of a shader, 0 means null.
        GLenum m_type   { 0 };  //!< Represents the type of shader, e.g. GL_VERTEX_SHADER.
        Source m_source { };    //!< Contains each source file that will be used to compile the shader.
};

#endif // _RENDERING_SHADER_