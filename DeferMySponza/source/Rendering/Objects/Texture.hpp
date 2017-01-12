#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_
#define         _RENDERING_OBJECTS_TEXTURE_

// Engine headers.
#include <tgl/tgl.h>


// Forward declarations.
template <GLenum target>
class TextureT;


// Aliases.
using TextureBuffer         = TextureT<GL_TEXTURE_BUFFER>;
using Texture1D             = TextureT<GL_TEXTURE_1D>;
using Texture1DArray        = TextureT<GL_TEXTURE_1D_ARRAY>;
using Texture2D             = TextureT<GL_TEXTURE_2D>;
using TextureCubeMap        = TextureT<GL_TEXTURE_CUBE_MAP>;
using TextureRectangle      = TextureT<GL_TEXTURE_RECTANGLE>;
using Texture2DArray        = TextureT<GL_TEXTURE_2D_ARRAY>;
using Texture3D             = TextureT<GL_TEXTURE_3D>;
using TextureCubeMapArray   = TextureT<GL_TEXTURE_CUBE_MAP_ARRAY>;


/// <summary>
/// An RAII encapsulation of an OpenGL texture object. Textures contain images of the same format, it could represent 
/// a single image, an array of images or even plain shader-accessible data. This is a base class of specialised
/// texture types, as such it offers no functionality other than being an RAII encapsulation of a texture.
/// </summary>
class Texture
{
    public:

        Texture() noexcept                  = default;

        Texture (Texture&& move) noexcept;
        Texture& operator= (Texture&& move) noexcept;

        Texture (const Texture&)            = delete;
        Texture& operator= (const Texture&) = delete;

        ~Texture() { clean(); }


        /// <summary> Check if the texture has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept              { return m_texture != 0U; }
        
        /// <summary> Gets the OpenGL ID of texture object. </summary>
        inline GLuint getID() const noexcept                    { return m_texture; }

        /// <summary> Gets the enum representing the desired texture unit to bind the texture to. </summary>
        inline GLuint getDesiredTextureUnit() const noexcept    { return m_unit; }

        /// <sumamry> Sets the desired texture unit that the texture should be bound to. </summary>
        /// <param name="unit"> The index of the unit to bind to, the limit is at least 80. </param>
        void setDesiredTextureUnit (const GLuint unit) noexcept { m_unit = unit; }


        /// <summary> Deletes the texture, allowing it to be initialised again. </summary>
        void clean() noexcept;

    protected:

        GLuint m_texture { 0 }; //!< The OpenGL ID representing the texture object.
        GLuint m_unit    { 0 }; //!< The desired texture unit to bind the texture too.
};


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>


/// <summary>
/// A specialised version of the Texture class, the texture target is specified and as such the available functionality
/// will change to suit the actual texture object. For example, if the target is GL_TEXTURE_BUFFER a "setBuffer()"
/// function will be available, GL_TEXTURE_2D will have allocateImmutableStorage(), etc.
/// </summary>
template <GLenum Target>
class TextureT final : public Texture
{

    public:
        
        using Type = TextureT<Target>;
        constexpr static auto target = Target; //!< The target binding of the texture.

    public:

        TextureT() noexcept                             = default;
        TextureT (TextureT&& move) noexcept             = default;
        TextureT& operator= (TextureT&& move) noexcept  = default;
        ~TextureT()                                     = default;

        TextureT (const Texture&)                       = delete;
        TextureT& operator= (const Texture&)            = delete;


        /// <summary> 
        /// Attempt to initialise the texture object. Successive calls with delete the currently managed object.
        /// Upon failure the object will not be changed.
        /// </summary>
        /// <param name="unit"> The desired texture unit to bind the texture to. </param>
        /// <returns> Whether the texture was successfully created or not. </returns>
        bool initialise (const GLuint unit) noexcept;


