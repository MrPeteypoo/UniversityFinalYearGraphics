#pragma once

#if !defined    _RENDERING_PROGRAMS_
#define	        _RENDERING_PROGRAMS_

// Personal headers.
#include <Rendering/Objects/Program.hpp>


// Forward declarations.
class Shaders;


/// <summary>
/// Contains the OpenGL programs required to perform rendering passes.
/// </summary>
struct Programs final
{
    constexpr static auto globalLightSubroutine = GLuint { 0 }; //!< The subroutine index for the lighting pass programs to apply global lighting.
    constexpr static auto pointLightSubroutine  = GLuint { 1 }; //!< The subroutine index for the lighting pass programs to apply point lighting.
    constexpr static auto spotlightSubroutine   = GLuint { 2 }; //!< The subroutine index for the lighting pass programs to apply spotlighting.

    Program shadowMapPass   { };    //!< A depth-pass used for shadow mapping.
    Program geometryPass    { };    //!< Basic shaders which construct the scene with ambient lighting.
    Program globalLightPass { };    //!< Provides a global light pass with an oversized triangle.
    Program lightingPass    { };    //!< Point and spotlight passes based on a subroutine.
    Program forwardRender   { };    //!< Peforms forward rendering, every fragment will determine the contribution of every light.
    

    Programs() noexcept                         = default;
    Programs (Programs&&) noexcept              = default;
    Programs& operator= (Programs&&) noexcept   = default;
    ~Programs()                                 = default;

    Programs (const Programs&)                  = delete;
    Programs& operator= (const Programs&)       = delete;


    /// <summary> Checks whether the core programs have been loaded. </summary>
    inline bool isInitialised() const noexcept 
    { 
        return geometryPass.isInitialised() && globalLightPass.isInitialised() && 
            lightingPass.isInitialised() && forwardRender.isInitialised();
    }


    /// <summary> 
    /// Initialise the core programs using the shaders provided. This is currently loaded using hard coded value.
    /// </summary>
    /// <param name="shaders"> The collection of shaders to attach and link to. </param>
    /// <returns> Whether the initialisation was successful. </returns>
    bool initialise (const Shaders& shaders) noexcept;

    /// <summary> Detaches all shaders and deletes each program. </summary>
    void clean() noexcept;


    /// <summary> 
    /// Sets the index of the subroutine in the currently active program. This assumes the active program only has one
    /// subroutine. 
    /// </summary>
    /// <param name="shaderType"> The shader to set the subroutine for. </param>
    /// <param name="index"> The index, as specified in a shader, of the subroutine method to use. </param>
    static void setActiveProgramSubroutine (const GLenum shaderType, const GLuint index) noexcept
    {
        glUniformSubroutinesuiv (shaderType, 1, &index);
    }


    template <typename Func>
    void performActionOnPrograms (const Func& func) const noexcept
    {
        func (shadowMapPass);
        func (geometryPass);
        func (globalLightPass);
        func (lightingPass);
        func (forwardRender);
    }

    template <typename Func>
    void performActionOnPrograms (const Func& func) noexcept
    {
        func (shadowMapPass);
        func (geometryPass);
        func (globalLightPass);
        func (lightingPass);
        func (forwardRender);
    }
};

#endif // _RENDERING_PROGRAMS_