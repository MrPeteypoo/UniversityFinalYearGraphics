#include "Shader.hpp"


// STL headers.
#include <array>
#include <iostream>
#include <utility>


// Engine headers.
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Utility/Algorithm.hpp>


Shader::Shader (Shader&& move) noexcept
{
    *this = std::move (move);
}


Shader& Shader::operator= (Shader&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak a shader.
        clean();

        m_shader    = move.m_shader;
        m_type      = move.m_type;
        m_source    = std::move (move.m_source);

        move.m_shader   = 0U;
        move.m_type     = 0U;
    }

    return *this;
}


bool Shader::initialise (const GLenum type) noexcept
{
    // Create a new shader.
    auto shaderID = glCreateShader (type);

    // Only use it if it's valid.
    if (shaderID == 0U)
    {
        return false;
    }

    // Delete any previously compiled shader.
    clean();

    // Success!
    m_shader    = shaderID;
    m_type      = type;
    
    return true;
}


void Shader::clean() noexcept
{
    if (isInitialised())
    {   
        glDeleteShader (m_shader);
        m_source.clear();
        m_shader    = 0U;
        m_type      = 0U;
    }
}


bool Shader::attachSource (const std::string& fileLocation) noexcept
{
    // Use the tygra library to read the entire file.
    auto string = tygra::createStringFromFile (fileLocation);
    
    // Ensure it's valid.
    if (string.empty())
    {
        return false;
    }

    m_source.emplace_back (std::move (string));
    return true;
}


bool Shader::attachSource (RawSource source) noexcept
{
    if (source.text.empty())
    { 
        return false;
    }
    
    m_source.emplace_back (std::move (source.text)); 
    return true;
}


bool Shader::compile() noexcept
{
    // Construct an array of strings for OpenGL to read from.
    auto strings = std::vector<const GLchar*> { };
    strings.reserve (m_source.size());

    std::for_each (m_source, [&] (std::string& string) { strings.push_back (string.c_str()); });

    // Attempt to compile the shader.
    glShaderSource (m_shader, static_cast<GLsizei> (strings.size()), strings.data(), nullptr);
    glCompileShader (m_shader);

    // Check whether compilation was successful.
    auto compileStatus = GLint { 0 };
    glGetShaderiv (m_shader, GL_COMPILE_STATUS, &compileStatus);
    
    if (compileStatus != GL_TRUE)
    {
        // Output error information.
        auto log = std::array<GLchar, 1024U> { };
        glGetShaderInfoLog (m_shader, static_cast<GLsizei> (log.size()), nullptr, log.data());

        std::cerr << log.data() << std::endl;
        return false;
    }

    return true;
}