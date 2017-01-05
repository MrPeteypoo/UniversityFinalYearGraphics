#pragma once

#if !defined    _RENDERING_PROGRAMS_
#define	        _RENDERING_PROGRAMS_

// Personal headers.
#include <Rendering/Objects/Program.hpp>


// Forward declarations.
struct Shaders;


/// <summary>
/// Contains the OpenGL programs required to perform rendering passes.
/// </summary>
struct Programs final
{

    Program geometry    {}; //!< Basic shaders which construct the scene with ambient lighting.
    Program globalLight {}; //!< Provides directional light shading.
    Program pointLight  {}; //!< Provides point light shading.
    Program spotlight   {}; //!< Provides spot light shading.
        

    Programs() noexcept                         = default;
    Programs (Programs&&) noexcept              = default;
    Programs& operator= (Programs&&) noexcept   = default;

    Programs (const Programs&)                  = delete;
    Programs& operator= (const Programs&)       = delete;

    ~Programs()                                 = default;


    /// <summary> Checks whether the core programs have been loaded. </summary>
    inline bool isInitialised() const noexcept 
    { 
        return geometry.isInitialised() || globalLight.isInitialised() || 
            pointLight.isInitialised() || spotlight.isInitialised();
    }


    /// <summary> 
    /// Initialise the core programs using the shaders provided. This is currently loaded using hard coded value.
    /// </summary>
    /// <param name="shaders"> The collection of shaders to attach and link to. </param>
    /// <returns> Whether the initialisation was successful. </returns>
    bool initialise (const Shaders& shaders) noexcept;

    /// <summary> Detaches all shaders and deletes each program. </summary>
    void clean() noexcept;

};

#endif // _RENDERING_PROGRAMS_