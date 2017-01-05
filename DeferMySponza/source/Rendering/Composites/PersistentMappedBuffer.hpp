#pragma once

#if !defined    _RENDERING_COMPOSITES_PERSISTANT_MAPPED_BUFFER
#define         _RENDERING_COMPOSITES_PERSISTANT_MAPPED_BUFFER

// STL headers.
#include <array>


// Personal headers.
#include <Rendering/Objects/Buffer.hpp>


/// <summary>
/// Manages a buffer that is intiaised with immutable storage and then it is mapped persistently, allowing for data
/// to be written at any time. This is a potentially dangerous object and needs to be handled carefully as to not
/// write to data which is already in use by the GPU.

/// The template paramenter determines how many partitions to split the buffer into. This allows for double and triple
/// buffering on the same buffer.
/// </summary>
template <size_t Partitions>
class PersistentMappedBuffer final
{
    public:

        static_assert (Partitions > 0, "Invalid partition value given to PersistentMappedBuffer. Must be larger than zero.");

        PersistentMappedBuffer() noexcept                                   = default;
        
        PersistentMappedBuffer (PersistentMappedBuffer&& move) noexcept;
        PersistentMappedBuffer& operator= (PersistentMappedBuffer&& move) noexcept;

        PersistentMappedBuffer (const PersistentMappedBuffer&)              = delete;
        PersistentMappedBuffer& operator= (const PersistentMappedBuffer&)   = delete;
        ~PersistentMappedBuffer() { clean(); }


        /// <summary> Check if the buffer has been initialised and is ready to be used. </summary>
        inline bool isInitialised() const noexcept  { return m_buffer != 0U; }
        
        /// <summary> Gets the OpenGL ID of buffer object. </summary>
        inline GLuint getID() const noexcept        { return m_buffer; }

        /// <summary> Gets the size of the buffer in bytes. </summary>
        inline GLintptr getSize() const noexcept    { return m_size; }


        /// <summary> 
        /// Attempt to construct and map a buffer with the given parameters. Will fail if the given size is not 
        /// divisible by the number of partitions. Successive calls will replace the buffer, invalidating any
        /// previously retrieved pointer. Upon failure the object will not be changed.
        /// </summary>
        /// <param name="size"> How many bytes of storage should be allocated. </param>
        /// <param name="read"> Will the mapped buffer be used for reading? </param>
        /// <param name="write"> Will the mapped buffer be used for writing? </param>
        /// <param name="coherent"> Should reading and writing of data be synchronised with the GPU? </param>
        /// <returns> Whether the buffer was successfully created or not. </returns>
        bool initialise (const GLintptr size, const bool read, const bool write, const bool coherent) noexcept;

        /// <summary> Deletes the buffer, freeing memory to the GPU. Also causes pointers to be invalidated. </summary>
        void clean() noexcept;


        /// <summary> Calculates the size of each individual partition in bytes. </summary>
        inline GLintptr partitionSize() const noexcept                      { return m_size / Partitions; }

        /// <summary> Calculates the byte offset into the buffer of the given partition. </summary>
        /// <param name="index"> The partition to determine the offset for. </param>
        inline GLintptr partitionOffset (const size_t index) const noexcept { return partiton * partitionSize(); }

        /// <summary> 
        /// Gets a pointer to the partition at the given index. Extreme care is required when handling the pointer.
        /// If an invalid index is given then the start of the buffer will be returned.
        /// </summary>
        /// <param name="partition"> The partition to return the pointer for. </param>
        inline void* pointer (const size_t partition) const noexcept
        {
            return partition < Partitions ? m_mapping + partitionOffset (partition) : m_mapping;
        }

        /// <summary> 
        /// Notifies OpenGL that it can find modified data at the specified range. Not required for coherent 
        /// buffers that were initialised as coherent data.
        /// </summary>
        /// <param name="partition"> The partition where data has changed. </param>
        /// <param name="startOffset"> How many bytes into the partition the modified data begins? </param>
        /// <param name="length"> How many bytes from the starting offset have been changed? </param>
        void notifyModifiedDataRange (const size_t partition, const GLintptr startOffset, const GLsizei length) noexcept;

    private:

        using Regions = std::array<void*, Partitions>;

        Buffer      m_buffer    { };            //!< The persistently mapped buffer.
        void*       m_mapping   { nullptr };    //!< A pointer provided by the GPU where we can write to.
        GLintptr    m_size      { 0 };          //!< How large the buffer is.
};

template <size_t Partitions>
using PMB       = PersistentMappedBuffer<Partitions>;
using SinglePMB = PersistentMappedBuffer<1>;
using DoublePMB = PersistentMappedBuffer<2>;
using TriplePMB = PersistentMappedBuffer<3>;

// STL headers.
#include <utility>


template <size_t Partitions>
PMB<Partitions>::PersistentMappedBuffer (PMB<Partitions>&& move) noexcept
{
    *this = std::move (move);
}


template <size_t Partitions>
PMB<Partitions>& PMB<Partitions>::operator= (PMB<Partitions>&& move) noexcept
{
    if (this != &move)
    {
        // Ensure we don't leak.
        clean();

        m_buffer    = std::move (move.m_buffer);
        m_mapping   = move.m_mapping;
        m_size      = move.m_size;

        move.m_mapping  = nullptr;
        move.m_size     = 0U;
    }

    return *this;
}


template <size_t Partitions>
bool PMB<Partitions>::initialise (const GLintptr size, const bool read, const bool write, const bool coherent) noexcept
{
    return true;
}


template <size_t Partitions>
void PMB<Partitions>::clean() noexcept
{
    if (isInitialised())
    {
        m_buffer.clean();
        m_mapping   = nullptr;
        m_size      = 0U;
    }
}


template <size_t Partitions>
void PMB<Partitions>::notifyModifiedDataRange (const size_t partition, const GLintptr startOffset, const GLsizei length) noexcept
{
    glFlushMappedNamedBufferRange (getID(), partitionOffset (partition) + startOffset, length);
}

#endif // _RENDERING_COMPOSITES_PERSISTANT_MAPPED_BUFFER