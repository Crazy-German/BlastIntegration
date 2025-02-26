#include "FeatherEngine.pch.h"
#include "SpriteInstance.h"
#include <AssetManagement/AssetManager.h>
#include "Checkbox.h"
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include "TextComponent.h"
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "../GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include "Tooltip.h"
#include <EngineSettings.h>
#include "MainSingleton.h"

Checkbox::Checkbox()
{
	myWhat = nullptr;
	MainSingleton::Get().GetAudioEngine().LoadSound("ClickSound_SFX.wav", SoundType::SFX);
	mySelectSprite = std::make_shared<Sprite2D>();
	myClickSprite = std::make_shared<Sprite2D>();
	myCheckSprite = std::make_shared<Sprite2D>();
}

Checkbox::~Checkbox()
{
	myText = nullptr;
}

void Checkbox::InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, SAnchorPoint aAnchor)
{

	auto baseSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox");
	auto selectSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_select");
	auto clickSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_click");
	auto checkSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_check");

	mySprite->InitRelative(aPosition, aSize, baseSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	mySelectSprite->InitRelative(aPosition, aSize, selectSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myClickSprite->InitRelative(aPosition, aSize, clickSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myCheckSprite->InitRelative(aPosition, aSize, checkSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void Checkbox::InitRelative(float aX, float aY, float aWidth, float aHeight, SAnchorPoint aAnchor)
{
	auto baseSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox");
	auto selectSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_select");
	auto clickSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_click");
	auto checkSprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_checkbox_check");


	mySprite->InitRelative(aX, aY, aWidth, aHeight, baseSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	mySelectSprite->InitRelative(aX, aY, aWidth, aHeight, selectSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myClickSprite->InitRelative(aX, aY, aWidth, aHeight, clickSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myCheckSprite->InitRelative(aX, aY, aWidth, aHeight, checkSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}



void Checkbox::SetState(bool aState)
{
	myState = aState;
}

void Checkbox::ToggleState()
{
	myState = !myState;
}

void Checkbox::ClickAction(const CU::Vector2f& aClickPosition)
{
	aClickPosition;
	MainSingleton::Get().GetAudioEngine().PlaySoundFromKey("ClickSound_SFX");
	SetInteractionState(InteractionState::Clicked);
	myClickTimer = 0.0f;
	
}

void Checkbox::SetEventType(eLetterRegister aEventType)
{
	myEventType = aEventType;
}

void Checkbox::SetEventData(void* aWhat)
{
	myWhat = aWhat;
}

bool Checkbox::IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse)
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

void Checkbox::Update(float aDeltaTime)
{
	if (myInteractionState == InteractionState::Clicked)
	{
		myClickTimer += aDeltaTime;
		if (myClickTimer > 0.1875f)
		{
			SetInteractionState(InteractionState::None);
			RealClickAction();
		}
	}
}

void Checkbox::Draw()
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

	if (myState)
	{
		myCheckSprite->Draw();
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

void Checkbox::SetText(const std::wstring& aText)
{
	
	myTextIsSet = true;
	myText = std::make_shared<TextComponent>();
	myText->InitAbsolute(mySprite->GetPosition(), 1.75f * EngineSettings::Get().GetResolutionScale(), AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, aText);
}

void Checkbox::RealClickAction()
{
	ToggleState();
	bool* state = new bool(myState);
	GameObjectPostMaster::Get()->Send({ myEventType, state});
	if (myEventType == eLetterRegister::ExitGame)
	{
		PostQuitMessage(0);
	}
	delete state;
}
