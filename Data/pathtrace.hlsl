#include "geo.h"

#ifndef MAX_RAY_BOUNCES
    #define MAX_RAY_BOUNCES 4
#endif

#ifndef SAMPLES_PER_FRAME
    #define SAMPLES_PER_FRAME 1
#endif

#ifndef SAMPLE_LIGHTS
    #define SAMPLE_LIGHTS 1
#endif

#ifndef WORK_GROUP_SIZE
    #define WORK_GROUP_SIZE 16
#endif

#ifndef BOKEH_SHAPE
    #define BOKEH_SHAPE 0
#endif

// keep in sync with main.cpp enum BokehShape
#define BOKEH_SHAPE_CIRCLE   0
#define BOKEH_SHAPE_CIRCLEG  1
#define BOKEH_SHAPE_SQUARE   2
#define BOKEH_SHAPE_RING     3
#define BOKEH_SHAPE_TRIANGLE 4
#define BOKEH_SHAPE_SOD      5

static const float c_pi = 3.14159265359f;
static const float c_goldenRatioConjugate = 0.61803398875f;

// Note, this is how you'd read the blue noise texture: gBlueNoiseTexture[texturePixel].r;
Texture2D gBlueNoiseTexture;

RWTexture2D<float4> gOutputF32;
RWTexture2D<float4> gOutputU8;

cbuffer ShaderConstants
{
    float4x4 invViewProjMtx;
    float4x4 viewMtx;
    float4x4 invViewMtx;
    float3 skyColor;
    float lerpAmount;
    uint frameRand;
    uint frameNumber;
    float DOFFocalLength;
    float DOFApertureRadius;
    float Exposure;
    float3 cameraPos;
    float3 cameraRight;
    float3 cameraUp;
    float3 cameraFwd;
    float4 sensorPlane;
    float4 focalPlane;
};

StructuredBuffer<Sphere> g_spheres;
StructuredBuffer<Sphere> g_lightSpheres;
StructuredBuffer<Quad> g_quads;
StructuredBuffer<PLight> g_plights;

