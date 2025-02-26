#pragma once
#include "GraphicsEngine/Sprite2D.h"
#include <array>
#include "InterfaceComponent.h"
#include <GraphicsEngine/Text/Text.h>

class TextComponent : public InterfaceComponent
{
public:
	TextComponent();
	~TextComponent();

	void InitRelative(CU::Vector2<float> aPosition, float aScale, std::shared_ptr<Font> aFont,		const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY,			float aScale, std::shared_ptr<Font> aFont,		const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(CU::Vector2<float> aPosition, float aScale, std::filesystem::path& aFontPath,	const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitRelative(float aX, float aY,			float aScale, std::filesystem::path& aFontPath,	const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
																										   
	void InitAbsolute(CU::Vector2<float> aPosition, float aScale, std::shared_ptr<Font> aFont,		const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(float aX, float aY,			float aScale, std::shared_ptr<Font> aFont,		const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(CU::Vector2<float> aPosition, float aScale, std::filesystem::path& aFontPath,	const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);
	void InitAbsolute(float aX, float aY,			float aScale, std::filesystem::path& aFontPath,	const std::wstring& aString, SAnchorPoint aAnchor = SAnchorPoint::Center);

	void SetPositionRelative(const CU::Vector2<float>& aPosition);
	void SetPositionRelative(const float aX, const float aY);

	void SetPositionAbsolute(const CU::Vector2<float>& aPosition);
	void SetPositionAbsolute(const float aX, const float aY);

	void SetScale(const float aScale);

	const float GetTextWidth() const;

	void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor);
	void SetColor(const float aR, const float aG, const float aB);
	void SetColor(const float aR, const float aG, const float aB, const float aA);

	/*void SetColor(const CU::Vector4f& aColor);
	void SetColor(const CU::Vector3f& aColor, const float aAlpha);
	void SetColor(const float aR, const float aG, const float aB, const float aA);

	void SetColor(const CU::Vector3f& aColor);
	void SetColor(const float aR, const float aG, const float aB);
	void SetAlpha(const float aAlpha);*/

	void SetText(const std::wstring& aText);

	void SetAnchor(const SAnchorPoint aAnchor);

	void Draw() override;
protected:
	
	float myScale;
	CU::Vector2f myPosition;
	std::wstring myString;
	SAnchorPoint myAnchor;

	std::shared_ptr<Text> myText;
};