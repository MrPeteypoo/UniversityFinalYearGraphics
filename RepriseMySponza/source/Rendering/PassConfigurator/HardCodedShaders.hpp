#pragma once

#if !defined    _RENDERER_HARD_CODED_SHADERS_
#define         _RENDERER_HARD_CODED_SHADERS_


// STL headers.
#include <string>


// Imports.
using namespace std::string_literals;


const auto sponzaVS             = "content:///sponza_vs.glsl"s;
const auto sponzaFS             = "content:///sponza_fs.glsl"s;
const auto vertexVS             = "content:///vertex.vs.glsl"s;
const auto ambientFS            = "content:///ambient.fs.glsl"s;
const auto directionalLightFS   = "content:///directionalLight.fs.glsl"s;
const auto pointLightFS         = "content:///pointLight.fs.glsl"s;
const auto spotlightFS          = "content:///spotlight.fs.glsl"s;

#endif // !defined _RENDERER_HARD_CODED_SHADERS_
