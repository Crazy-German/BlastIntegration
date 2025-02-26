#include "FeatherEngine.pch.h"
#include "SpriteAnimation.h"

SpriteAnimation::SpriteAnimation()
{
	mySprite = nullptr;
	myFrameSize = {};
	myAnimTimerReset = 0;
	myMaxFrames = 0;


	myFrame = 0;
	myAnimTimer = 0;
	myVerticalFrame = 0;
	myLoop = true;
	myIsPlaying = true;
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::Initialize(CU::Vector2<float> aPosition, std::shared_ptr<SpriteAsset> aSprite, CU::Vector2<float> aFrameSize, float aAnimTimer, int aMaxFrames, bool aLoop)
{
	Initialize(aPosition.x, aPosition.y, aSprite, aFrameSize.x, aFrameSize.y, aAnimTimer, aMaxFrames, aLoop);
}

void SpriteAnimation::Initialize(float aX, float aY, std::shared_ptr<SpriteAsset> aSprite, float aFrameWidth, float aFrameHeight, float aAnimTimer, int aMaxFrames, bool aLoop)
{
	mySprite = std::make_shared<Sprite2D>();
	mySprite->InitRelative(aX, aY, aFrameWidth, aFrameHeight, aSprite->myTexture);

	myFrameSize = { aFrameWidth, aFrameHeight };
	myAnimTimerReset = aAnimTimer;
	myLoop = aLoop;
	myMaxFrames = aMaxFrames;
}

void SpriteAnimation::Update(float aDeltaTime)
{
	if (myIsPlaying)
	{
		myAnimTimer += aDeltaTime;
		if (myAnimTimer > myAnimTimerReset)
		{
			myAnimTimer = 0.0f;
			myFrame++;
			if (myFrame > myMaxFrames)
			{
				if (!myLoop)
				{
					myIsPlaying = false;
				}
				myFrame = 0;
			}
			mySprite->SetSourceRectangle({ myFrameSize.x * myFrame, myFrameSize.y * myVerticalFrame }, myFrameSize);
		}
	}
}

void SpriteAnimation::Stop()
{
	myIsPlaying = false;
	myFrame = 0;
	myAnimTimer = 0;
	mySprite->SetSourceRectangle({ myFrameSize.x * myFrame, myFrameSize.y * myVerticalFrame }, myFrameSize);
}

void SpriteAnimation::Pause()
{
	myIsPlaying = false;
}

void SpriteAnimation::Play()
{
	myIsPlaying = true;
}

void SpriteAnimation::SetPositionRelative(const CU::Vector2<float>& aPosition)
{
	mySprite->SetPositionRelative(aPosition);
}

void SpriteAnimation::SetPositionRelative(const float aX, const float aY)
{
	mySprite->SetPositionRelative(aX, aY);
}

void SpriteAnimation::SetPosition(const CU::Vector2<float>& aPosition)
{
	mySprite->SetPosition(aPosition);
}

void SpriteAnimation::SetPosition(const float aX, const float aY)
{
	mySprite->SetPosition(aX, aY);
}

void SpriteAnimation::SetColor(const CU::Vector4f& aColor)
{
	mySprite->SetColor(aColor);
}

void SpriteAnimation::SetColor(const CU::Vector3f& aColor, const float aAlpha)
{
	mySprite->SetColor(aColor, aAlpha);
}

void SpriteAnimation::SetColor(const float aR, const float aG, const float aB, const float aA)
{
	mySprite->SetColor(aR, aG, aB, aA);
}

void SpriteAnimation::SetColor(const CU::Vector3f& aColor)
{
	mySprite->SetColor(aColor);
}

void SpriteAnimation::SetColor(const float aR, const float aG, const float aB)
{
	mySprite->SetColor(aR, aG, aB);
}

void SpriteAnimation::SetAlpha(const float aAlpha)
{
	mySprite->SetAlpha(aAlpha);
}

void SpriteAnimation::SetAnchor(const AnchorPoint aAnchor)
{
	mySprite->SetAnchor(aAnchor);
}

void SpriteAnimation::SetSourceRectangle(const CU::Vector2f& aStart, const CU::Vector2f& aSize)
{
	mySprite->SetSourceRectangle(aStart, aSize);
}

void SpriteAnimation::Draw()
{
	mySprite->Draw();
}
