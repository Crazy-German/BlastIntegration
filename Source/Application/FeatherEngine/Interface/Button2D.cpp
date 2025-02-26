#include "FeatherEngine.pch.h"
#include "SpriteInstance.h"
#include <AssetManagement/AssetManager.h>
#include "Button2D.h"
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include "TextComponent.h"
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "../GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include "Tooltip.h"
#include <EngineSettings.h>

Button2D::Button2D()
{
	myWhat = nullptr;
	mySelectSprite = std::make_shared<Sprite2D>();
	myClickSprite = std::make_shared<Sprite2D>();
}

Button2D::~Button2D()
{
	myText = nullptr;
}

void Button2D::InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite->InitRelative(aPosition, aSize, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));

	auto ssprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_select");
	auto csprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_click");
	mySelectSprite->InitRelative(aPosition, aSize, ssprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myClickSprite->InitRelative(aPosition, aSize, csprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void Button2D::InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite->InitRelative(aX, aY, aWidth, aHeight, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));

	auto ssprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_select");
	auto csprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_click");
	mySelectSprite->InitRelative(aX, aY, aWidth, aHeight, ssprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myClickSprite->InitRelative(aX, aY, aWidth, aHeight, csprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void Button2D::InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	InitRelative(aPosition.x, aPosition.y, aSize.x, aSize.y, aSpritePath, aAnchor);
}

void Button2D::InitRelative(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	AssetManager::Get().RegisterAsset(aSpritePath);
	auto sprite = AssetManager::Get().GetAsset<SpriteAsset>(aSpritePath.c_str());

	auto ssprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_select");
	auto csprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_button_click");

	mySprite->InitRelative(aX, aY, aWidth, aHeight, sprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	mySelectSprite->InitRelative(aX, aY, aWidth, aHeight, ssprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myClickSprite->InitRelative(aX, aY, aWidth, aHeight, csprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void Button2D::ClickAction(const CU::Vector2f& aClickPosition)
{
	aClickPosition;
	SetInteractionState(InteractionState::Clicked);
	myClickTimer = 0.0f;
	
}

void Button2D::SetEventType(eLetterRegister aEventType)
{
	myEventType = aEventType;
}

void Button2D::SetEventData(void* aWhat)
{
	myWhat = aWhat;
}

bool Button2D::IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse)
{
	if (aUseMouse)
	{
		if (InterfaceComponent::IsHovered(aMousePosition) && myInteractionState != InteractionState::Clicked)
		{
			SetInteractionState(InteractionState::Selected);
			return true;
		}
		else if(myInteractionState != InteractionState::Clicked)
		{
			SetInteractionState(InteractionState::None);
			return false;
		}
	}
	return false;
}

void Button2D::Update(float aDeltaTime)
{
	if (myInteractionState == InteractionState::Clicked)
	{
		myClickTimer += aDeltaTime;
		if (myClickTimer > 0.1f)
		{
			SetInteractionState(InteractionState::None);
			RealClickAction();
		}
	}
}

void Button2D::Draw()
{
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	//GraphicsCommandList::Get().Enqueue<GfxCmd_SetTextureResource>(PIPELINE_STAGE_PIXEL_SHADER, 0, mySprite->GetTexture());
	switch (myInteractionState)
	{
	case InteractionState::None:
		mySprite->Draw();
		break;

	case InteractionState::Selected:
		mySelectSprite->Draw();
		break;

	case InteractionState::Clicked:
		myClickSprite->Draw();
		break;
	}
	

	if (myText)
	{
		myText->Draw();
	}

	if (myHasTooltip)
	{
		if (myTooltip->IsActive())
		{
			myTooltip->Draw();
		}
	}
}

void Button2D::SetText(const std::wstring& aText)
{
	
	myTextIsSet = true;
	myText = std::make_shared<TextComponent>();
	myText->InitAbsolute(mySprite->GetPosition(), 1.75f * EngineSettings::Get().GetResolutionScale(), AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, aText);
}

void Button2D::RealClickAction()
{
	GameObjectPostMaster::Get()->Send({ myEventType, myWhat });
	if (myEventType == eLetterRegister::ExitGame)
	{
		PostQuitMessage(0);
	}
}
