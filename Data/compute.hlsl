#include "geo.h"

Texture2D gBlueNoiseTexture;
RWTexture2D<float4> gOutput;

cbuffer ShaderConstants
{
    float4x4 invViewProjMtx;
    float3 fillColor;

    float4 sphere1;
    float4 sphere1Albedo;
    float4 sphere1Emissive;

    float4 sphere2;
    float4 sphere2Albedo;
    float4 sphere2Emissive;
};

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
    ret = fillColor + float3(uv, 0.0f);

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

    CollisionInfo collisionInfo;
    collisionInfo.collisionTime = -1.0f;
    collisionInfo.normal = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.albedo = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.emissive = float3(0.0f, 0.0f, 0.0f);

    RayIntersectsSphere(ray, sphere1, collisionInfo, sphere1Albedo.rgb, sphere1Emissive.rgb);
    RayIntersectsSphere(ray, sphere2, collisionInfo, sphere2Albedo.rgb, sphere2Emissive.rgb);

    ret = collisionInfo.albedo;

    //ret = collisionInfo.collisionTime >= 0.0f ? float3(0.0f, 1.0f, 0.0f) : float3(1.0f, 0.0f, 0.0f);

#endif

    gOutput[pixel] = float4(ret.bgr, 1.0f);
}