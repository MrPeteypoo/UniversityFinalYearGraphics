#pragma once

#if !defined    _RENDERING_OBJECTS_VERTEX_ARRAY_
#define         _RENDERING_OBJECTS_VERTEX_ARRAY_

// Engine headers.
#include <tgl/tgl.h>


// Forward declarations.
class Buffer;


/// <summary>
/// An RAII encapsulation of a OpenGL vertex array object. It stores references to the buffers containing vertex
/// attribute and element data, it can then be bound and used when drawing meshes.
/// </summary>
class VertexArray final
{
    public:

        /// <summary> Determines the attribute function called when formatting attributes. </summary>
        enum class AttributeLayout
        {
            Float32,
            Float64,
            Integer
        };

        VertexArray() noexcept                      = default;

        VertexArray (VertexArray&& move) noexcept;
        VertexArray& operator= (VertexArray&& move) noexcept;
        ~VertexArray() { clean(); }

        VertexArray (const VertexArray&)            = delete;
        VertexArray& operator= (const VertexArray&) = delete;


        /// <summary> Check if the vertex array has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_array != 0U; }
        
        /// <summary> Gets the OpenGL ID of vertex array object. </summary>
        inline GLuint getID() const noexcept        { return m_array; }


        /// <summary> 
        /// Attempt to initialise the vertex array object. Successive calls to this function will cause the stored
        /// array to be deleted and a fresh array will take its place. Upon failure the object will not be modified.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> Deletes the vertex array object. </summary>
        void clean() noexcept;


        /// <summary> Attaches a vertex buffer at the given binding index. </summary>
        /// <param name="buffer"> The buffer to be attached. </param>
        /// <param name="bufferIndex"> The index inside the vertex array object where the buffer should be bound. </param>
        /// <param name="offset"> The memory offset into the buffer to start at when reading. </param>
        /// <param name="stride"> How many bytes to increment by the find the next element when reading. </param>
        void attachVertexBuffer (const Buffer& buffer, GLuint bufferIndex, GLintptr offset, GLsizei stride) noexcept;

        /// <summary> Enables or disables the vertex attribute at the given inex. </summary>
        /// <param name="attributeIndex"> The index of the attribute to set the status of. </param>
        /// <param name="bufferIndex"> The binding index of the buffer where the attribute data is stored. </param>
        /// <param name="isEnabled"> Whether the attribute should be enabled or disabled. </param>
        void specifyAttributeStatus (GLuint attributeIndex, GLuint bufferIndex, bool isEnabled) noexcept;

        /// <summary> 
        /// Specifies the formatting for the given attribute. When given AttributeLayout::Float64, the GL_DOUBLE type
        /// must be used. When given AttributeLayout::Float32 and type is GL_DOUBLE the data will be cast to GL_FLOAT.
        /// </summary>
        /// <param name="attributeIndex"> The index of the attribute to specify the format of. </param>
        /// <param name="layout"> This effects which OpenGL formatting function is used. </param>
        /// <param name="size"> The number of components per vertex of the given type. </param>
        /// <param name="type"> The underlying data type, e.g. GL_FLOAT, GL_HALF_FLOAT. </param>
        /// <param name="relativeOffset"> The amount of bytes to the first element in the offsetted vertex buffer. </param>
        /// <param name="isNormalised"> Whether the data will be normalised or not. Only applies to Float32. </param>
        void specifyAttributeFormat (GLuint attributeIndex, AttributeLayout layout,
            GLint size, GLenum type, GLuint relativeOffset, GLboolean isNormalised = false) noexcept;

        /// <summary>
        /// Specify how many instances must be rendered before incrementing vertex attributes bound to the buffer at
        /// the given index.
        /// </summary>
        /// <param name="bufferIndex"> The binding index of the buffer to enable a divisor for. </param>
        /// <param name="divisor"> How many instances will use the same attribute element. </param>
        void specifyVertexBufferDivisor (GLuint bufferIndex, GLuint divisor) noexcept;

        /// <summary> Specifies the elemnt array buffer to use when drawing with the VAO bound. </summary>
        /// <param name="elementArrayBuffer"> The buffer containing vertex element data. </param>
        void specifyElementBuffer (const Buffer& elementArrayBuffer) noexcept;

    private:

        GLuint m_array { 0 }; //!< The OpenGL ID representing the vertex array object.
};

#endif // _RENDERING_OBJECTS_VERTEX_ARRAY_