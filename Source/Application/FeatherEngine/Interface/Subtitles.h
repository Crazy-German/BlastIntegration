#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

class TextComponent;

struct SubtitleLine
{
	float myKey;
	std::string myText;
};



//constexpr auto DELIMITER = L"skibidi";

class Subtitles : public InterfaceComponent
{
public:
	Subtitles();
	~Subtitles();

	void Initialize();
	void Update(float aDeltaTime);
	void Draw() override;

	void PlaySubtitles();
	void PrepareSubtitles();
	const float GetLength() const;

	//TEMP
	//void WriteSubtitleFile();


protected:
	//void ReadSubtitleFile(std::string& aFilename);
	float myDuration;
	float myElapsed;

	unsigned myCurrentIndex = 0;

	std::vector<SubtitleLine> myTextLines;
	std::vector<std::shared_ptr<TextComponent>> myTextComponents;
	//std::vector<std::shared_ptr<TextComponent>> myTextLines;

	friend class TapePlayer;
};