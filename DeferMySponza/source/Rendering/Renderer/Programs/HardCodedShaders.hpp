#pragma once

#if !defined    _RENDERER_HARD_CODED_SHADERS_
#define         _RENDERER_HARD_CODED_SHADERS_

// STL headers.
#include <string>


// Imports.
using namespace std::string_literals;


const auto geometryVS       = "content:///Geometry.vs.glsl"s;
const auto fullScreenQuadVS = "content:///FullScreenQuad.vs.glsl"s;

const auto geometryFS       = "content:///Geometry.fs.glsl"s;
const auto globalLightFS    = "content:///GlobalLight.fs.glsl"s;
const auto pointLightFS     = "content:///PointLight.fs.glsl"s;
const auto spotlightFS      = "content:///Spotlight.fs.glsl"s;

#endif // !defined _RENDERER_HARD_CODED_SHADERS_
