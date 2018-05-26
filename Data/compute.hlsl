Texture2D gBlueNoiseTexture;
RWTexture2D<float4> gOutput;

cbuffer ShaderConstants
{
    float4x4 viewProjectionInverseMtx;
    float3 fillColor;
};

[numthreads(1, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadId)
{
    uint2 resolution;
    gOutput.GetDimensions(resolution.x, resolution.y);

    uint2 pixel = groupId.xy + groupThreadId.xy;

    float2 pixelPercent = float2(pixel) / float2(resolution);

    float3 ret = float3(0.0f, 0.0f, 0.0f);

#ifdef _PIXELATE
    ret = fillColor + float3(pixelPercent, 0.0f);
#else

    uint2 blueNoiseDims;
    gBlueNoiseTexture.GetDimensions(blueNoiseDims.x, blueNoiseDims.y);

    uint2 texturePixel = pixel;
    texturePixel.x = texturePixel.x % blueNoiseDims.x;
    texturePixel.y = texturePixel.y % blueNoiseDims.y;

    ret = gBlueNoiseTexture[texturePixel].rgb;
#endif

    gOutput[pixel] = float4(ret.bgr, 1.0f);
}