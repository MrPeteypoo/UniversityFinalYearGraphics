#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_ALIGNED_ITEM_
#define         _RENDERING_UNIFORMS_COMPONENTS_ALIGNED_ITEM_

// STL headers.
#include <type_traits>


// Engine headers.
#include <glm/vec4.hpp>


// We'll manage the data alignment by enforcing 4-byte alignment for all types.
#pragma pack (push, 4)


/// <summary>
/// A base class for an item that appears in an array within a uniform block. It provides ways of measuring the size of
/// an object and whether it will need any padding to be used in an array.
/// </summary>
template <typename T>
struct AlignedItemWithoutPadding : public T
{
    constexpr static auto alignment = sizeof (glm::vec4);
    constexpr static auto unaligned = sizeof (T) % alignment;
};


/// <summary>
/// Extends ArrayItenWithoutPadding by adding padding to make the objects size divideable by the required array 
/// alignment size of array items in a GLSL uniform block.
/// </summary>
template <typename T>
struct AlignedItemWithPadding : public AlignedItemWithoutPadding<T>
{
    private:

        GLbyte padding[alignment - unaligned]; //!< The amount of excess bytes that need to be inserted to pad the object to an acceptible size.
};


/// <summary>
/// An encapsulation of an array item that is an arithmetic value. This allows it to be padded correctly.
/// </summary>
template <typename T = std::enable_if_t<std::is_arithmetic<T>::value, T>>
struct ArithmeticItem
{
    T value { 0 };   //!< An arithmetic object.  
};

/// <summary>
/// Represents an item that would exist in an array. The reason this is important is because items need to be aligned
/// to the size of a glm::vec4 when used in arrays. This construct will automatically add padding to the object if
/// it isn't already aligned correctly.
/// </summary>
template <typename T, bool = std::is_arithmetic<T>::value>
struct AlignedItem final { };

template <typename T>
struct AlignedItem<T, true> final : public std::conditional_t<AlignedItemWithoutPadding<ArithmeticItem<T>>::unaligned == 0,
    AlignedItemWithoutPadding<ArithmeticItem<T>>, AlignedItemWithPadding<ArithmeticItem<T>>>
{
    AlignedItem& operator= (const T val) noexcept
    {
        value = val;
        return *this;
    }
};

template <typename T>
struct AlignedItem<T, false> final : public std::conditional_t<AlignedItemWithoutPadding<T>::unaligned == 0,
    AlignedItemWithoutPadding<T>, AlignedItemWithPadding<T>>
{
    AlignedItem& operator= (T&& forward) noexcept
    {
        T::operator= (std::forward<T> (forward));

        return *this;
    }
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_ALIGNED_ITEM_
