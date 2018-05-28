#include "geo.h"

#define MAX_RAY_BOUNCES 4

static const float c_pi = 3.14159265359f;
static const float c_goldenRatioConjugate = 0.61803398875f;

Texture2D gBlueNoiseTexture;
RWTexture2D<float4> gOutputF32;
RWTexture2D<float4> gOutputU8;

cbuffer ShaderConstants
{
    float4x4 invViewProjMtx;
    float lerpAmount;
    float4 frameRand;
};

StructuredBuffer<Sphere> gSpheres;

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

//----------------------------------------------------------------------------
// Links to some shader friendly prngs:
// https://www.shadertoy.com/view/4djSRW "Hash Without Sine" by Dave_Hoskins
// https://www.shadertoy.com/view/MsV3z3 2d Weyl Hash by MBR
// https://github.com/gheshu/gputracer/blob/master/src/depth.glsl#L43 From Lauren @lh0xfb
// https://www.shadertoy.com/view/4tl3z4
//----------------------------------------------------------------------------
// from "hash without sine" https://www.shadertoy.com/view/4djSRW
//  2 out, 1 in...
float2 hash21(inout float p)
{
    float3 p3 = frac(float3(p,p,p) * float3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.xx + p3.yz)*p3.zy);
}

//  1 out, 2 in...
float hash12(in float2 p)
{
    float3 p3 = frac(float3(p.xyx) * float3(443.8975f, 443.8975f, 443.8975f));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//  1 out, 3 in...
float hash13(float3 p3)
{
    p3 = frac(p3 * float3(443.8975f, 443.8975f, 443.8975f));
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------
// from smallpt path tracer: http://www.kevinbeason.com/smallpt/
float3 CosineSampleHemisphere (in float3 normal, inout float rngSeed)
{
    float2 rnd = hash21(rngSeed);
    rngSeed += c_goldenRatioConjugate;

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

float3 LightOutgoing(in CollisionInfo collisionInfo, float3 rayHitPos, inout float rngSeed)
{
    float3 lightSum = float3(0.0f, 0.0f, 0.0f);
    float3 lightMultiplier = float3(1.0f, 1.0f, 1.0f);

    for (int i = 0; i <= MAX_RAY_BOUNCES; ++i)
    {
        // update our light sum and future light multiplier
        lightSum += collisionInfo.emissive * lightMultiplier;
        lightMultiplier *= collisionInfo.albedo;

        // add a random recursive sample for global illumination
        float3 newRayDir = CosineSampleHemisphere(collisionInfo.normal, rngSeed);
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

    // get the ray for this pixel
    Ray ray = GetRayForPixel(uv);

    float3 ret = float3(0.0f, 0.0f, 0.0f);

#ifdef _PIXELATE
    ret = abs(ray.direction);// *0.5f + 0.5f;
    ret *= float3(10.0f, 10.0f, 0.1f);
#else

    /*
    uint2 blueNoiseDims;
    gBlueNoiseTexture.GetDimensions(blueNoiseDims.x, blueNoiseDims.y);

    uint2 texturePixel = pixel;
    texturePixel.x = texturePixel.x % blueNoiseDims.x;
    texturePixel.y = texturePixel.y % blueNoiseDims.y;

    ret = gBlueNoiseTexture[texturePixel].rgb;
    */

    float frameRngSeed = hash13(frameRand.xyz);
    float rngSeed = hash13(float3(uv, frameRngSeed)); 

    CollisionInfo collisionInfo = RayIntersectsScene(ray);

    if (collisionInfo.collisionTime > 0.0f)
        ret = LightOutgoing(collisionInfo, ray.origin + ray.direction * collisionInfo.collisionTime, rngSeed);

#endif

    // incremental average to integrate when you get one sample at a time
    // https://blog.demofox.org/2016/08/23/incremental-averaging/
    ret = lerp(gOutputF32[pixel].rgb, ret.bgr, lerpAmount);

    gOutputF32[pixel] = float4(ret, 1.0f);
    gOutputU8[pixel] = float4(ret, 1.0f);
}