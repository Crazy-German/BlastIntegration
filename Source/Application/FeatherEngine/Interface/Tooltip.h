#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include <AssetManagement/AssetTypes/Sprite.h>
#include "InterfaceComponent.h"
#include "../GameObjectSystem/PostMaster/Letter.h"

class TextComponent;

constexpr auto DELIMITER = L"skibidi";

class Tooltip : public InterfaceComponent
{
public:
	Tooltip();
	~Tooltip();

	//To create line breaks, insert DELIMITER/skibidi at the desired position of the line break
	void Initialize(const std::wstring& aText, CU::Vector2f aPosition);

	void SetActive(const bool aState);
	const bool IsActive() const;
	void Draw() override;

protected:
	bool myTextIsSet = false;
	bool myShouldDraw = false;
	//std::shared_ptr<TextComponent> myText;

	unsigned myLineAmount;
	std::vector<std::shared_ptr<TextComponent>> myTextLines;
};