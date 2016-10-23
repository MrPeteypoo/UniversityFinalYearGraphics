#pragma once

#if !defined    _UTIL_ALGORITHM_
#define         _UTIL_ALGORITHM_


// STL headers.
#include <algorithm>


namespace util
{
    /// <summary> A shortcut for std::for_each which calls std::begin and std::end. </summary>
    template <class Container, class UnaryFunction>
    inline UnaryFunction for_each (Container&& container, UnaryFunction&& f)
    {
        return std::for_each 
        (
            std::begin (std::forward<Container> (container)), 
            std::end (std::forward<Container> (container)), 
            std::forward<UnaryFunction> (f)
        );
    }
}

#endif