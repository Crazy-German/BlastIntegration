#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>

class SpriteAnimation
{
public:
	SpriteAnimation();
	~SpriteAnimation();

	void Initialize(CU::Vector2<float> aPosition, std::shared_ptr<SpriteAsset> aSprite, CU::Vector2<float> aFrameSize, float aAnimTimer, int aMaxFrames, bool aLoop = true);
	void Initialize(float aX, float aY, std::shared_ptr<SpriteAsset> aSprite, float aFrameWidth, float aFrameHeight, float aAnimTimer, int aMaxFrames, bool aLoop = true);

	void Update(float aDeltaTime);

	void Stop();
	void Pause();
	void Play();


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

	void SetAnchor(const AnchorPoint aAnchor);

	void SetSourceRectangle(const CU::Vector2f& aStart, const CU::Vector2f& aSize);


	void Draw();

private:
	std::shared_ptr<Sprite2D> mySprite;
	std::shared_ptr<SpriteAsset> mySpriteAsset;
	//spriteinstance

	CU::Vector2<float> myFrameSize;

	float myAnimTimer;
	float myAnimTimerReset;

	int myFrame;
	int myMaxFrames;

	int myVerticalFrame; //?

	bool myLoop;
	bool myIsPlaying;
};