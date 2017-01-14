#pragma once

#if !defined    _RENDERING_RENDERER_GLOBAL_CONFIG_
#define         _RENDERING_RENDERER_GLOBAL_CONFIG_

// Personal headers.
#include <Rendering/Composites/DrawCommands.hpp>
#include <Rendering/Composites/PersistentMappedBuffer.hpp>


struct GlobalConfig final
{
    constexpr static auto multiBuffering = size_t { 5 }; //!< How much multi-buffering should be performed on dynamic buffers.

    using PMB = PersistentMappedBuffer<multiBuffering>;
};

#endif // _RENDERING_RENDERER_GLOBAL_CONFIG_