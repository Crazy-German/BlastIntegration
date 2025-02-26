#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

class Slider : public InterfaceComponent
{
public:
	Slider();
	~Slider();

	void InitRelative(float aX, float aY, float aWidth, float aHeight, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 }) override;
	void HorizontalMoveActionContinuous(float aHorizontalMove) override;
	void SetEventType(eLetterRegister aEventType);

	void Draw() override;

	void SetValue(const float aValue);
protected:

	float myValue;

	eLetterRegister myEventType;

	std::shared_ptr<Sprite2D> myFill;
	std::shared_ptr<Sprite2D> myFrame;
	std::shared_ptr<Sprite2D> myHandle;
	std::shared_ptr<Sprite2D> myHandleSelect;

	float myTotalWidth;

	CU::Vector2f myEvilValues;

	void CalculateHandlePosition(const CU::Vector2f aMousePosition);
	const CU::Vector2f GetRelativePositionOnSlider(const CU::Vector2f aMousePosition);
	void SetFillAmount(const float aPixelPosition);
};