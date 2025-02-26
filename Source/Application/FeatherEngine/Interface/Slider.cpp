#include "Slider.h"
#include <CommonUtilities/Input.h>
#include "../GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include <AssetManagement/AssetManager.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include <EngineSettings.h>

Slider::Slider()
{
}

Slider::~Slider()
{
	mySprite = nullptr;
	myFill = nullptr;
	myFrame = nullptr;
	myHandle = nullptr;
	myHandleSelect = nullptr;
}

void Slider::InitRelative(float aX, float aY, float aWidth, float aHeight, SAnchorPoint aAnchor)
{
	auto& asset = AssetManager::Get();

	mySprite = std::make_shared<Sprite2D>();
	myFill = std::make_shared<Sprite2D>();
	myFrame = std::make_shared<Sprite2D>();
	myHandle = std::make_shared<Sprite2D>();
	myHandleSelect = std::make_shared<Sprite2D>();

	mySprite->InitRelative(aX, aY, aWidth, aHeight, asset.GetAsset<SpriteAsset>("sprite_slider_background")->myTexture, static_cast<AnchorPoint>(aAnchor));

	CU::Vector2<float> resolution = static_cast<CU::Vector2f>(GetViewportSize(GraphicsEngine::Get().GetViewPort()));
	myFill->InitAbsolute(aX * resolution.x - (aWidth * 0.5f), aY * resolution.y, aWidth, aHeight, asset.GetAsset<SpriteAsset>("sprite_slider_fill")->myTexture, AnchorPoint::Left);
	myFrame->InitRelative(aX, aY, aWidth, aHeight, asset.GetAsset<SpriteAsset>("sprite_slider_frame")->myTexture, static_cast<AnchorPoint>(aAnchor));
	myHandle->InitRelative(aX, aY, 64 * EngineSettings::Get().GetResolutionScale(), 64 * EngineSettings::Get().GetResolutionScale(), asset.GetAsset<SpriteAsset>("sprite_slider_handle")->myTexture, static_cast<AnchorPoint>(aAnchor));
	myHandleSelect->InitRelative(aX, aY, 64 * EngineSettings::Get().GetResolutionScale(), 64 * EngineSettings::Get().GetResolutionScale(), asset.GetAsset<SpriteAsset>("sprite_slider_handle_select")->myTexture, static_cast<AnchorPoint>(aAnchor));

	myEvilValues.x = (aX * resolution.x) - (aWidth * 0.5f);
	myEvilValues.y = aY * resolution.y;

	myTotalWidth = aWidth ;
}

void Slider::ClickAction(const CU::Vector2f& aClickPosition)
{
	
	CalculateHandlePosition(aClickPosition * EngineSettings::Get().GetResolutionScaleInverse()); //think i made this needlessly complicated

	myValue = std::clamp(myValue, 0.0f, 1.0f);
	if (myValue > 0.9875f)
	{
		SetValue(1.0f);
	}
	else if (myValue < 0.0125f)
	{
		SetValue(0.0f);
	}

	GameObjectPostMaster::Get()->Send({ myEventType, &myValue });
}

void Slider::HorizontalMoveActionContinuous(float aHorizontalMove)
{
	SetValue(myValue + aHorizontalMove);
}

void Slider::SetEventType(eLetterRegister aEventType)
{
	myEventType = aEventType;
}

void Slider::Draw()
{
	mySprite->Draw();
	//myFill->Draw(); //this crashes the game because of the rendering sprite being killed by the time it's going to render, will try to figure out a fix
	myFrame->Draw();

	if (myInteractionState == InteractionState::Selected)
	{
		myHandleSelect->Draw();
	}
	else
	{
		myHandle->Draw();
	}
}

void Slider::SetValue(const float aValue)
{
	myValue = aValue;
	if (myValue > 1.0f)
	{
		myValue = 1.0f;
	}
	if (myValue < 0.0f)
	{
		myValue = 0.0f;
	}
	//myValue = std::clamp(aValue, 0.0f, 1.0f);
	CalculateHandlePosition({ (myValue * myTotalWidth + mySprite->GetLeft()) *EngineSettings::Get().GetResolutionScaleInverse()/*(1 + 1 - EngineSettings::Get().GetResolutionScale())*/  , 0 });
}

void Slider::CalculateHandlePosition(CU::Vector2f aMousePosition)
{
	float sliderValue = GetRelativePositionOnSlider(aMousePosition).x;
	SetFillAmount(sliderValue);
	myHandle->SetPosition(std::clamp(sliderValue + mySprite->GetLeft(), 10.0f * EngineSettings::Get().GetResolutionScale() + mySprite->GetLeft(), 502 * EngineSettings::Get().GetResolutionScale() + mySprite->GetLeft()), myHandle->GetPosition().y);
	myHandleSelect->SetPosition(std::clamp(sliderValue + mySprite->GetLeft(), 10.0f * EngineSettings::Get().GetResolutionScale() + mySprite->GetLeft(), 502 * EngineSettings::Get().GetResolutionScale() + mySprite->GetLeft()), myHandle->GetPosition().y);
	sliderValue /= myTotalWidth;
	myValue = sliderValue;
}

const CU::Vector2f Slider::GetRelativePositionOnSlider(const CU::Vector2f aMousePosition)
{
	CU::Vector2f topLeft = { mySprite->GetLeft(), mySprite->GetTop() }; // 679, 464
	CU::Vector2f mousePos = aMousePosition * EngineSettings::Get().GetResolutionScale(); //792, 496

	return mousePos - topLeft;
}

void Slider::SetFillAmount(const float aPixelPosition)
{
	myFill = nullptr;
	myFill = std::make_shared<Sprite2D>();
	myFill->InitAbsolute(myEvilValues.x, myEvilValues.y, aPixelPosition, 64 * EngineSettings::Get().GetResolutionScale(), AssetManager::Get().GetAsset<SpriteAsset>("sprite_slider_fill")->myTexture, AnchorPoint::Left);
	//myFill->InitRelative(myEvilValues.x, myEvilValues.y, aPixelPosition, 64, AssetManager::Get().GetAsset<SpriteAsset>("sprite_slider_fill")->myTexture, AnchorPoint::Left); //this is diabolical
	myFill->SetSourceRectangle(CU::Vector2f(0, 0), CU::Vector2f(aPixelPosition * EngineSettings::Get().GetResolutionScaleInverse(), 64)); //? not a clue if this works
	//need to resize the sprite as well :(((((((((((((((((((((((((((((((((((
	//myFill->se
}
