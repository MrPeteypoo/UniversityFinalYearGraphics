#pragma once

#if !defined    _RENDERER_HARD_CODED_SHADERS_
#define         _RENDERER_HARD_CODED_SHADERS_

// STL headers.
#include <string>


// Imports.
using namespace std::string_literals;


// Preprocessing files.
const auto pbsDefines           = "content:///PhysicallyBasedShadingDefinitions.glsl"s;


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

#endif // !defined _RENDERER_HARD_CODED_SHADERS_
