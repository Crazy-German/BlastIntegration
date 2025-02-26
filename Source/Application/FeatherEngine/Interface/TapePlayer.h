#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"
#include "Subtitles.h"
#include "SpriteAnimation.h"
#include "Button2D.h"

class TapePlayer : public InterfaceComponent, PostBox
{
public:
	TapePlayer();
	~TapePlayer();

	void Put(Letter aLetter) override;

	void Initialize();
	void Update(float aDeltaTime);
	void Draw() override;

	void PlaySubtitles(std::string& aFilename);
	const float GetLength() const;

	void SetInventoryContent(std::array<std::shared_ptr<Button2D>, 4> aInventoryButtons, int offset);
	const int GetInventorySize() const;

protected:
	std::shared_ptr<Subtitles> ReadSubtitleFile(std::string& aFilename);

	bool myIsPlayingTape = false;
	float myTapeLength = 0.0f;
	float myTapeElapsed = 0.0f;
	std::shared_ptr<SpriteAnimation> myTape;
	float myTapeDeactivationTimer = 0.0f;
	const float myTapeDeactivationTimerReset = 1.5f;

	std::vector<std::string> myUnlockedTapes;

	std::shared_ptr<Subtitles> myCurrentSubtitles;
	std::string myLastTapePlayed = "none";

	std::unordered_map<std::string, std::shared_ptr<Subtitles>> myCachedTapes;
};