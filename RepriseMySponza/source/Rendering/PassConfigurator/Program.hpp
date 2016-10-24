#pragma once

#if !defined    _RENDERING_PROGRAM_
#define         _RENDERING_PROGRAM_


// Forward declarations.
using GLuint = unsigned int;
class Shader;


/// <summary>
/// Contains an OpenGL program required to perform rendering passes.
/// </summary>
class Program final
{
    public:

        Program() noexcept = default;
        Program (Program&& move) noexcept;
        Program& operator= (Program&& move) noexcept;
        ~Program() { if (isInitialised()) clean(); }

        Program (const Program&)            = delete;
        Program& operator= (const Program&) = delete;


        /// <summary> Check if the Shader has been successfully compiled. </summary>
        inline bool isInitialised() const noexcept  { return m_program != 0; }
        
        /// <summary> Gets the OpenGL ID of the stored shader. </summary>
        inline GLuint getID() const noexcept        { return m_program; }


        /// <summary> Attempt to initialise the program. </summary>
        /// <returns> Whether the program was successful or not. </returns>
        bool initialise() noexcept;

        /// <summary> Detaches all shaders and deletes each program. </summary>
        void clean() noexcept;

        /// <summary> Attaches the given shader to the program. </summary>
        void attachShader (const Shader& shader) const noexcept;

        /// <summary> Attempts to link the program, created a fully compiled program. </summary>
        /// <returns> Whether the program could be linked. </returns>
        bool link() const noexcept;

    private:

        GLuint m_program { 0 }; //!< The OpenGL ID representing a program. 0 means null.
};

#endif // _RENDERING_PROGRAM_