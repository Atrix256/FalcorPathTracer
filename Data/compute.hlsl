Texture2D gInput;
RWTexture2D<float4> gOutput;

[numthreads(1, 1, 1)]
void main(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadId)
{
    uint2 pixel = groupId.xy + groupThreadId.xy;

#ifdef _PIXELATE
    gOutput[pixel] = float4(0.0f, 1.0f, 1.0f, 1.0f);
#else

    uint3 resDim;
    gInput.GetDimensions(resDim.x, resDim.y);

    uint2 texturePixel = pixel;
    texturePixel.x = texturePixel.x % resDim.x;
    texturePixel.y = texturePixel.y % resDim.y;

    gOutput[pixel] = gInput[texturePixel];
#endif
}