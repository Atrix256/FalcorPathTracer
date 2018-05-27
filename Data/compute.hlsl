#include "geo.h"

#define MAX_RAY_BOUNCES 2

Texture2D gBlueNoiseTexture;
RWTexture2D<float4> gOutput;

cbuffer ShaderConstants
{
    float4x4 invViewProjMtx;
    float lerpAmount;
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

[numthreads(1, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadId)
{
    // calculate the percentage across the screen that we are
    uint2 resolution;
    gOutput.GetDimensions(resolution.x, resolution.y);
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



    CollisionInfo collisionInfo = RayIntersectsScene(ray);

    uint i = 0;
    while (collisionInfo.collisionTime > 0.0f && i < MAX_RAY_BOUNCES)
    {
        // TODO: real lighting
        ret += collisionInfo.albedo * 0.25f;

        // TODO: diffuse reflection, not specular!
        Ray newRay;
        newRay.origin = ray.origin + ray.direction * collisionInfo.collisionTime;
        newRay.direction = reflect(ray.direction, collisionInfo.normal);

        // TODO: a minimum collision time to prevent self intersection
        newRay.origin += newRay.direction * 0.01f;

        ray = newRay;

        collisionInfo = RayIntersectsScene(ray);

        ++i;
    }


#endif

    // incremental average to integrate when you get one sample at a time
    // https://blog.demofox.org/2016/08/23/incremental-averaging/
    ret = lerp(gOutput[pixel].rgb, ret.bgr, lerpAmount);

    gOutput[pixel] = float4(ret, 1.0f);
}