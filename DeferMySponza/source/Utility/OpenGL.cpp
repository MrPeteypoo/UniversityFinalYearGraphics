#include "OpenGL.hpp"


// STL headers.
#include <iostream>


// Engine headers.
#include <glm/vec4.hpp>
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <MyView/Internals/Material.hpp>


namespace util
{    
    // Instantiate the different required templates to avoid including OpenGL in the header.
    template void fillBuffer (GLuint& vbo, const std::vector<Material>& data, const GLenum target, const GLenum usage);


    void allocateBuffer (GLuint& buffer, const size_t size, const GLenum target, const GLenum usage)
    {
        if (buffer == 0U)
        {
            glGenBuffers (1, &buffer);
        }

        glBindBuffer (target, buffer);
        glBufferData (target, size, nullptr, usage);
        glBindBuffer (target, 0);
    }


    template <typename T> void fillBuffer (GLuint& buffer, const std::vector<T>& data, const GLenum target, const GLenum usage)
    {
        if (buffer == 0)
        {
            glGenBuffers (1, &buffer);
        }

        glBindBuffer (target, buffer);
        glBufferData (target, data.size() * sizeof (T), data.data(), usage);
        glBindBuffer (target, 0);
    }


    void createMatrix4Attribute (const int attribLocation, const GLsizei stride, const int extraOffset, const int divisor)
    {
        // Pre-condition: A valid attribute location has been given.
        if (attribLocation >= 0)
        {
            // We need to go through each column of the matrices creating attribute pointers.
            const auto matrixColumns = GLint { 4 };
            for (GLint i { 0 }; i < matrixColumns; ++i)
            {
                const int current { attribLocation + i };

                // Enable each column and set the divisor.
                glEnableVertexAttribArray (current);
                glVertexAttribDivisor (current, divisor);

                // Calculate the offsets for each column.
                const auto offset = TGL_BUFFER_OFFSET (sizeof (glm::vec4) * i + extraOffset);

                // Create the columns attribute pointer.
                glVertexAttribPointer (current, 4, GL_FLOAT, GL_FALSE, stride, offset);
            }
        }
    }


    void generateTexture2D (GLuint& textureBuffer, const std::string& fileLocation)
    {
        // Attempt to load the image.
        const auto image = tygra::createImageFromPngFile (fileLocation);

        if (image.doesContainData()) 
        {
            // Start by preparing the texture buffer.
            if (textureBuffer == 0U)
            {
                glGenTextures (1, &textureBuffer);
            }

            glBindTexture (GL_TEXTURE_2D, textureBuffer);

            // Enable standard filters.
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,      GL_REPEAT);
            glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,      GL_REPEAT);

            // Enable each different pixel format.
            GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };

            // Load the texture into OpenGL.
            glTexImage2D (  GL_TEXTURE_2D, 0, GL_RGBA,          
                      
                            // Dimensions and border.
                            static_cast<GLsizei> (image.width()), static_cast<GLsizei> (image.height()), 0,   
                      
                            // Format and type.
                            pixel_formats[image.componentsPerPixel()], image.bytesPerComponent() == 1U ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
                      
                            // Data.
                            image.pixelData());

            // Generate the mipmaps from the loaded texture and finish.
            glGenerateMipmap (GL_TEXTURE_2D);
            glBindTexture (GL_TEXTURE_2D, 0);
        }
    }
}