uint wang_hash(inout uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float RandomFloat01(inout uint state)
{
    return float(wang_hash(state)) / 4294967296.0;
}

float3 RandomUnitVector(inout uint state)
{
    float z = RandomFloat01(state) * 2.0f - 1.0f;
    float a = RandomFloat01(state) * 2.0f * 3.1415926f;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return float3(x, y, z);
}

float2 PointInTriangle(in float2 A, in float2 B, in float2 C, in float2 rand)
{
    return (1.0f - sqrt(rand.x)) * A.yx + sqrt(rand.x)*(1.0f - rand.y) * B.yx + rand.y * sqrt(rand.x) * C.yx;
}

float SignedTriArea(float2 a, float2 b, float2 c)
{
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

float TriArea(float2 a, float2 b, float2 c)
{
    return abs(SignedTriArea(a, b, c));
}

Ray GetRayForPixel(float2 uv, inout uint state, out float lightMultiplier)
{
    lightMultiplier = 1.0f;

    // convert from [0,1] space to [-1,1] space
    float2 pixelClipSpace = uv * 2.0f - 1.0f;
    pixelClipSpace.x *= -1.0f;

    // the ray starts at the camera
    Ray ret;
    ret.origin = cameraPos;

    // transform the clip space pixel at z 0 to be able to calculate the ray origin in world space
    float4 destination = mul(float4(pixelClipSpace, 0.0f, 1.0f), invViewProjMtx);
    destination /= destination.w;
    ret.direction = -normalize(destination.xyz - cameraPos);

    // if DOF is on, need to adjust the origin and direction based on aperture size and shape
    #ifdef ENABLE_DOF
        float3 fwdVector = ret.direction;
        float3 upVector = cameraUp;
        float3 rightVector = cameraRight;

        #if BOKEH_SHAPE == BOKEH_SHAPE_SQUARE
            float2 offset = (float2(RandomFloat01(state), RandomFloat01(state)) * 2.0f - 1.0f) * DOFApertureRadius;
            float shapeArea = 4.0f * DOFApertureRadius;
        #elif BOKEH_SHAPE == BOKEH_SHAPE_CIRCLE
            float angle = RandomFloat01(state) * 2.0f * c_pi;
            float radius = sqrt(RandomFloat01(state));
            float2 offset = float2(cos(angle), sin(angle)) * radius * DOFApertureRadius;
            float shapeArea = c_pi * DOFApertureRadius * DOFApertureRadius;
        #elif BOKEH_SHAPE == BOKEH_SHAPE_CIRCLEG
            float r = sqrt(-2.0f * log(RandomFloat01(state)));
            float theta = 2 * c_pi*RandomFloat01(state);
            float2 offset;
            offset.x = r * cos(theta);
            offset.y = r * sin(theta);
            offset *= DOFApertureRadius;
            float shapeArea = c_pi * DOFApertureRadius * DOFApertureRadius;
        #elif BOKEH_SHAPE == BOKEH_SHAPE_RING
            float angle = RandomFloat01(state) * 2.0f * c_pi;
            float2 offset = float2(cos(angle), sin(angle)) * DOFApertureRadius;
            float shapeArea = 2.0f * c_pi * DOFApertureRadius;
        #elif BOKEH_SHAPE == BOKEH_SHAPE_TRIANGLE
            const float2 A = float2(1.0f, 0.0f); //0 degrees
            const float2 B = float2(-0.5f, 0.866f); //120 degrees
            const float2 C = float2(-0.5f, -0.866f); //120 degrees

            float2 rand = float2(RandomFloat01(state), RandomFloat01(state));

            float2 offset = PointInTriangle(A, B, C, rand);

            offset *= DOFApertureRadius;

            float shapeArea = TriArea(A*DOFApertureRadius,B*DOFApertureRadius,C*DOFApertureRadius);
        #elif BOKEH_SHAPE == BOKEH_SHAPE_SOD

            const float2 A = float2(1.0f, 0.0f);
            const float2 B = float2(-0.5f, 0.866f);
            const float2 C = float2(-0.5f, -0.866f);

            const float2 D = float2(0.5f, 0.866f);
            const float2 E = float2(-1.0f, 0.0f);
            const float2 F = float2(0.5f, -0.866f);

            const float2 G = float2(0.5f, 0.288f);
            const float2 H = float2(0.0f, 0.577f);
            const float2 I = float2(-0.5f, 0.288f);

            const float2 J = float2(-0.5f, -0.288f);
            const float2 K = float2(0.0f, -0.577f);
            const float2 L = float2(0.5f, -0.288f);

            float triangleChoice = RandomFloat01(state);
            float2 offset;

            float2 rand = float2(RandomFloat01(state), RandomFloat01(state));
            if (triangleChoice < 0.76)
                offset = PointInTriangle(A, B, C, rand);
            else if (triangleChoice < 0.84)
                offset = PointInTriangle(G, D, H, rand);
            else if (triangleChoice < 0.92)
                offset = PointInTriangle(I, E, J, rand);
            else
                offset = PointInTriangle(K, F, L, rand);

            offset *= DOFApertureRadius;

            float shapeArea =
                TriArea(A*DOFApertureRadius, B*DOFApertureRadius, C*DOFApertureRadius) +
                TriArea(G*DOFApertureRadius, D*DOFApertureRadius, H*DOFApertureRadius) +
                TriArea(I*DOFApertureRadius, E*DOFApertureRadius, J*DOFApertureRadius) +
                TriArea(K*DOFApertureRadius, F*DOFApertureRadius, L*DOFApertureRadius);
        #endif

        // we are only sampling one sample in the bokeh shape, but light is coming in from all samples so we need to account for that.
        // a more mathematical way of looking at this is that we need to divide by the pdf to do proper monte carlo integration.
        // the pdf is 1/shapeArea, so dividing by that pdf is the same as multiplying by shapeArea. 
        lightMultiplier = shapeArea;

        // Find where the ray hits the sensor plane (negative time) and move the image plane to the focal length distance.
        // This is where the ray starts.
        // Note: could also adjust FOV as focal distance changed, but this feels closer to the real geometry.
        // FOV is the FOV at a focal distance of 1.
        float3 sensorPos;
        {
            float t = -(dot(ret.origin, sensorPlane.xyz) + sensorPlane.w) / dot(ret.direction, sensorPlane.xyz);
            sensorPos = ret.origin + ret.direction * t;

            // convert the sensorPos from world space to camera space
            float3 cameraSpacePos = mul(float4(sensorPos, 1.0f), viewMtx).xyz;

            // elongate z by the focal length
            cameraSpacePos.z *= DOFFocalLength;

            // convert back into world space
            sensorPos = mul(float4(cameraSpacePos, 1.0f), invViewMtx).xyz;
        }

        // calculate the world space point chosen on the aperture
        float3 aperturePos = cameraPos + rightVector * offset.x + upVector.xyz * offset.y;

        #ifdef PINHOLE_CAMERA
            // pinhole camera shoots the ray from the sensor position to the aperture position
            ret.origin = sensorPos;
            ret.direction = normalize(aperturePos - sensorPos);
        #else
            // lense camera shoots a ray from the aperture position to the focus position on the focus plane
            float3 rstart = cameraPos;
            float3 rdir = -normalize(destination.xyz - cameraPos);
            float t = -(dot(rstart, focalPlane.xyz) + focalPlane.w) / dot(rdir, focalPlane.xyz);
            float3 focusPos = rstart + rdir * t;
            ret.origin = aperturePos;
            ret.direction = normalize(focusPos - aperturePos);
        #endif
    #endif

    return ret;
}

CollisionInfo RayIntersectsScene(Ray ray, bool allowEmissive, float maxT = -1.0f)
{
    CollisionInfo collisionInfo;
    collisionInfo.collisionTime = maxT;
    collisionInfo.normal = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.albedo = float3(0.0f, 0.0f, 0.0f);
    collisionInfo.emissive = float3(0.0f, 0.0f, 0.0f);

    // test the spheres
    {
        uint count = 0;
        uint stride;
        g_spheres.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            RayIntersects(ray, g_spheres[i], collisionInfo);
    }

    // test the light spheres
    {
        uint count = 0;
        uint stride;
        g_lightSpheres.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            RayIntersects(ray, g_lightSpheres[i], collisionInfo);
    }

    // test the quads
    {
        uint count = 0;
        uint stride;
        g_quads.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            RayIntersects(ray, g_quads[i], collisionInfo);
    }

    if (!allowEmissive)
        collisionInfo.emissive = float3(0.0f, 0.0f, 0.0f);

    return collisionInfo;
}

float lengthSq(float3 v)
{
    return dot(v, v);
}

float3 SampleLight(in CollisionInfo collisionInfo, in float3 position, inout uint rngState, in Sphere sphere)
{
    // for sampling the spherical light, see:
    // https://github.com/aras-p/ToyPathTracer/blob/01-initial/Cpp/Source/Test.cpp#L83

    // don't sample self
    if (collisionInfo.geoID == sphere.geoID)
        return float3(0.0f, 0.0f, 0.0f);

    // TODO: clean this up

    float distToLight = length(sphere.position - position);

    // create a random direction towards sphere
    // coord system for sampling: sw, su, sv
    float3 sw = normalize(sphere.position - position);
    float3 su = normalize(cross(abs(sw.x)>0.01f ? float3(0, 1, 0) : float3(1, 0, 0), sw));
    float3 sv = cross(sw, su);

    // sample sphere by solid angle
    float cosAMax = sqrt(1.0f - sphere.radius*sphere.radius / lengthSq(position - sphere.position));
    float eps1 = RandomFloat01(rngState);
    float eps2 = RandomFloat01(rngState);
    float cosA = 1.0f - eps1 + eps1 * cosAMax;
    float sinA = sqrt(1.0f - cosA * cosA);
    float phi = 2 * c_pi* eps2;
    float3 l = su * cos(phi) * sinA + sv * sin(phi) * sinA + sw * cosA;
    l = normalize(l);

    // raytrace against the scene
    Ray ray;
    ray.origin = position;
    ray.direction = l;
    CollisionInfo newCollisionInfo = RayIntersectsScene(ray, true);

    // TODO: maybe give a max distance. the below "missing" counting as valid doesn't make sense w/o that btw.

    // if we missed the world, or hit the light we wanted to, return the light
    if (newCollisionInfo.collisionTime < 0.0f || newCollisionInfo.geoID == sphere.geoID)
    {
        float omega = 2 * c_pi * (1 - cosAMax);

        float3 nDotL = max(dot(collisionInfo.normal, l), 0.0f);

        return collisionInfo.albedo * sphere.emissive * nDotL * omega / c_pi;
    }
    // otherwise, return no light
    else
        return float3(0.0f, 0.0f, 0.0f);
}

float3 SampleLight(in CollisionInfo collisionInfo, in float3 position, in PLight light)
{
    float3 vecToLight = light.position - position;
    float distToLight = length(vecToLight);
    float3 dirToLight = normalize(vecToLight);

    // raytrace against the scene
    Ray ray;
    ray.origin = position;
    ray.direction = dirToLight;
    CollisionInfo newCollisionInfo = RayIntersectsScene(ray, true);

    // if we didn't hit anything, apply the lighting
    if (newCollisionInfo.collisionTime < 0.0f || newCollisionInfo.collisionTime > distToLight)
    {
        // TODO: is this correct?
        float3 nDotL = max(dot(collisionInfo.normal, dirToLight), 0.0f);

        // distance attenuation
        float atten = 1.0f / (distToLight*distToLight);

        return collisionInfo.albedo * light.color * atten * 2.0f * nDotL;
    }
    // otherwise, return no light
    else
        return float3(0.0f, 0.0f, 0.0f);
}

float3 SampleLights(in CollisionInfo collisionInfo, in float3 position, inout uint rngState)
{
    float3 ret = float3(0.0f, 0.0f, 0.0f);

    // test the light spheres
    {
        uint count = 0;
        uint stride;
        g_lightSpheres.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            ret += SampleLight(collisionInfo, position, rngState, g_lightSpheres[i]);
    }

    // TODO: sample quad lights

    // test the point lights
    {
        uint count = 0;
        uint stride;
        g_plights.GetDimensions(count, stride);

        for (uint i = 0; i < count; i++)
            ret += SampleLight(collisionInfo, position, g_plights[i]);
    }

    return ret;
}

float3 LightOutgoing(in CollisionInfo collisionInfo, in float3 rayHitPos, inout uint rngState)
{
    float3 lightSum = float3(0.0f, 0.0f, 0.0f);
    float3 lightMultiplier = float3(1.0f, 1.0f, 1.0f);
    float cosTheta = 1.0f;

    #if SAMPLE_LIGHTS == 1
    bool allowEmissive = false;
    #else
    bool allowEmissive = true;
    #endif

    for (int i = 0; i <= MAX_RAY_BOUNCES; ++i)
    {
        // do explicit light sampling if we should
        #if SAMPLE_LIGHTS == 1
        lightSum += SampleLights(collisionInfo, rayHitPos, rngState) * lightMultiplier;
        #endif

        // update our light sum and future light multiplier
        lightSum += collisionInfo.emissive * lightMultiplier;
        lightMultiplier *= collisionInfo.albedo * cosTheta;

        // add a random recursive sample for global illumination
        #ifdef COSINE_WEIGHTED_HEMISPHERE_SAMPLING
        float3 newRayDir = normalize(collisionInfo.normal + RandomUnitVector(rngState));
        #else
        float3 newRayDir = RandomUnitVector(rngState);
        if (dot(collisionInfo.normal, newRayDir) < 0.0f)
            newRayDir *= -1.0f;
        cosTheta = 2.0f * dot(collisionInfo.normal, newRayDir);
        #endif

        Ray newRay;
        newRay.origin = rayHitPos;
        newRay.direction = newRayDir;
        CollisionInfo newCollisionInfo = RayIntersectsScene(newRay, allowEmissive);

        // if we hit something new, we continue
        if (newCollisionInfo.collisionTime >= 0.0f)
        {
            collisionInfo = newCollisionInfo;
            rayHitPos += newRayDir * newCollisionInfo.collisionTime;
        }
        // else we missed so we are done
        else
        {
            lightSum += skyColor * lightMultiplier;
            i = MAX_RAY_BOUNCES;
        }
    }

    return lightSum;
}

[numthreads(WORK_GROUP_SIZE, WORK_GROUP_SIZE, 1)]
void main(uint3 gid : SV_DispatchThreadID)
{
    // calculate the percentage across the screen that we are
    uint2 resolution;
    gOutputF32.GetDimensions(resolution.x, resolution.y);
    uint2 pixel = gid.xy;
    float2 uv = float2(pixel) / float2(resolution);

    // set up our initial random number generation state
    uint rngState = (pixel.x * 1973 + pixel.y * 9277 + frameNumber * 26699) | 1;
    rngState = rngState ^ frameRand;

    float3 ret = float3(0.0f, 0.0f, 0.0f);
    for (uint i = 0; i < SAMPLES_PER_FRAME; ++i)
    {
        // get a random offset to jitter the pixel by
        float2 uvOffset = float2(RandomFloat01(rngState), RandomFloat01(rngState)) * float2(1.0f / float(resolution.x), 1.0f / float(resolution.y));

        // get the ray for this pixel
        float lightMultiplier = 1.0f;
        Ray ray = GetRayForPixel(uv + uvOffset, rngState, lightMultiplier);

        CollisionInfo collisionInfo = RayIntersectsScene(ray, true);

        if (collisionInfo.collisionTime > 0.0f)
            ret += LightOutgoing(collisionInfo, ray.origin + ray.direction * collisionInfo.collisionTime, rngState) * lightMultiplier;
        else
            ret += skyColor * lightMultiplier;
    }
    ret /= float(SAMPLES_PER_FRAME);

    // incremental average to integrate when you get one sample at a time
    // https://blog.demofox.org/2016/08/23/incremental-averaging/
    ret = lerp(gOutputF32[pixel].rgb, ret.bgr, lerpAmount);

    gOutputF32[pixel] = float4(ret, 1.0f);

    // convert from linear to sRGB for output
    gOutputU8[pixel] = float4(pow(ret * Exposure, 1.0f / 2.2f), 1.0f);
}