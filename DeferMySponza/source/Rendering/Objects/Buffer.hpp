#pragma once

#if !defined    _RENDERING_OBJECTS_BUFFER_
#define         _RENDERING_OBJECTS_BUFFER_

// Engine headers.
#include <tgl/tgl.h>


/// <summary>
/// Manages an OpenGL buffer. This is a general purpose RAII encapsulation with the expectation of being used in
/// the composition of more complex object types.
/// </summary>
class Buffer
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
        /// be deleted and a fresh buffer will take its place.
        /// </summary>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise() noexcept;

        /// <summary> Deletes the buffer, freeing memory to the GPU. </summary>
        void clean() noexcept;
        

        /// <summary> 
        /// Allocates the desired amount of memory to the buffer. 
        /// This function will bind and unbind itself to the given target.
        /// </summary>
        /// <param name="size"> The total size in bytes to allocate. </param>
        /// <param name="target"> The target buffer type, e.g. GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER. </param>
        /// <param name="usage"> The usage parameter of the buffered data, e.g. GL_STATIC_DRAW/GL_DYNAMIC_DRAW. </param>
        void allocate (const GLsizeiptr size, const GLenum target, const GLenum usage) const noexcept;

        /// <summary> 
        /// Resizes the buffer to the exact size of the given data and then copies the data across. 
        /// This function will bind and unbind itself to the given target.
        /// </summary>
        /// <param name="data"> An array of data to fill the buffer with. </param>
        /// <param name="target"> The target buffer type, e.g. GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER. </param>
        /// <param name="usage"> The usage parameter of the buffered data, e.g. GL_STATIC_DRAW/GL_DYNAMIC_DRAW. </param>
        template <typename Data, template <typename, typename...> typename Container, typename... Args>
        void fillWith (const Container<Data, Args...>& data, const GLenum target, const GLenum usage) const noexcept;

        /// <summary> 
        /// Used to place data inside the previously allocated buffer at the given offset. If allocate() or fillWith() 
        /// hasn't been called then this will likely cause an OpenGL error. Bounds checking will not occur.
        /// This function will bind and unbind itself to the given target.
        /// <summary>
        /// <param name="data"> The data to place inside the buffer. </param>
        /// <param name="target"> The binding target of the buffer, e.g. GL_ARRAY_BUFFER. </param>
        /// <param name="offset"> How many bytes into the buffer the data should be written. </param>
        template <typename Data>
        void placeInside (const Data& data, const GLenum target, const GLintptr offset) const noexcept;

    private:

        GLuint m_buffer { 0 }; //!< The OpenGL ID representing the buffer object.
};


template <typename Data, template <typename, typename...> typename Container, typename... Args>
void Buffer::fillWith (const Container<Data, Args...>& data, const GLenum target, const GLenum usage) const noexcept
{
    glBindBuffer (target, m_buffer);
    glBufferData (target, data.size() * sizeof (Data), data.data(), usage);
    glBindBuffer (target, 0);
}


template <typename Data>
void Buffer::placeInside (const Data& data, const GLenum target, const GLintptr offset) const noexcept
{
    glBindBuffer (target, m_buffer);
    glBufferSubData (target, offset, sizeof (Data), &data);
    glBindBuffer (target, 0);
}

#endif // _RENDERING_OBJECTS_BUFFER_