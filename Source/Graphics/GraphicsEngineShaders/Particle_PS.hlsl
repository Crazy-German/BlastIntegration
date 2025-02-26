#include "Quad_GS.hlsl"
Texture2D texAlbedo : register(t0);
SamplerState texSampler : register(s0);

float4 main(ParticleGSToPS input) : SV_TARGET
{
	if(input.Lifetime < 0.0f)
    {
        discard;
        return 0;
    }
    
    const float4 textureColor = texAlbedo.Sample(texSampler, input.UV) * input.ChannelMask;
    //const float color = textureColor.r + textureColor.g + textureColor.b + textureColor.a;
    //return color * input.Color;
    return textureColor * input.Color;
}