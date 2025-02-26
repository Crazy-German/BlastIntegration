#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

struct Resolution; //could use something else, like just an index list but idk whatever this is only gonna be used for resolution anyway so doesn't matter xD

class TextComponent;

class OptionsList : public InterfaceComponent
{
public:
	OptionsList();
	~OptionsList();

	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void Update(float aDeltaTime) override;

	void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 }) override;
	void SetEventType(eLetterRegister aEventType);
	//void SetEventData(void* aWhat);
	void HorizontalMoveActionStep(int aHorizontalMove) override;

	void SetData(std::vector<Resolution*>* aData);
	void SetCurrentOption(const unsigned aIndex);

	void Draw() override;

	//void SetText(const std::wstring& aText);
protected:
	bool myTextIsSet = false;
	std::shared_ptr<TextComponent> myText;
	std::shared_ptr<Sprite2D> mySelectSprite;

	float myInputCooldown = 0.2f;
	float myRemainingInputCooldown = 0.0f;

	std::vector<Resolution*>* myData;
	eLetterRegister myEventType;

	float myX;
	float myY;

	int myIndex;

	void SetDisplayText();

	CU::Vector2f GetRelativePosition(const CU::Vector2f aMousePos);
};