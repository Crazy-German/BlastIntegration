#include "Buffers/AnimationBuffer.hlsli"
#include "Buffers/FrameBuffer.hlsli"
#include "Buffers/ObjectBuffer.hlsli"
#include "Includes/DefaultShaderIncludes.hlsli"


DefaultVStoPS main(DefaultVertex vertex)
{
    DefaultVStoPS result;

    float3x3 vxWorldInvT = (float3x3) OB_WorldInvT;
    vertex.Normal = normalize(vertex.Normal);
    vertex.Tangent = normalize(vertex.Tangent);

    if (OB_HasSkinning)
    {
        float4x4 skinMatrix = AB_JointTransforms[vertex.BoneIDs[0]] * vertex.SkinWeights[0];
        skinMatrix += AB_JointTransforms[vertex.BoneIDs[1]] * vertex.SkinWeights[1];
        skinMatrix += AB_JointTransforms[vertex.BoneIDs[2]] * vertex.SkinWeights[2];
        skinMatrix += AB_JointTransforms[vertex.BoneIDs[3]] * vertex.SkinWeights[3];
        vertex.Position = mul(skinMatrix, vertex.Position);

        float3x3 skinMatrix3x3T = (float3x3) skinMatrix;
        skinMatrix3x3T = transpose(skinMatrix3x3T);
        vertex.Normal = mul(skinMatrix3x3T, vertex.Normal);
        vertex.Tangent = mul(skinMatrix3x3T, vertex.Tangent);
    }

    vertex.Normal = mul(vxWorldInvT, vertex.Normal);
    vertex.Tangent = mul(vxWorldInvT, vertex.Tangent);
    float3 binormal = cross(vertex.Normal, vertex.Tangent);

    float4 vxWorldPos = mul(OB_World, vertex.Position);
    float4 vxViewPos = mul(FB_InvView, vxWorldPos);
    float4 vxProjPos = mul(FB_Projection, vxViewPos);

    result.Position = vxProjPos;
    result.WorldPos = vxWorldPos.xyz;
    result.Color = vertex.Color;
    result.UV = vertex.UV;
    result.Normal = vertex.Normal;
    result.Tangent = vertex.Tangent;
    result.Binormal = binormal;
    result.DebugMode = 0;
    
    return result;
};