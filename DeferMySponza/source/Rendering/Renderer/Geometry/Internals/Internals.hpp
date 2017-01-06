#pragma once

#if !defined    _RENDERING_GEOMETRY_INTERNALS_
#define         _RENDERING_GEOMETRY_INTERNALS_

// STL headers.
#include <unordered_map>


// Personal headers.
#include <Rendering/Renderer/Geometry/Geometry.hpp>


/// <summary>
/// Contains internal data that doesn't need to be accessed externally but needs to be stored nonetheless.
/// </summary>
struct Geometry::Internals final
{
    constexpr static auto   sceneVerticesIndex  = size_t { 0 },             //!< The index of the scene vertices buffer.
                            sceneElementsIndex  = sceneVerticesIndex + 1,   //!< The index of the scene elements buffer.
                            transformsIndex     = sceneElementsIndex + 1,   //!< The index of the transforms buffer.
                            materialIDsIndex    = transformsIndex + 1,      //!< The index of the material IDs buffer.
                            lightVerticesIndex  = materialIDsIndex + 1,     //!< The index of the light vertices buffer.
                            lightElementsIndex  = lightVerticesIndex + 1,   //!< The index of the light elements buffer.
                            bufferCount         = lightElementsIndex + 1;   //!< The total number of stored buffers.

    using Meshes    = std::unordered_map<scene::MeshId, Mesh>;
    using Buffers   = std::array<Buffer, bufferCount>;
    
    Meshes  sceneMeshes { };    //!< A list of mesh data for buffered scene meshes.
    Buffers buffers     { };    //!< Contains pretty much every static buffer for scene and lighting geometry.
    
    Internals()                                         = default;
    Internals (Internals&&)                             = default;
    Internals (const Internals&) noexcept               = default;
    Internals& operator= (const Internals&) noexcept    = default;
    Internals& operator= (Internals&&) noexcept         = default;
    ~Internals()                                        = default;

    bool isInitialised() const noexcept 
    {
        for (const auto& buffer : buffers)
        {
            if (!buffer.isInitialised())
            {
                return false;
            }
        }

        return true;
    }

    bool initialise() noexcept
    {
        sceneMeshes.reserve (128);

        for (auto& buffer : buffers)
        {
            if (!buffer.initialise())
            {
                return false;
            }
        }

        return true;
    }

    void clean() noexcept
    {
        sceneMeshes.clear();
       
        for (auto& buffer : buffers)
        {
            buffer.clean();
        }
    }
};

#endif // _RENDERING_GEOMETRY_INTERNALS_