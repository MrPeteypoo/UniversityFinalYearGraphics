#pragma once

#if !defined    _RENDERING_UNIFORMS_UNIFORM_BLOCKS_
#define         _RENDERING_UNIFORMS_UNIFORM_BLOCKS_


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Rendering/Renderer/Uniforms/Internals/Scene.hpp>
#include <Rendering/Renderer/Uniforms/Internals/DirectionalLight.hpp>
#include <Rendering/Renderer/Uniforms/Internals/PointLight.hpp>
#include <Rendering/Renderer/Uniforms/Internals/Spotlight.hpp>


/// <summary>
/// A collection of each uniform block used in the application.
/// </summary>
struct UniformBlocks final
{
    Scene               scene               { };    //!< Contains the scene-wide rendering data.
    DirectionalLight    directionalLight    { };    //!< Used to draw directional lights.
    PointLight          pointLight          { };    //!< Used to draw point lights.
    Spotlight           spotlight           { };    //!< Used to draw spotlights.

    

    constexpr static auto   sceneName               { "scene" },
                            directionalLightName    { "directionalLight" },
                            pointLightName          { "pointLight" },
                            spotlightName           { "spotlight" };
    
    constexpr static GLuint sceneBlock              { 0U }, //!< Where the scene data should be bound on the GPU.
                            directionalLightBlock   { 1U }, //!< Where the directional light data should be bound on the GPU.
                            pointLightBlock         { 2U }, //!< Where the point light data should be bound on the GPU.
                            spotlightBlock          { 3U }; //!< Where the spotlight data should be bound on the GPU.

    /// <summary>
    /// How many bytes each block should be aligned to.
    /// </summary>
    constexpr static GLintptr   blockAlignment          { 256 },
                                sceneOffset             { sceneBlock * blockAlignment },
                                directionalLightOffset  { directionalLightBlock * blockAlignment },
                                pointLightOffset        { pointLightBlock * blockAlignment },
                                spotlightOffset         { spotlightBlock * blockAlignment };
};

#endif // _RENDERING_UNIFORMS_UNIFORM_BLOCKS_
