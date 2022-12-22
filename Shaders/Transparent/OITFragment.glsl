#version 400 core

uniform sampler2D accumulationTex;
uniform sampler2D revealTex;

uniform sampler2D opaqueDepthTex;
uniform sampler2D transparentDepthTex;


out vec4 fragment;
// epsilon number
const float EPSILON = 0.00001f;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    // fragment coordination
    ivec2 coords = ivec2(gl_FragCoord.xy);

    //depth information
    float opaqueDepth = texelFetch(opaqueDepthTex, coords, 0).r;
    float transparentDepth = texelFetch(transparentDepthTex, coords, 0).r;

    //depth occlusion
    if (transparentDepth - opaqueDepth >=0.000001)
        discard;

    // fragment revealage
    float revealage = texelFetch(revealTex, coords, 0).r;

    // fragment color
    vec4 accumulation = texelFetch(accumulationTex, coords, 0);

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb))))
        accumulation.rgb = vec3(accumulation.a);

    // prevent floating point precision bug
    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    // blend pixels
    fragment = vec4(average_color, 1.0 - revealage);
}