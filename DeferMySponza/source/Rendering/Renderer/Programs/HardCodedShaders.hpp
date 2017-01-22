#pragma once

#if !defined    _RENDERER_HARD_CODED_SHADERS_
#define         _RENDERER_HARD_CODED_SHADERS_

// STL headers.
#include <string>


// Imports.
using namespace std::string_literals;


// Vertex shaders.
const auto geometryVS           = "content:///Geometry.vs.glsl"s;
const auto fullScreenTriangleVS = "content:///FullScreenTriangle.vs.glsl"s;
const auto lightVolumeVS        = "content:///LightVolume.vs.glsl"s;


// Fragment shaders.
const auto forwardRenderFS      = "content:///ForwardRender.fs.glsl"s;
const auto geometryFS           = "content:///Geometry.fs.glsl"s;
const auto lightingPassFS       = "content:///LightingPass.fs.glsl"s;
const auto lightsFS             = "content:///Lights.fs.glsl"s;
const auto materialFetcherFS    = "content:///MaterialFetcher.fs.glsl"s;
const auto reflectionModelsFS   = "content:///ReflectionModels.fs.glsl"s;

const std::string vertexShaderStrings[]     = { geometryVS, fullScreenTriangleVS, lightVolumeVS };
const std::string fragmentShaderStrings[]   = 
{
    forwardRenderFS, geometryFS, lightingPassFS, lightsFS, materialFetcherFS, reflectionModelsFS
};

#endif // !defined _RENDERER_HARD_CODED_SHADERS_
