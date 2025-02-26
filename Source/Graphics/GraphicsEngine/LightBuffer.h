#pragma once
#include "CommonUtilities/Vector.hpp"
#include "CommonUtilities\Matrix4x4.hpp"
struct LightData
{
	CU::Vector3f Color;
	float Intensity;
	CU::Vector3f LightPos;
	int CastShadows;
	CU::Vector3f LightDir;
	float InnerConeAngle = 0.0f;
	CU::Matrix4x4f LightViewInv;
	CU::Matrix4x4f LightProj;
	int Active = 0;
	float Range;
	float OuterConeAngle;
	int LightMode;
};

struct LightBuffer
{
	LightBuffer() = default;
	LightBuffer(const LightBuffer& aLightBuffer)
	{
		DirLight = aLightBuffer.DirLight;
		PointLights[0] = aLightBuffer.PointLights[0];
		PointLights[1] = aLightBuffer.PointLights[1];
		PointLights[2] = aLightBuffer.PointLights[2];
		PointLights[3] = aLightBuffer.PointLights[3];
		PointLights[4] = aLightBuffer.PointLights[4];
		PointLights[5] = aLightBuffer.PointLights[5];
		PointLights[6] = aLightBuffer.PointLights[6];
		PointLights[7] = aLightBuffer.PointLights[7];
		SpotLights[0] = aLightBuffer.SpotLights[0];
		SpotLights[1] = aLightBuffer.SpotLights[1];
		SpotLights[2] = aLightBuffer.SpotLights[2];
		SpotLights[3] = aLightBuffer.SpotLights[3];
		SpotLights[4] = aLightBuffer.SpotLights[4];
		SpotLights[5] = aLightBuffer.SpotLights[5];
		SpotLights[6] = aLightBuffer.SpotLights[6];
		SpotLights[7] = aLightBuffer.SpotLights[7];
	}

	struct DirLightData
	{
		DirLightData() = default;
		DirLightData(const LightData& someLightData)
		{
			Color = someLightData.Color;
			Intensity = someLightData.Intensity;
			LightPos = someLightData.LightPos;
			Active = someLightData.Active;
			LightDir = someLightData.LightDir;
			Padding = 0;
			LightViewInv = someLightData.LightViewInv;
			LightProj = someLightData.LightProj;
		}
		CU::Vector3f Color;
		float Intensity;
		CU::Vector3f LightPos;
		int Active = 0;
		CU::Vector3f LightDir;
		int Padding;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
	} DirLight;
	struct PointLightData
	{
		PointLightData() = default;
		PointLightData(const LightData& someLightData)
		{
			Color = someLightData.Color;
			Intensity = someLightData.Intensity;
			LightPos = someLightData.LightPos;
			CastShadows = someLightData.CastShadows;
			LightViewInv = someLightData.LightViewInv;
			LightProj = someLightData.LightProj;
			Active = someLightData.Active;
			Range = someLightData.Range;
			Padding = CU::Vector2f(0, 0);
		}
		CU::Vector3f Color;
		float Intensity;
		CU::Vector3f LightPos;
		int CastShadows;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
		int Active = 0;
		float Range;
		CU::Vector2f Padding;
	} PointLights[8];

	struct SpotLightData
	{
		SpotLightData() = default;
		SpotLightData(const LightData& someLightData)
		{
			Color = someLightData.Color;
			Intensity = someLightData.Intensity;
			LightPos = someLightData.LightPos;
			CastShadows = someLightData.CastShadows;
			LightDir = someLightData.LightDir;
			InnerConeAngle = someLightData.InnerConeAngle;
			LightViewInv = someLightData.LightViewInv;
			LightProj = someLightData.LightProj;
			Active = someLightData.Active;
			Range = someLightData.Range;
			OuterConeAngle = someLightData.OuterConeAngle;
			Padding = 0.f;
		}
		CU::Vector3f Color;
		float Intensity;
		CU::Vector3f LightPos;
		int CastShadows;
		CU::Vector3f LightDir;
		float InnerConeAngle;
		CU::Matrix4x4f LightViewInv;
		CU::Matrix4x4f LightProj;
		int Active = 0;
		float Range;
		float OuterConeAngle;
		float Padding;
	}SpotLights[8];

	int PointLightCount = 0;
	int SpotLightCount = 0;

	CU::Vector2f padding;

};