#pragma once
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
//#include "../Graphics/GraphicsEngine/Sprite2D.h"
//#include "../Graphics/GraphicsEngine/SpriteAnchor.h"

class Sprite2D;

class Tooltip;

enum class InteractionState
{
	None,
	Selected,
	Clicked
};


enum class SAnchorPoint
{
	Center,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left
};


class InterfaceComponent
{
public:
	InterfaceComponent();
	~InterfaceComponent();

	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void InitAbsolute(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void SetPositionRelative(const CU::Vector2<float>& aPosition);
	void SetPositionRelative(const float aX, const float aY);

	void SetPosition(const CU::Vector2<float>& aPosition);
	void SetPosition(const float aX, const float aY);

	void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor, const float aAlpha);
	void SetColor(const float aR, const float aG, const float aB, const float aA);

	void SetColor(const CU::Vector3f& aColor);
	void SetColor(const float aR, const float aG, const float aB);
	void SetAlpha(const float aAlpha);

	void SetAnchor(const SAnchorPoint aAnchor);

	//Returns true if mouse position was inside the bounds of the object
	virtual bool IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse = true);

	void SetSourceRectangle(const CU::Vector2f& aStart, const CU::Vector2f& aSize);

	virtual void Update(float aDeltaTime);
	virtual void SetInteractionState(InteractionState aInteractionState);
	virtual InteractionState GetInteractionState();

	virtual void Draw();

	virtual void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 });

	//input is between -1.0 for left and 1.0 for right
	virtual void HorizontalMoveActionContinuous(float aHorizontalMove);
	//input is -1 for left and 1 for right
	virtual void HorizontalMoveActionStep(int aHorizontalMove);

	virtual void HoverAction();
	virtual void AddTooltip(const std::wstring& aTooltipText);
protected:
	std::shared_ptr<Sprite2D> mySprite;
	

	float myClickTimer;
	bool myIsHoverable = true;
	bool myHasTooltip = false;
	std::shared_ptr<Tooltip> myTooltip;
	InteractionState myInteractionState;
};