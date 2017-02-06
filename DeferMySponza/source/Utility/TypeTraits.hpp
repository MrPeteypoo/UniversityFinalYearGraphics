#pragma once

#if !defined    _UTILITY_TYPE_TRAITS_
#define         _UTILITY_TYPE_TRAITS_

// STL headers.
#include <type_traits>


namespace util
{
    template <typename T, typename... Args>
    struct is_any;

    template <typename T, typename Check>
    struct is_any<T, Check>
    {
        constexpr static auto value = std::is_same<T, Check>::value;
    };

    template <typename T, typename Check, typename... Args>
    struct is_any<T, Check, Args...>
    {
        constexpr static auto value = is_any<T, Check>::value || is_any<T, Args...>::value;
    };

    template <typename T, typename... Args>
    constexpr auto is_any_v = is_any<T, Args...>::value;
}

#endif // _UTILITY_TYPE_TRAITS_
