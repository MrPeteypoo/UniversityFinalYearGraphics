#include "Program.hpp"


// STL headers.
#include <iostream>
#include <utility>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/Objects/Shader.hpp>


Program::Program (Program&& move) noexcept
{
    *this = std::move (move);
}


Program& Program::operator= (Program&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak.
        if (isInitialised())
        {
            clean();
        }

        m_program       = move.m_program;
        move.m_program  = 0U;
    }

    return *this;
}


void Program::clean() noexcept
{
    glDeleteProgram (m_program);
    m_program = 0U;
}


bool Program::initialise() noexcept
{
    if (isInitialised())
    {
        clean();
    }
    
    m_program = glCreateProgram();

    // Non-zero values indicate glCreateProgram succeeded.
    return m_program != 0U;
}


void Program::attachShader (const Shader& shader) const noexcept
{
    if (shader.isInitialised())
    {
        glAttachShader (m_program, shader.getID());
    }
}


bool Program::link() const noexcept
{
    // Attempt to link the program.
    glLinkProgram (m_program);

    // Test the status for any errors.
    auto linkStatus = GLint { 0 };
    glGetProgramiv (m_program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus != GL_TRUE) 
    {
        // Output error information.
        const auto stringLength = 1024U;
        GLchar log[stringLength];

        glGetProgramInfoLog (m_program, stringLength, NULL, log);
        std::cerr << log << std::endl;

        return false;
    }

    return true;
}
