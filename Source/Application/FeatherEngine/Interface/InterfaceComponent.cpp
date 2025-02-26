#include "FeatherEngine.pch.h"
#include "InterfaceComponent.h"
#include <AssetManagement/AssetManager.h>
#include <GraphicsEngine/Sprite2D.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include "Tooltip.h"
#include "InterfaceManager.h"

InterfaceComponent::InterfaceComponent()
{
	mySprite = std::make_shared<Sprite2D>();
	myInteractionState = InteractionState::None;
}

InterfaceComponent::~InterfaceComponent()
{
	mySprite = nullptr;
}

void InterfaceComponent::InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	
	mySprite->InitRelative(aPosition, aSize, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void InterfaceComponent::InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite->InitRelative(aX, aY, aWidth, aHeight, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void InterfaceComponent::InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	//calls below function
	InitRelative(aPosition.x, aPosition.y, aSize.x, aSize.y, aSpritePath, aAnchor);
}

void InterfaceComponent::InitRelative(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	AssetManager::Get().RegisterAsset(aSpritePath);
	auto sprite = AssetManager::Get().GetAsset<SpriteAsset>(aSpritePath.c_str());

	mySprite->InitRelative(aX, aY, aWidth, aHeight, sprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void InterfaceComponent::InitAbsolute(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite->InitAbsolute(aPosition, aSize, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void InterfaceComponent::InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite->InitRelative(aX, aY, aWidth, aHeight, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}

void InterfaceComponent::InitAbsolute(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	//calls below function
	InitAbsolute(aPosition.x, aPosition.y, aSize.x, aSize.y, aSpritePath, aAnchor);
}

void InterfaceComponent::InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor)
{
	AssetManager::Get().RegisterAsset(aSpritePath);
	auto sprite = AssetManager::Get().GetAsset<SpriteAsset>(aSpritePath.c_str());

	mySprite->InitAbsolute(aX, aY, aWidth, aHeight, sprite->myTexture, static_cast<AnchorPoint>(aAnchor));
}


void InterfaceComponent::SetPositionRelative(const CU::Vector2<float>& aPosition)
{
	mySprite->SetPositionRelative(aPosition);
}

void InterfaceComponent::SetPositionRelative(const float aX, const float aY)
{
	mySprite->SetPositionRelative(aX, aY);
}

void InterfaceComponent::SetPosition(const CU::Vector2<float>& aPosition)
{
	mySprite->SetPosition(aPosition);
}

void InterfaceComponent::SetPosition(const float aX, const float aY)
{
	mySprite->SetPosition(aX, aY);
}

void InterfaceComponent::SetColor(const CU::Vector4f& aColor)
{
	mySprite->SetColor(aColor);
}

void InterfaceComponent::SetColor(const CU::Vector3f& aColor, const float aAlpha)
{
	mySprite->SetColor(aColor, aAlpha);
}

void InterfaceComponent::SetColor(const float aR, const float aG, const float aB, const float aA)
{
	mySprite->SetColor(aR, aG, aB, aA);
}

void InterfaceComponent::SetColor(const CU::Vector3f& aColor)
{
	mySprite->SetColor(aColor);
}

void InterfaceComponent::SetColor(const float aR, const float aG, const float aB)
{
	mySprite->SetColor(aR, aG, aB);
}

void InterfaceComponent::SetAlpha(const float aAlpha)
{
	mySprite->SetAlpha(aAlpha);
}

void InterfaceComponent::SetAnchor(const SAnchorPoint aAnchor)
{
	mySprite->SetAnchor(static_cast<AnchorPoint>(aAnchor));
}

bool InterfaceComponent::IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse)
{
	aUseMouse;
	float left, right, up, down;
	left = mySprite->GetLeft();
	right = mySprite->GetRight();
	up = mySprite->GetTop();
	down = mySprite->GetBottom();

	if (aMousePosition.x < left || aMousePosition.x > right)
	{
		return false;
	}
	if (aMousePosition.y < up || aMousePosition.y > down)
	{
		return false;
	}

	return true;
}

void InterfaceComponent::SetSourceRectangle(const CU::Vector2f& aStart, const CU::Vector2f& aSize)
{
	mySprite->SetSourceRectangle(aStart, aSize);
}

void InterfaceComponent::Update(float aDeltaTime)
{
	aDeltaTime;
	if (myInteractionState == InteractionState::Clicked)
	{
		myClickTimer += aDeltaTime;
		if (myClickTimer > 0.1f)
		{
			myInteractionState = InteractionState::None;
		}
	}

	//if (myHasTooltip)
	//{
	//	myTooltip->SetActive(false);
	//}
}

void InterfaceComponent::SetInteractionState(InteractionState aInteractionState)
{
	myInteractionState = aInteractionState;
}

InteractionState InterfaceComponent::GetInteractionState()
{
	return myInteractionState;
}

void InterfaceComponent::Draw()
{
	
	mySprite->Draw();

	//if (myHasTooltip)
	//{
	//	if (myTooltip->IsActive())
	//	{
	//		myTooltip->Draw();
	//	}
	//}
}

void InterfaceComponent::ClickAction(const CU::Vector2f& aClickPosition)
{
	aClickPosition;
}

void InterfaceComponent::HorizontalMoveActionContinuous(float aHorizontalMove)
{
	aHorizontalMove;
}

void InterfaceComponent::HorizontalMoveActionStep(int aHorizontalMove)
{
	aHorizontalMove;
}

void InterfaceComponent::HoverAction()
{
	if (myIsHoverable)
	{
		if (myHasTooltip)
		{
			InterfaceManager::Get()->DrawTooltip(myTooltip);
		}
	}
}

void InterfaceComponent::AddTooltip(const std::wstring& aTooltipText)
{
	myIsHoverable = true;
	myHasTooltip = true;
	myTooltip = std::make_shared<Tooltip>();
	myTooltip->Initialize(aTooltipText, mySprite->GetPosition());
}
