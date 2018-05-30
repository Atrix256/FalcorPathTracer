#include "geo.h"

#ifndef MAX_RAY_BOUNCES
    #define MAX_RAY_BOUNCES 4
#endif

#ifndef SAMPLES_PER_FRAME
    #define SAMPLES_PER_FRAME 1
#endif

static const float c_pi = 3.14159265359f;
static const float c_goldenRatioConjugate = 0.61803398875f;

Texture2D gBlueNoiseTexture;
RWTexture2D<float4> gOutputF32;
RWTexture2D<float4> gOutputU8;

cbuffer ShaderConstants
{
    float4x4 invViewProjMtx;
    float lerpAmount;
    uint frameRand;
    uint frameNumber;
};

StructuredBuffer<Sphere> gSpheres;

uint RNG(inout uint state)
{
    uint x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 15;
    state = x;
    return x;
}

float RandomFloat01(inout uint state)
{
    return (RNG(state) & 0xFFFFFF) / 16777216.0f;
}

Ray GetRayForPixel(float2 uv)
{
    // convert from [0,1] space to [-1,1] space
    float2 pixelClipSpace = uv * 2.0f - 1.0f;

    // transform the clip space pixel at z 0 to get the ray origin in world space
    Ray ret;
    float4 origin = mul(float4(pixelClipSpace, 0.0f, 1.0f), invViewProjMtx);
    origin /= origin.w;
    ret.origin = origin.xyz;

    // transform the clip space pixel at a different z to get another world space point along the ray, to make the direction from
    float4 destination = mul(float4(pixelClipSpace, 0.1f, 1.0f), invViewProjMtx);
    destination /= destination.w;
    ret.direction = normalize(destination.xyz - origin.xyz);

    return ret;
}

CollisionInfo RayIntersectsScene(Ray ray)
{
    CollisionInfo collisionInfo;
    collisionInfo.collisionTime = -1.0f;
    collisionInfo.normal = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.albedo = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.emissive = float3(0.0f, 0.0f, 0.0f);

    // test the spheres
    {
        uint count = 0;
        uint stride;
        gSpheres.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            RayIntersectsSphere(ray, gSpheres[i], collisionInfo);
    }

    return collisionInfo;
}

float3 CosineSampleHemisphere (in float3 normal, inout uint rngState)
{
    float2 rnd;
    rnd.x = RandomFloat01(rngState);
    rnd.y = RandomFloat01(rngState);

    float r1 = 2.0f * c_pi * rnd.x;
    float r2 = rnd.y;
    float r2s = sqrt(r2);

    float3 w = normal;
    float3 u;
    if (abs(w[0]) > 0.1f)
        u = cross(float3(0.0f, 1.0f, 0.0f), w);
    else
        u = cross(float3(1.0f, 0.0f, 0.0f), w);

    u = normalize(u);
    float3 v = cross(w, u);
    float3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1.0f - r2));
    d = normalize(d);

    return d;
}

float3 LightOutgoing(in CollisionInfo collisionInfo, float3 rayHitPos, inout uint rngState)
{
    float3 lightSum = float3(0.0f, 0.0f, 0.0f);
    float3 lightMultiplier = float3(1.0f, 1.0f, 1.0f);

    for (int i = 0; i <= MAX_RAY_BOUNCES; ++i)
    {
        // update our light sum and future light multiplier
        lightSum += collisionInfo.emissive * lightMultiplier;
        lightMultiplier *= collisionInfo.albedo;

        // add a random recursive sample for global illumination
        float3 newRayDir = CosineSampleHemisphere(collisionInfo.normal, rngState);
        Ray newRay;
        newRay.origin = rayHitPos;
        newRay.direction = newRayDir;
        CollisionInfo newCollisionInfo = RayIntersectsScene(newRay);

        // if we hit something new, we continue
        if (newCollisionInfo.collisionTime >= 0.0f)
        {
            collisionInfo = newCollisionInfo;
            rayHitPos += newRayDir * newCollisionInfo.collisionTime;
        }
        // else we missed so we are done
        else
        {
            i = MAX_RAY_BOUNCES;
        }
    }

    return lightSum;
}

[numthreads(1, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadId)
{
    // calculate the percentage across the screen that we are
    uint2 resolution;
    gOutputF32.GetDimensions(resolution.x, resolution.y);
    uint2 pixel = groupId.xy + groupThreadId.xy;
    float2 uv = float2(pixel) / float2(resolution);

    uint rngState = (pixel.x * 1973 + pixel.y * 9277 + frameNumber * 26699) | 1;
    rngState = rngState ^ frameRand;

    float3 ret = float3(0.0f, 0.0f, 0.0f);

    /*
    uint2 blueNoiseDims;
    gBlueNoiseTexture.GetDimensions(blueNoiseDims.x, blueNoiseDims.y);

    uint2 texturePixel = pixel;
    texturePixel.x = texturePixel.x % blueNoiseDims.x;
    texturePixel.y = texturePixel.y % blueNoiseDims.y;

    ret = gBlueNoiseTexture[texturePixel].rgb;
    */

    for (uint i = 0; i < SAMPLES_PER_FRAME; ++i)
    {
        // get a random offset to jitter the pixel by
        float2 uvOffset = float2(RandomFloat01(rngState), RandomFloat01(rngState)) * float2(1.0f / float(resolution.x), 1.0f / float(resolution.y));

        // get the ray for this pixel
        Ray ray = GetRayForPixel(uv + uvOffset);

        CollisionInfo collisionInfo = RayIntersectsScene(ray);

        if (collisionInfo.collisionTime > 0.0f)
            ret += LightOutgoing(collisionInfo, ray.origin + ray.direction * collisionInfo.collisionTime, rngState);
    }

    ret /= float(SAMPLES_PER_FRAME);

    // incremental average to integrate when you get one sample at a time
    // https://blog.demofox.org/2016/08/23/incremental-averaging/
    ret = lerp(gOutputF32[pixel].rgb, ret.bgr, lerpAmount);

    gOutputF32[pixel] = float4(ret, 1.0f);
    gOutputU8[pixel] = float4(ret, 1.0f);
}