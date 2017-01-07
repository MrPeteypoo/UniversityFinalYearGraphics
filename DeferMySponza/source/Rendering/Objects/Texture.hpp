#pragma once

#if !defined    _RENDERING_OBJECTS_TEXTURE_
#define         _RENDERING_OBJECTS_TEXTURE_

// Engine headers.
#include <tgl/tgl.h>


// Forward declarations.
template <GLenum target>
class TextureT;


// Aliases.
using Texture1DArray    = TextureT<GL_TEXTURE_1D_ARRAY>;
using Texture2D         = TextureT<GL_TEXTURE_2D>;
using TextureCubeMap    = TextureT<GL_TEXTURE_CUBE_MAP>;
using TextureRectangle  = TextureT<GL_TEXTURE_RECTANGLE>;
using Texture2DArray    = TextureT<GL_TEXTURE_2D_ARRAY>;
using TextureBuffer     = TextureT<GL_TEXTURE_BUFFER>;


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
        inline GLenum getDesiredTextureUnit() const noexcept    { return m_unit; }

        /// <sumamry> Sets the desired texture unit that the texture should be bound to. </summary>
        /// <param name="unit"> The index of the unit to bind to, the limit is at least 80. </param>
        void setDesiredTextureUnit (const GLuint unit) noexcept { m_unit = GL_TEXTURE0 + unit; }


        /// <summary> Deletes the texture, allowing it to be initialised again. </summary>
        void clean() noexcept;

    protected:

        GLuint m_texture { 0 }; //!< The OpenGL ID representing the texture object.
        GLenum m_unit    { 0 }; //!< The desired texture unit to bind the texture too.
};

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
        template <typename = std::enable_if_t<Target == GL_TEXTURE_2D_ARRAY || Target == GL_TEXTURE_3D || Target == GL_TEXTURE_CUBE_ARRAY>>
        void allocateImmutableStorage (GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, 
            GLsizei levels = 1) noexcept
        {
            glTextureStorage3D (m_texture, levels, internalFormat, width, height, depth);
        }

        /// <summary> 
        /// Allocates mutable storage and uploads data at the same time. Doing so after allocating immutable storage
        /// is an error and won't do anything. Binds the given texture to modify it.
        /// </summary>
        /// <param name="internalFormat"> Specifies the internal format of the memory, e.g. GL_RGB8. </param>
        /// <param name="width"> How many texels wide the texture should be. </param>
        /// <param name="height"> How many texels tall the texture should be. </param>
        /// <param name="pixelFormat"> The colour channel format of the given pixel data. E.g. GL_RGBA. </param>
        /// <param name="pixelType"> The underlying data type of the given pixel data. E.g. GL_FLOAT. </param>
        /// <param name="pixelData"> The data to upload to the allocated storage. </param>
        /// <param name="level"> The level of image to allocate memory for, 0 is the base image. </param>
        template <typename = std::enable_if_t<Target == GL_TEXTURE_1D_ARRAY || Target == GL_TEXTURE_2D || Target == GL_TEXTURE_RECTANGLE>>
        void allocateMutableStorage (GLenum internalFormat, GLsizei width, GLsizei height, 
            GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0) noexcept
        {
            const TextureBinder<Target> binder { *this };
            glTexImage2D (Target, level, internalFormat, width, height, 0, pixelFormat, pixelType, pixelData);
        }

        /// <summary>
        /// An override for cube maps, the desired face must be specified in the first template parameter. E.g. 
        /// util::allocateMutableStorage<GL_TEXTURE_CUBE_MAP_POSITIVE_X> (blah...). Binds the given texture to modify it. 
        /// </summary>
        template <GLenum CubeFace, typename = std::enable_if_t<Target == GL_TEXTURE_CUBE_MAP>>
        void allocateMutableStorage (GLenum internalFormat, GLsizei width, GLsizei height, 
            GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0) noexcept
        {
            static_assert (
                CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_X || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
                CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_Y || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
                CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_Z || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                "A cube face must be specified when using allocateMutableStorage for TextureCubeMap objects."
            );

            const TextureBinder<Target> binder { *this };
            glTexImage2D (CubeFace, level, internalFormat, width, height, 0, pixelFormat, pixelType, pixelData);
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