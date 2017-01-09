#pragma once

#if !defined    _RENDERING_OBJECTS_BUFFER_
#define         _RENDERING_OBJECTS_BUFFER_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Manages an OpenGL buffer. This is a general purpose RAII encapsulation with the expectation of being used in
/// the composition of more complex object types.
/// </summary>
class Buffer final
{
    public:

        Buffer() noexcept                   = default;

        Buffer (Buffer&& move) noexcept;
        Buffer& operator= (Buffer&& move) noexcept;

        Buffer (const Buffer&)              = delete;
        Buffer& operator= (const Buffer&)   = delete;

        ~Buffer() { clean(); }


        /// <summary> Check if the buffer has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_buffer != 0U; }
        
        /// <summary> Gets the OpenGL ID of buffer object. </summary>
        inline GLuint getID() const noexcept        { return m_buffer; }


        /// <summary> 
        /// Attempt to initialise the buffer object. Successive calls to this function will cause the stored buffer to
        /// be deleted and a fresh buffer will take its place. Upon failure the object will not be modified.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> Deletes the buffer, freeing memory to the GPU. </summary>
        void clean() noexcept;
        

        /// <summary> 
        /// Allocates immutable storage to the buffer. This is only valid to use once, afterwards any attempt to
        /// reallocate data will be an error. Doing this will allow for more optimisation possibilities and is
        /// recommended. The contents of the buffer will be undefined after calling this function. Note: You cannot
        /// create static buffer this way, you need to use immutablyFillWith().
        /// </summary>
        /// <param name="size"> How many bytes of memory to allocate. </param>
        /// <param name="flags"> 
        /// Flags that determine the capabilities of the buffer. E.g. GL_DYNAMIC_STORAGE_BIT allows for data to be
        /// placed inside the storage. 0 should not be given because the contents will be permanently uninitialised.
        /// </param>
        void allocateImmutableStorage (const GLsizeiptr size, const GLbitfield flags = GL_DYNAMIC_STORAGE_BIT) noexcept
        {
            glNamedBufferStorage (m_buffer, size, nullptr, flags);
        }

        /// <summary> 
        /// Allocates immutable storage to the buffer. This is only valid to use once, afterwards any attempt to
        /// reallocate data will be an error. Doing this will allow for more optimisation possibilities and is
        /// recommended. Static buffers can be created using this method as long as the flags parameter isn't given.
        /// </summary>
        /// <param name="data"> A container of data to fill the buffer with. </param>
        /// <param name="flags"> 
        /// Flags that determine the capabilities of the buffer. E.g. GL_DYNAMIC_STORAGE_BIT allows for data to be
        /// placed inside the storage. 0 will signify that the data is static and will never be changed.
        /// </param>
        /// <returns> The amount of allocated data. </returns>
        template <typename Data, template <typename, typename = size_t...> typename Container, typename... Args>
        GLsizeiptr immutablyFillWith (const Container<Data, Args...>& data, const GLbitfield flags = 0) noexcept
        {
            const auto size = data.size() * sizeof (Data);
            glNamedBufferStorage (m_buffer, size, data.data(), flags);
            return size;
        }

        /// <summary> 
        /// A catch-all overload where the passed object will fill the buffer.
        /// </summary>
        /// <returns> The amount of allocated data. </returns>
        template <typename Data>
        GLsizeiptr immutablyFillWith (Data& data, const GLbitfield flags = 0) noexcept
        {
            glNamedBufferStorage (m_buffer, sizeof (Data), &data, flags);
            return sizeof (Data);
        }

        /// <summary> 
        /// Allocates the desired amount of memory to the buffer. This function will bind and unbind itself to the
        /// given target. Mutable storage can be reallocated but may restrict implementation optimisation. This
        /// should not be performed after allocating immutable storage.
        /// </summary>
        /// <param name="size"> The total size in bytes to allocate. </param>
        /// <param name="usage"> The usage hint for the buffered data, e.g. GL_STATIC_DRAW/GL_DYNAMIC_DRAW. </param>
        void allocateMutableStorage (const GLsizeiptr size, const GLenum usage) noexcept
        {
            glNamedBufferData (m_buffer, size, nullptr, usage);
        }

        /// <summary> 
        /// Resizes the buffer to the exact size of the given data and then copies the data across. 
        /// This cannot be performed after allocating immutable storage.
        /// </summary>
        /// <param name="data"> An array of data to fill the buffer with. </param>
        /// <param name="usage"> The usage parameter of the buffered data, e.g. GL_STATIC_DRAW/GL_DYNAMIC_DRAW. </param>
        template <typename Data, template <typename, typename = size_t...> typename Container, typename... Args>
        GLsizeiptr mutablyFillWith (const Container<Data, Args...>& data, const GLenum usage) noexcept
        {
            const auto size = data.size() * sizeof (Data);
            glNamedBufferData (m_buffer, data.size() * sizeof (Data), data.data(), usage);
            return size;
        }

