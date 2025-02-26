#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

class TextComponent;

class Checkbox : public InterfaceComponent
{
public:
	Checkbox();
	~Checkbox();

	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void SetState(bool aState);
	void ToggleState();

	void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 }) override;
	void SetEventType(eLetterRegister aEventType);
	void SetEventData(void* aWhat);
	bool IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse = true) override;

	void Update(float aDeltaTime) override;
	void Draw() override;

	void SetText(const std::wstring& aText);
protected:
	bool myTextIsSet = false;
	bool myState = false;
	std::shared_ptr<TextComponent> myText;
	void* myWhat;
	std::shared_ptr<Sprite2D> mySelectSprite;
	std::shared_ptr<Sprite2D> myClickSprite;
	std::shared_ptr<Sprite2D> myCheckSprite;

	void RealClickAction();

	eLetterRegister myEventType;
};