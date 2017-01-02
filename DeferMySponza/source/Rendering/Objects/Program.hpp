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

        Program() noexcept                  = default;

        Program (Program&& move) noexcept;
        Program& operator= (Program&& move) noexcept;

        Program (const Program&)            = delete;
        Program& operator= (const Program&) = delete;

        ~Program() { clean(); }


        /// <summary> Check if the program has been successfully compiled. </summary>
        inline bool isInitialised() const noexcept  { return m_program != 0U; }
        
        /// <summary> Gets the OpenGL ID of the stored program. </summary>
        inline GLuint getID() const noexcept        { return m_program; }


        /// <summary> 
        /// Attempt to initialise the program. Successive calls will delete the old program and create a new one.
        /// Upon failure the object will be untouched and left in a clean state.
        /// </summary>
        /// <returns> Whether the program was successfully created or not. </returns>
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