#pragma once

#if !defined    _RENDERING_OBJECTS_PROGRAM_BINDER_
#define         _RENDERING_OBJECTS_PROGRAM_BINDER_

// Personal headers.
#include <Rendering/Objects/Program.hpp>


/// <summary>
/// A simple RAII utility to set a program for usage when rendering. The program will be unbound upon destruction.
/// </summary>
struct ProgramBinder final
{
    inline ProgramBinder() noexcept = default;
    
    inline ProgramBinder (const Program& program) noexcept
    {
        bind (program.getID());
    }

    inline ProgramBinder (const GLuint program) noexcept
    {
        bind (program);
    }

    inline ~ProgramBinder()
    {
        unbind();
    }

    static inline void bind (const Program& program) noexcept
    {
        glUseProgram (program.getID());
    }

    static inline void bind (const GLuint program) noexcept
    {
        glUseProgram (program);
    }

    static inline void unbind() noexcept
    {
        glUseProgram (0);
    }
};

#endif // _RENDERING_OBJECTS_PROGRAM_BINDER_