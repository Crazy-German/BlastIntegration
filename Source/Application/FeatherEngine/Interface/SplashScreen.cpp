#include "FeatherEngine.pch.h"
#include "SplashScreen.h"
#include <AssetManagement/AssetTypes/MaterialAsset.h>
#include "AssetManagement/AssetManager.h"
#include <GraphicsEngine/GraphicsEngine.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_RenderInterface.h>
#include <AssetManagement/AssetTypes/Sprite.h>
#include <GraphicsEngine/GfxCmd_SetShaders.h>

SplashScreen::SplashScreen()
{
	myTimer = 0.0f;
	myThresholds = { 0.5f,  3.0f, 4.0f, 6.5f, 7.5f, 10.0f, 11.5f};
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::Initialize()
{
	myBackground = std::make_shared<Sprite2D>();
	myTgaLogo = std::make_shared<Sprite2D>();
	myGroupLogo = std::make_shared<Sprite2D>();
	myFmodLogo = std::make_shared<Sprite2D>();

	CU::Vector2<float> resolution = GetViewportSize(GraphicsEngine::Get().GetViewPort());

	float screenWidth = static_cast<float>(resolution.x);

	float ratio2k = screenWidth / 1024;
	float ratiok5 = screenWidth / 512;

	auto& jonklerFentCart = AssetManager::Get();

	myBackground->InitRelative(0.5f, 0.5f, static_cast<float>(resolution.x), static_cast<float>(resolution.y), jonklerFentCart.GetAsset<SpriteAsset>("sprite_void")->myTexture, AnchorPoint::Center);
	myTgaLogo->InitRelative(0.5f, 0.5f, (1024 * ratio2k) * 0.33f, (1024 * ratio2k) * 0.33f * 0.5f, jonklerFentCart.GetAsset<SpriteAsset>("sprite_tgalogo")->myTexture, AnchorPoint::Center);
	myGroupLogo->InitRelative(0.5f, 0.5f, (512 * ratiok5) * 0.33f, (512 * ratiok5) * 0.33f, jonklerFentCart.GetAsset<SpriteAsset>("sprite_grouplogo")->myTexture, AnchorPoint::Center);
	myFmodLogo->InitRelative(0.5f, 0.5f, (1024 * ratio2k) * 0.33f, (1024 * ratio2k) * 0.33f * 0.25f, jonklerFentCart.GetAsset<SpriteAsset>("sprite_fmodlogo")->myTexture, AnchorPoint::Center);

	myBackground->SetAlpha(1.0f);
	myTgaLogo->SetAlpha(0.0f);
	myGroupLogo->SetAlpha(0.0f);
	myFmodLogo->SetAlpha(0.0f);

	myTimer = 0.0f;
}

bool SplashScreen::Run(float aDeltaTime)
{
	myTimer += aDeltaTime;

	if (myTimer >= myThresholds[0] && myTimer < myThresholds[1])
	{
		float color = lerp(0.0f, 1.0f, (myTimer - myThresholds[0]));
		myTgaLogo->SetAlpha(color);
		//fade in tga logo
	}

	if (myTimer >= myThresholds[1] && myTimer < myThresholds[2])
	{
		float color = lerp(1.0f, 0.0f, (myTimer - myThresholds[1])*2.0f);
		myTgaLogo->SetAlpha(color);
		//fade out tga logo
	}

	if (myTimer >= myThresholds[2] && myTimer < myThresholds[3])
	{
		float color = lerp(0.0f, 1.0f, (myTimer - myThresholds[2])*2.0f);
		myFmodLogo->SetAlpha(color);
	}

	if (myTimer >= myThresholds[3] && myTimer < myThresholds[4])
	{
		float color = lerp(1.0f, 0.0f, (myTimer - myThresholds[3])*2.0f);
		myFmodLogo->SetAlpha(color);
	}

	if (myTimer >= myThresholds[4] && myTimer < myThresholds[5])
	{
		float color = lerp(0.0f, 1.0f, (myTimer - myThresholds[4])*2.0f);
		myGroupLogo->SetAlpha(color);
	}

	if (myTimer >= myThresholds[5] && myTimer < myThresholds[6])
	{
		float color = lerp(1.0f, 0.0f, (myTimer - myThresholds[5])*2.0f);
		myGroupLogo->SetAlpha(color);
	}

	if (myTimer >= myThresholds[6])
	{
		float color = lerp(1.0f, 0.0f, (myTimer - myThresholds[6]) * 2.0f);
		myBackground->SetAlpha(color);
	}

	if (myTimer >= myThresholds[6] + 0.5f)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void SplashScreen::Draw()
{
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_SetPixelShader>("UI_PS");
	//GraphicsCommandList::Get().Enqueue<GfxCmd_SetVertexShader>("UI_VS");
	myBackground->Draw();
	myFmodLogo->Draw();
	myTgaLogo->Draw();
	myGroupLogo->Draw();
}

float SplashScreen::lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}
