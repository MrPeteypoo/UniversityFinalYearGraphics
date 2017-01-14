#pragma once

#if !defined    _RENDERING_UNIFORMS_COMPONENTS_ARRAY_ITEM_
#define         _RENDERING_UNIFORMS_COMPONENTS_ARRAY_ITEM_

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
struct ArrayItemWithoutPadding : public T
{
    constexpr static auto alignment = sizeof (glm::vec4);
    constexpr static auto excess    = alignment % sizeof (T);
};


/// <summary>
/// Extends ArrayItenWithoutPadding by adding padding to make the objects size divideable by the required array 
/// alignment size of array items in a GLSL uniform block.
/// </summary>
template <typename T>
struct ArrayItemWithPadding : public ArrayItemWithoutPadding<T>
{
    private:

        GLbyte padding[excess]; //!< The amount of excess bytes that need to be inserted to fill the UBO block.
};


/// <summary>
/// An encapsulation of an array item that is an arithmetic value. This allows it to be padded correctly.
/// </summary>
template <typename T = std::enable_if_t<std::is_arithmetic<T>::value, T>>
struct ArithmeticItem
{
    T item { 0 };   //!< An arithmetic object.
};

/// <summary>
/// Represents an item that would exist in an array. The reason this is important is because items need to be aligned
/// to the size of a glm::vec4 when used in arrays. This construct will automatically add padding to the object if
/// it isn't already aligned correctly.
/// </summary>
template <typename T, bool = std::is_arithmetic<T>::value>
struct ArrayItem final { };

template <typename T>
struct ArrayItem<T, true> final : public std::conditional_t<ArrayItemWithoutPadding<ArithmeticItem<T>>::excess == 0,
    ArrayItemWithoutPadding<ArithmeticItem<T>>, ArrayItemWithPadding<ArithmeticItem<T>>>
{
};

template <typename T>
struct ArrayItem<T, false> final : public std::conditional_t<ArrayItemWithoutPadding<T>::excess == 0,
    ArrayItemWithoutPadding<T>, ArrayItemWithPadding<T>>
{
};


// Undo the alignment.
#pragma pack (pop)

#endif // _RENDERING_UNIFORMS_COMPONENTS_ARRAY_ITEM_
