cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_World;
    float4x4 OB_WorldInvT;

    bool OB_HasSkinning;
	unsigned int OB_MaterialCount;
	bool OB_IsEnemy;
	bool OB_IsElite;

    float OB_IsHit;
    float3 OB_Padding;
}