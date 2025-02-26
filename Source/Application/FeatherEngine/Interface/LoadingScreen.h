#pragma once
#include "InterfaceComponent.h"

class SpriteInstance;
class TextComponent;

class LoadingScreen : public InterfaceComponent, public PostBox
{
public:
	LoadingScreen();
	~LoadingScreen();

	void Put(Letter aLetter);

	void Initialize();
	void Update(float aDeltaTime);
	void Start();
	void Stop();
	void Fade();

	void Draw();

private:
	std::shared_ptr<SpriteInstance> myBackground;
	std::vector<std::shared_ptr<SpriteInstance>> myMasksBurning;
	std::shared_ptr<TextComponent> myLoadingText;

	int myLastText;
	unsigned myLevelIndex = 0;
	float myTextHeight = 0.8f;

	bool myIsLoading;
	bool myIsFadingOut;

	float myTimer;
};