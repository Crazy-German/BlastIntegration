#include "../Includes/DefaultShaderIncludes.hlsli"
#include "../Buffers/FrameBuffer.hlsli"
#include "../Buffers/ObjectBuffer.hlsli"

DefaultVStoPS main(DefaultVertex input)
{
    DefaultVStoPS result;
	//From local to world
    float4 world = mul(OB_WorldInvT, input.Position);
    
	//From world to view
    float4 view = mul(FB_InvView, world);
	
	//from view to clip
    float4 clip = mul(FB_Projection, view);
    
    result.Color = input.Color;
    result.Normal = input.Normal;
    result.UV = input.UV;
    result.Tangent = input.Tangent;
	
    result.Position = clip;
    result.Color = input.Color;
    return result;
}