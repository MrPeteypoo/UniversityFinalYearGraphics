#pragma once

#if !defined    _RENDERER_HARD_CODED_SHADERS_
#define         _RENDERER_HARD_CODED_SHADERS_

// STL headers.
#include <string>


// Imports.
using namespace std::string_literals;


// Definitions.
const auto pbsDefines       = "content:///Shaders/Defines/PhysicallyBasedShading.glsl"s;
const auto SMAAVSDefines    = "content:///Shaders/Defines/SMAAVertexShader.glsl"s;
const auto SMAAFSDefines    = "content:///Shaders/Defines/SMAAFragmentShader.glsl"s;


// Vertex shaders.
const auto geometryVS               = "content:///Shaders/Rendering/Geometry.vs.glsl"s;
const auto fullScreenTriangleVS     = "content:///Shaders/Rendering/FullScreenTriangle.vs.glsl"s;
const auto lightVolumeVS            = "content:///Shaders/Rendering/LightVolume.vs.glsl"s;
const auto edgeDetectionVS          = "content:///Shaders/SMAA/EdgeDetection.vs.glsl"s;
const auto blendingWeightVS         = "content:///Shaders/SMAA/BlendingWeightCalculation.vs.glsl"s;
const auto neighborhoodBlendingVS   = "content:///Shaders/SMAA/NeighborhoodBlending.vs.glsl"s;


// Fragment shaders.
const auto forwardRenderFS          = "content:///Shaders/Rendering/ForwardRender.fs.glsl"s;
const auto geometryFS               = "content:///Shaders/Rendering/Geometry.fs.glsl"s;
const auto lightingPassFS           = "content:///Shaders/Rendering/LightingPass.fs.glsl"s;
const auto lightsFS                 = "content:///Shaders/Rendering/Lights.fs.glsl"s;
const auto materialFetcherFS        = "content:///Shaders/Rendering/MaterialFetcher.fs.glsl"s;
const auto reflectionModelsFS       = "content:///Shaders/Rendering/ReflectionModels.fs.glsl"s;
const auto edgeDetectionFS          = "content:///Shaders/SMAA/EdgeDetection.fs.glsl"s;
const auto blendingWeightFS         = "content:///Shaders/SMAA/BlendingWeightCalculation.fs.glsl"s;
const auto neighborhoodBlendingFS   = "content:///Shaders/SMAA/NeighborhoodBlending.fs.glsl"s;


// Others.
const auto SMAAUberShader = "content:///Shaders/SMAA/SMAA.hlsl"s;

#endif // !defined _RENDERER_HARD_CODED_SHADERS_