        /// <summary> 
        /// Used to place data inside the previously allocated buffer at the given offset. If allocate() or fillWith() 
        /// hasn't been called then this will likely cause an OpenGL error. Bounds checking will not occur.
        /// If the storage is immutable then GL_DYNAMIC_STORAGE_BIT must have been specified to do this.
        /// </summary>
        /// <param name="data"> The data to place inside the buffer. </param>
        /// <param name="offset"> How many bytes into the buffer the data should be written. </param>
        template <typename Data, template <typename, typename = size_t...> typename Container, typename... Args>
        void placeAt (const GLintptr offset, const Container<Data, Args...>& data) noexcept
        {
            glNamedBufferSubData (m_buffer, offset, data.size() * sizeof (Data), data.data());
        }

        /// <summary> 
        /// Used to place data inside the previously allocated buffer at the given offset. If allocate() or fillWith() 
        /// hasn't been called then this will likely cause an OpenGL error. Bounds checking will not occur.
        /// If the storage is immutable then GL_DYNAMIC_STORAGE_BIT must have been specified to do this.
        /// </summary>
        /// <param name="data"> The data to place inside the buffer. </param>
        /// <param name="offset"> How many bytes into the buffer the data should be written. </param>
        template <typename Data>
        void placeAt (const GLintptr offset, const Data& data) noexcept
        {
            glNamedBufferSubData (m_buffer, offset, sizeof (Data), &data);
        }

        /// <summary> 
        /// Used to place data inside the previously allocated buffer at the given offset. If allocate() or fillWith() 
        /// hasn't been called then this will likely cause an OpenGL error. Bounds checking will not occur.
        /// If the storage is immutable then GL_DYNAMIC_STORAGE_BIT must have been specified to do this.
        /// </summary>
        /// <param name="data"> The data to place inside the buffer. </param>
        /// <param name="offset"> How many bytes into the buffer the data should be written. </param>
        void placeAt (const GLintptr offset, const size_t size, const void* data) noexcept
        {
            if (data)
            {
                glNamedBufferSubData (m_buffer, offset, size, data);
            }
        }

        /// <summary> 
        /// Maps a portion of the buffer memory based on the parameters given. This is a potentially expensive
        /// operation and should not be performed frequently. The const version of this function only permits read
        /// access. Note: The offset given may not be the offset returned. This is because the returned pointer will be 
        /// aligned according to GL_MIN_MAP_ALIGNMENT.
        /// </summary>
        /// <param name="offset"> The offset into the buffer to map. </param>
        /// <param name="length"> How many bytes after the offset should be accessible. </param>
        /// <param name="access"> What you will do with the mapped range. E.g. GL_MAP_READ_BIT. </param>
        void* mapRange (const GLintptr offset, const GLsizeiptr length, const GLbitfield access) const noexcept;

        /// <summary> 
        /// Maps a portion of the buffer memory based on the parameters given. This is a potentially expensive
        /// operation and should not be performed frequently. Note: The offset given may not be the offset returned. 
        /// This is because the returned pointer will be aligned according to GL_MIN_MAP_ALIGNMENT.
        /// </summary>
        /// <param name="offset"> The offset into the buffer to map. </param>
        /// <param name="length"> How many bytes after the offset should be accessible. </param>
        /// <param name="access"> What you will do with the mapped range. E.g. GL_MAP_WRITE_BIT. </param>
        void* mapRange (const GLintptr offset, const GLsizeiptr length, const GLbitfield access)  noexcept;

        /// <summary> 
        /// Unmaps any previously mapped buffer range. If an error occurs then the contents of the range are undefined.
        /// </summary>
        /// <returns> If an error occurred when unmapping. </returns>
        bool unmap() const noexcept;

        /// <summary> 
        /// Tells the driver that the contents can be discarded, potentially speeding up subsequent placement and
        /// mapping operations.
        /// </summary>
        void invalidate() noexcept;

        /// <summary> Invalidates the specified range inside the buffer. </summary>
        /// <param name="offset"> The offset inside the buffer to start invalidating. </param>
        /// <param name="length"> How many bytes should be invalidated. </param>
        void invalidateRange (const GLintptr offset, const GLsizeiptr length) noexcept;

    private:

        GLuint m_buffer { 0 }; //!< The OpenGL ID representing the buffer object.
};

#endif // _RENDERING_OBJECTS_BUFFER_