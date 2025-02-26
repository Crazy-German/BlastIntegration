#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"


class TextComponent;
class VideoTexture;

class VideoComponent : public InterfaceComponent, public PostBox
{
public:
	VideoComponent();
	~VideoComponent();

	void Put(Letter aLetter) override;

	void Initialize(float aX, float aY);
	void Update(float aDeltaTime) override;

	void SetEventType(eLetterRegister aEventType);
	void SetEventData(void* aWhat);

	void ClickAction(const CU::Vector2f& aClickPosition = { 0, 0 }) override;

	void Draw() override;
protected:
	void* myWhat;
	eLetterRegister myEventType;

	float myClickTimer;
	float myClickTimerReset;

	float mySkipTextTimer;
	float mySkipTextTimerReset;

	bool myIsPaused;
	bool myShouldThisBePausable = false;
	bool myIsLoaded;

	Letter myFinishLetter;

	std::shared_ptr<VideoTexture> myVideo;
	std::shared_ptr<TextComponent> myText;

	void StopAndDestroy();
};