#include "Particle_VS.hlsl"

struct ParticleGSToPS
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD;
    float4 Color : COLOR;
    float Lifetime : LIFETIME;
    float3 Velocity : VELOCITY;
    float4 ChannelMask : CHANNELMASK;
};

[maxvertexcount(4)]
void main(point ParticleVSToGS input[1], inout TriangleStream< ParticleGSToPS > output)
{
    const float2 offsets[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };
    
    float2 uvs[4] =
    {
        { 1.0f, 0.0f },
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f }
    };
    
    
    
    if(input[0].Mirror.x && input[0].Mirror.y)
    {
        uvs[0] = float2(0.0f, 1.0f);
        uvs[1] = float2(1.0f, 1.0f);
        uvs[2] = float2(0.0f, 0.0f);
        uvs[3] = float2(1.0f, 0.0f);

    }
    else if (input[0].Mirror.x)
    {
        uvs[0] = float2(0.0f, 0.0f);
        uvs[1] = float2(1.0f, 0.0f);
        uvs[2] = float2(0.0f, 1.0f);
        uvs[3] = float2(1.0f, 1.0f);

    }
    else if (input[0].Mirror.y)
    {
        uvs[0] = float2(1.0f, 1.0f);
        uvs[1] = float2(0.0f, 1.0f);
        uvs[2] = float2(1.0f, 0.0f);
        uvs[3] = float2(0.0f, 0.0f);

    }
    
    const float PI = 3.14159265358979323846f;
    const float DEGTORAD = PI / 180.0f;
    
    const ParticleVSToGS vertex = input[0];
    
	for (uint i = 0; i < 4; i++)
	{
		ParticleGSToPS result;
        result.Position = vertex.Position;
        float4 origin = result.Position;
        result.Position.xy += offsets[i] * vertex.Size;
        
        {
            float s = sin(input[0].Angle * DEGTORAD);
            float c = cos(input[0].Angle * DEGTORAD);

            result.Position.xy -= origin.xy;
            
            float newX = result.Position.x * c - result.Position.y * s;
            float newY = result.Position.x * s + result.Position.y * c;
            
            result.Position.xy = float2(newX + origin.x, newY + origin.y);
        }
        
        result.Position = mul(FB_Projection, result.Position);
        
        result.Color = vertex.Color;
        result.Lifetime = vertex.Lifetime;
        result.Velocity = vertex.Velocity;
        result.ChannelMask = vertex.ChannelMask;
        
        result.UV = uvs[i];
		output.Append(result);
	}
}