        /// <summary> 
        /// Allocates immutable storage for the texture which can't be changed without reinitialising the
        /// texture. This is the recommended way of storing textures. The contents of the texture will be blank.
        /// </summary>
        /// <param name="internalFormat"> Specifies the internal format of the memory, e.g. GL_RGB8. </param>
        /// <param name="width"> How many texels wide the texture should be. </param>
        /// <param name="height"> How many texels tall the texture should be. </param>
        /// <param name="levels"> How many levels of images are required. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_CUBE_MAP || Target == GL_TEXTURE_RECTANGLE>>
        void allocateImmutableStorage (GLenum internalFormat, GLsizei width, GLsizei height, 
            GLsizei levels = 1) noexcept
        {
            glTextureStorage2D (m_texture, levels, internalFormat, width, height);
        }

        /// <summary> 
        /// Allocates immutable storage for the texture which can't be changed without reinitialising the
        /// texture. This is the recommended way of storing textures. The contents of the texture will be blank.
        /// </summary>
        /// <param name="internalFormat"> Specifies the internal format of the memory, e.g. GL_RGB8. </param>
        /// <param name="width"> How many texe;s wide the texture should be. </param>
        /// <param name="height"> How many texels tall the texture should be. </param>
        /// <param name="depth"> How many texels deep the texture should be. </param>
        /// <param name="levels"> How many levels of images are required. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_CUBE_MAP_ARRAY>>
        void allocateImmutableStorage (GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, 
            GLsizei levels = 1) noexcept
        {
            glTextureStorage3D (m_texture, levels, internalFormat, width, height, depth);
        }

        /// <summary> 
        /// Places data at the given location inside the texture. Bounds checking will not occur care must be taken
        /// when setting parameters. This overload is only applicable to 2D texture types.
        /// </summary>
        /// <param name="xOffset"> The number of texels to offset into the image on the X axis. </param>
        /// <param name="yOffset"> The number of texels to offset into the image on the Y axis. </param>
        /// <param name="width"> How many texels wide the data is. </param>
        /// <param name="height"> How many texels tall the data is. </param>
        /// <param name="pixelFormat"> The colour channel format of the given pixel data. E.g. GL_RGBA. </param>
        /// <param name="pixelType"> The underlying data type of the given pixel data. E.g. GL_FLOAT. </param>
        /// <param name="pixelData"> The data to upload to the allocated storage. </param>
        /// <param name="level"> The mipmap level of the image to set the data for, 0 is the base image. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_RECTANGLE>>
        void placeAt (GLint xOffset, GLint yOffset, GLsizei width, GLsizei height, 
            GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0) noexcept
        {
            glTextureSubImage2D (m_texture, level, xOffset, yOffset, width, height, pixelFormat, pixelType, pixelData);
        }

        /// <summary> 
        /// Places data at the given location inside the texture. Bounds checking will not occur care must be taken
        /// when setting parameters. This is only for 3D textures types.
        /// </summary>
        /// <param name="xOffset"> The number of texels to offset into the image on the X axis. </param>
        /// <param name="yOffset"> The number of texels to offset into the image on the Y axis. </param>
        /// <param name="zOffset"> The number of texels to offset into the image on the Z axis. </param>
        /// <param name="width"> How many texels wide the data is. </param>
        /// <param name="height"> How many texels tall the data is. </param>
        /// <param name="depth"> How many texels deep the data is. </param>
        /// <param name="pixelFormat"> The colour channel format of the given pixel data. E.g. GL_RGBA. </param>
        /// <param name="pixelType"> The underlying data type of the given pixel data. E.g. GL_FLOAT. </param>
        /// <param name="pixelData"> The data to upload to the allocated storage. </param>
        /// <param name="level"> The mipmap level of the image to set the data for, 0 is the base image. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_CUBE_MAP_ARRAY>>
        void placeAt (GLint xOffset, GLint yOffset, GLint zOffset, GLsizei width, GLsizei height, GLsizei depth,
            GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0) noexcept
        {
            glTextureSubImage3D (m_texture, level, xOffset, yOffset, zOffset,
                width, height, depth, pixelFormat, pixelType, pixelData);
        }

