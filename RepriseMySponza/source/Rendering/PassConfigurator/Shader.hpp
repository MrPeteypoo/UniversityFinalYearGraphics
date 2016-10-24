#pragma once

#if !defined    _RENDERING_SHADER_
#define         _RENDERING_SHADER_


// STL headers.
#include <string>


// Aliases.
using GLenum = unsigned int;
using GLuint = unsigned int;


/// <summary>
/// Encapsulates an OpenGL shader, allows for easy construction and destruction.
/// </summary>
class Shader final
{
    public:

        Shader() noexcept = default;

        Shader (Shader&&) noexcept;
        Shader& operator= (Shader&&) noexcept;

        Shader (const Shader&)              = delete;
        Shader& operator= (const Shader&)   = delete;

        ~Shader() { if (isInitialised()) clean(); }
        

        /// <summary> Check if the Shader has been successfully compiled. </summary>
        inline bool isInitialised() const noexcept  { return m_shader != 0; }
        
        /// <summary> Gets the OpenGL ID of the stored shader. </summary>
        inline GLuint getID() const noexcept        { return m_shader; }

        /// <summary> Gets the GLenum representing the type of the shader. </summary>
        inline GLenum getType() const noexcept      { return m_type; }


        /// <summary>
        /// Compile the shader from the given file locations. If called on a previously compiled shader, the old shader
        /// will be marked for deletion but deletion cannot occur until it has been detached from all programs.
        /// </summary>
        /// <param name="file"> The file location of the shader to be compiled. </param>
        /// <param name="type"> Describes the type of shader, e.g. GL_VERTEX_SHADER, GL_FRAGMENT_SHADER. </param>
        /// <returns> The result of the compilation. </returns>
        bool compileFromFile (const std::string& file, const GLenum type) noexcept;

        /// <summary> Detaches all shaders and deletes each program. </summary>
        void clean() noexcept;

    private:

        GLuint m_shader { 0 };  //!< The OpenGL ID of a shader, 0 means null.
        GLenum m_type   { 0 };  //!< Represents the type of shader, e.g. GL_VERTEX_SHADER.
};

#endif // _RENDERING_SHADER_