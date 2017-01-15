#pragma once

#if !defined    _RENDERING_OBJECTS_VERTEX_ARRAY_
#define         _RENDERING_OBJECTS_VERTEX_ARRAY_

// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/Composites/PersistentMappedBuffer.hpp>


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
        /// <param name="divisor"> If applicable, specifies how many instances will use the same data. </param>
        void attachVertexBuffer (const Buffer& buffer, GLuint bufferIndex, 
            GLintptr offset, GLsizei stride, GLuint divisor = 0) noexcept;

        /// <summary> 
        /// Attaches every partition contained in a persistently mapped buffer, partitions attached in sequence
        /// starting from the initial index given.
        /// </summary>
        /// <param name="buffer"> The buffer to be attached. </param>
        /// <param name="initialBufferIndex"> The starting index to bind each partition to. </param>
        /// <param name="stride"> The strides to be applied to each partition. </param>
        /// <param name="divisor"> If applicable, specifies how many instances will use the same data. </param>
        template <size_t Partitions>
        void attachPersistentMappedBuffer (const PersistentMappedBuffer<Partitions>& buffer, GLuint initialBufferIndex, 
            GLsizei stride, GLuint divisor = 0) noexcept;

        /// <summary> Enables or disables the vertex attribute at the given index. </summary>
        /// <param name="attributeIndex"> The index of the attribute to set the status of. </param>
        /// <param name="isEnabled"> Whether the attribute should be enabled or disabled. </param>
        void setAttributeStatus (GLuint attributeIndex, bool isEnabled) noexcept;

        /// <summary> Enables or disables a number of vertex attributes starting from the given index. </summary>
        /// <param name="startingIndex"> The initial attribute index to begin formatting from. </param>
        /// <param name="count"> How many attributes should have their status set. </param>
        /// <param name="isEnabled"> Whether the attribute should be enabled or disabled. </param>
        void setAttributeStatus (GLuint startingIndex, GLuint count, bool isEnabled) noexcept;
        
        /// <summary> Specifies the binding index of the buffer containing data for the given attribute. </summary>
        /// <param name="attributeIndex"> The index of the attribute to set the status of. </param>
        /// <param name="bufferIndex"> The binding index of the buffer where the attribute data is stored. </param>
        void setAttributeBufferBinding (GLuint attributeIndex, GLuint bufferIndex) noexcept;
        
        /// <summary> 
        /// Specifies the binding index of the buffer containing data for a number of attributes starting at the 
        /// given index. 
        /// </summary>
        /// <param name="startingIndex"> The initial attribute index to begin formatting from. </param>
        /// <param name="count"> How many attributes to set thhe buffer binding for. </param>
        /// <param name="bufferIndex"> The binding index of the buffer where the attribute data is stored. </param>
        void setAttributeBufferBinding (GLuint startingIndex, GLuint count, GLuint bufferIndex) noexcept;

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
        void setAttributeFormat (GLuint attributeIndex, AttributeLayout layout,
            GLint size, GLenum type, GLuint relativeOffset, GLboolean isNormalised = GL_FALSE) noexcept;

        /// <summary> 
        /// Specifies the formatting for a number of attributes starting at the given index. Relative offset will be
        /// set for all attributes based on the given offset-per-attribute value.
        /// </summary>
        /// <param name="startingIndex"> The initial attribute index to begin formatting from. </param>
        /// <param name="layout"> This effects which OpenGL formatting function is used. </param>
        /// <param name="size"> The number of components per vertex of the given type. </param>
        /// <param name="type"> The underlying data type, e.g. GL_FLOAT, GL_HALF_FLOAT. </param>
        /// <param name="relativeOffset"> The amount of bytes to the first element in the offsetted vertex buffer. </param>
        /// <param name="isNormalised"> Whether the data will be normalised or not. Only applies to Float32. </param>
        void setAttributeFormat (GLuint startingIndex, GLuint count, GLuint offsetPerAttribute,
            AttributeLayout layout, GLint size, GLenum type, GLuint relativeOffset, 
            GLboolean isNormalised = GL_FALSE) noexcept;

        /// <summary> Specifies the elemnt array buffer to use when drawing with the VAO bound. </summary>
        /// <param name="elementArrayBuffer"> The buffer containing vertex element data. </param>
        void setElementBuffer (const Buffer& elementArrayBuffer) noexcept;

    private:

        GLuint m_array { 0 }; //!< The OpenGL ID representing the vertex array object.
};


template <size_t Partitions>
void VertexArray::attachPersistentMappedBuffer (const PersistentMappedBuffer<Partitions>& buffer, GLuint initialBufferIndex, 
    GLsizei stride, GLuint divisor) noexcept
{
    // Attach each partition separately.
    for (size_t i { 0 }; i < Partitions; ++i)
    {
        // Calculate the index and offset for the partition.
        const auto index    = static_cast<GLuint> (initialBufferIndex + i);
        const auto offset   = buffer.partitionOffset (i);

        // Finally attach the partition.
        attachVertexBuffer (buffer.getBuffer(), index, offset, stride, divisor);
    }
}

#endif // _RENDERING_OBJECTS_VERTEX_ARRAY_