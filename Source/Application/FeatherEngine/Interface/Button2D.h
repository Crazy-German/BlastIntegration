#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

class TextComponent;

class Button2D : public InterfaceComponent
{
public:
	Button2D();
	~Button2D();

	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::filesystem::path& aSpritePath, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 }) override;
	void SetEventType(eLetterRegister aEventType);
	void SetEventData(void* aWhat);
	bool IsHovered(const CU::Vector2f& aMousePosition, bool aUseMouse = true) override;

	void Update(float aDeltaTime) override;
	void Draw() override;

	void SetText(const std::wstring& aText);
protected:
	bool myTextIsSet = false;
	std::shared_ptr<TextComponent> myText;
	void* myWhat;
	std::shared_ptr<Sprite2D> mySelectSprite;
	std::shared_ptr<Sprite2D> myClickSprite;

	void RealClickAction();

	eLetterRegister myEventType;
};