// Minote - glsl/smaaNeighbor.vert.glsl
// SMAA neighborhood blending stage.
// Adapted from https://github.com/turol/smaaDemo

#version 330 core

out vec2 fTexCoords;
out vec4 fOffset;

uniform vec4 screenSize;

#define SMAA_RT_METRICS screenSize
#define SMAA_INCLUDE_VS 1
#define SMAA_INCLUDE_PS 0
#include "smaaParams.glslh"
#include "util.glslh"

void main()
{
    vec2 pos = triangleVertex(gl_VertexID, fTexCoords);

    fOffset = vec4(0.0, 0.0, 0.0, 0.0);
    SMAANeighborhoodBlendingVS(fTexCoords, fOffset);
    gl_Position = vec4(pos, 1.0, 1.0);
}
