#pragma once

#if !defined    _RENDERING_UNIFORMS_FULL_BLOCK_
#define         _RENDERING_UNIFORMS_FULL_BLOCK_

// STL headers.
#include <type_traits>


// Engine headers.
#include <tgl/tgl.h>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A base uniform block class, it contains the number of T objects that can fit into a single UBO block. Excess space
/// is not padded.
/// </summary>
template <typename T>
struct FullBlockWithoutPadding
{
    constexpr static auto uboSize   = 16'384;
    constexpr static auto available = uboSize - sizeof (GLuint);
    constexpr static auto max       = available / sizeof (T);
    constexpr static auto excess    = available % sizeof (T);
    
    GLuint  count { 0 };    //!< The number of objects that have data written to them.
    T       objects[max];   //!< The total number of objects that can fit into a UBO block (minus the count).
};


/// <summary>
/// Extends FullBlockWithoutPadding by adding padding to make the object equal to the size of a UBO block.
/// </summary>
template <typename T>
struct FullBlockWithPadding : public FullBlockWithoutPadding<T>
{
    private:
        GLbyte padding[excess]; //!< The amount of excess bytes that need to be inserted to fill the UBO block.
};


/// <summary>
/// Represents a full uniform buffer object block containing an array count and an array containing the maximum
/// number of objects that can fit into the block. Excess padding will be added as necessary, this object will
/// always be equal to 16KiB.
/// </summary>
template <typename T>
struct FullBlock final : public std::conditional_t<FullBlockWithoutPadding<T>::excess == 0, 
    FullBlockWithoutPadding<T>, FullBlockWithPadding<T>>
{
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_FULL_BLOCK_
