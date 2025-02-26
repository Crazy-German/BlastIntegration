#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>

class SplashScreen
{
public:
	SplashScreen();
	~SplashScreen();

	void Initialize();
	bool Run(float aDeltaTime);

	void Draw();

private:
	std::shared_ptr<Sprite2D> myBackground;
	std::shared_ptr<Sprite2D> myTgaLogo;
	std::shared_ptr<Sprite2D> myGroupLogo;
	std::shared_ptr<Sprite2D> myFmodLogo;

	std::array<float, 7> myThresholds;

	float myTimer;

	float lerp(float v0, float v1, float t);
};