        /// <summary> Tells OpenGL to generate mipmaps based on the data currently stored by the texture. </summary>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_CUBE_MAP || Target == GL_TEXTURE_CUBE_MAP_ARRAY>>
        void generateMipmap() noexcept
        {
            glGenerateTextureMipmap (m_texture);
        }

        /// <summary> Attaches the entirety of a buffer as the texture objects data store. </summary>
        /// <param name="buffer"> The buffer to be attached to the texture. </param>
        /// <param name="internalFormat"> The format of the data in the buffer, e.g. GL_RGBA32F. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_BUFFER>>
        void setBuffer (const Buffer& buffer, const GLenum internalFormat) noexcept
        {
            glTextureBuffer (m_texture, internalFormat, buffer.getID());
        }

        /// <summary> Attaches a subset of a buffer as the texture objects data store. </summary>
        /// <param name="buffer"> The buffer to be attached to the texture. </param>
        /// <param name="internalFormat"> The format of the data in the buffer, e.g. GL_RGBA32F. </param>
        /// <param name="offset"> The start of the range of the buffers data store to attach. </param>
        /// <param name="size"> How many bytes, after the offset, should the texture have access to. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_BUFFER>>
        void setBuffer (const Buffer& buffer, const GLenum internalFormat, 
            const GLintptr offset, const GLsizeiptr size) noexcept
        {
            glTextureBufferRange (m_texture, internalFormat, buffer.getID(), offset, size);
        }

        /// <summary> Sets the given texture parameter to the given value. </summary>
        /// <param name="name"> The name of the parameter to set, e.g. GL_TEXTURE_MIN_FILTER. </param>
        /// <param name="value"> The value to set the parameter to. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_RECTANGLE || Target == GL_TEXTURE_CUBE_MAP>>
        void setParameter (const GLenum name, const GLfloat value) noexcept
        {
            glTextureParameterf (m_texture, name, value);
        }

        /// <summary> Sets the given texture parameter to the given value. </summary>
        /// <param name="name"> The name of the parameter to set, e.g. GL_TEXTURE_MIN_FILTER. </param>
        /// <param name="value"> The value to set the parameter to. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_RECTANGLE || Target == GL_TEXTURE_CUBE_MAP>>
        void setParameter (const GLenum name, const GLint value) noexcept
        {
            glTextureParameteri (m_texture, name, value);
        }

        /// <summary> Sets the given texture parameter to the given value. </summary>
        /// <param name="name"> The name of the parameter to set, e.g. GL_TEXTURE_MIN_FILTER. </param>
        /// <param name="values"> The values to set the parameter to. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_RECTANGLE || Target == GL_TEXTURE_CUBE_MAP>>
        void setParameter (const GLenum name, const GLfloat* values) noexcept
        {
            glTextureParameterfv (m_texture, name, values);
        }

        /// <summary> Sets the given texture parameter to the given value. </summary>
        /// <param name="name"> The name of the parameter to set, e.g. GL_TEXTURE_MIN_FILTER. </param>
        /// <param name="values"> The values to set the parameter to. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_RECTANGLE || Target == GL_TEXTURE_CUBE_MAP>>
        void setParameter (const GLenum name, const GLint* values) noexcept
        {
            glTextureParameteriv (m_texture, name, values);
        }
};


template <GLenum Target>
bool TextureT<Target>::initialise (const GLuint unit) noexcept
{
    // Generate an object.
    auto texture = GLuint { 0 };
    glCreateTextures (Target, 1, &texture);

    // Check the validity before using it.
    if (texture == 0U)
    {
        return false;
    }

    // Ensure we don't leak.
    clean();
    m_texture = texture;
    setDesiredTextureUnit (unit);

    return true;
}


#endif // _RENDERING_OBJECTS_TEXTURE_