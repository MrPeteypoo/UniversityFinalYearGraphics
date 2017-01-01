#pragma once

#if !defined    _UTILITY_OPENGL_TEXTURES_
#define         _UTILITY_OPENGL_TEXTURES_

// Personal headers.
#include <Rendering/Binders/TextureBinder.hpp>
#include <Utility/TypeTraits.hpp>


namespace util
{
    /// <summary> 
    /// Allocates immutable storage for the texture which can't be changed without reinitialising the
    /// texture. This is the recommended way of storing textures. The contents of the texture will be blank.
    /// </summary>
    /// <param name="texture"> The texture to allocate immutable storage for. Must be valid for glTexStorage2D. </param>
    /// <param name="internalFormat"> Specifies the internal format of the memory, e.g. GL_RGB8. </param>
    /// <param name="width"> How many pixels wide the texture should be. </param>
    /// <param name="height"> How many pixels tall the texture should be. </param>
    /// <param name="levels"> How many levels of images are required. </param>
    template <typename Tex, typename = std::enable_if_t<is_any_v<Tex, Texture1DArray, Texture2D, TextureCubeMap, TextureRectangle>, Tex>>
    void allocateImmutableStorage (Tex& texture, GLenum internalFormat, 
        GLsizei width, GLsizei height, GLsizei levels = 1)
    {
        const TextureBinder<Tex::target> binder { texture };
        glTexStorage2D (Tex::target, levels, internalFormat, width, height);
    }

    /// <summary> 
    /// Allocates mutable storage and uploads data at the same time. Doing so after allocating immutable storage
    /// is an error and won't do anything.
    /// </summary>
    /// <param name="internalFormat"> Specifies the internal format of the memory, e.g. GL_RGB8. </param>
    /// <param name="width"> How many pixels wide the texture should be. </param>
    /// <param name="height"> How many pixels tall the texture should be. </param>
    /// <param name="pixelFormat"> The colour channel format of the given pixel data. E.g. GL_RGBA. </param>
    /// <param name="pixelType"> The underlying data type of the given pixel data. E.g. GL_FLOAT. </param>
    /// <param name="pixelData"> The data to upload to the allocated storage. </param>
    /// <param name="level"> The level of image to allocate memory for, 0 is the base image. </param>
    template <typename Tex, typename = std::enable_if_t<is_any_v<Tex, Texture1DArray, Texture2D, TextureRectangle>, Tex>>
    void allocateMutableStorage (Tex& texture, GLenum internalFormat, GLsizei width, GLsizei height, 
        GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0)
    {
        const TextureBinder<Tex::target> binder { texture };
        glTexImage2D (Tex::target, level, internalFormat, width, height, 0, pixelFormat, pixelType, pixelData);
    }

    /// <summary>
    /// An override for cube maps, the desired face must be specified in the first template parameter. 
    /// E.g. util::allocateMutableStorage<GL_TEXTURE_CUBE_MAP_POSITIVE_X> (blah...).
    /// </summary>
    template <GLenum CubeFace, typename Tex, typename = std::enable_if_t<std::is_same<Tex, TextureCubeMap>::value, Tex>>
    void allocateMutableStorage (Tex& texture, GLenum internalFormat, GLsizei width, GLsizei height, 
        GLenum pixelFormat, GLenum pixelType, const GLvoid* pixelData = nullptr, GLsizei level = 0)
    {
        static_assert (
            CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_X || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_X ||
            CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_Y || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_Y ||
            CubeFace == GL_TEXTURE_CUBE_MAP_POSITIVE_Z || CubeFace == GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            "A cube face must be specified when using allocateMutableStorage for TextureCubeMap objects."
        );

        const TextureBinder<Tex::target> binder { texture };
        glTexImage2D (CubeFace, level, internalFormat, width, height, 0, pixelFormat, pixelType, pixelData);
    }
}

#endif // _UTILITY_OPENGL_TEXTURES_