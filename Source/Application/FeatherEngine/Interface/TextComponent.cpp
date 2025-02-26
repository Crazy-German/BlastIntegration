#include "FeatherEngine.pch.h"
#include "TextComponent.h"
#include <AssetManagement/AssetManager.h>
#include <AssetManagement/AssetTypes/FontAsset.h>
#include <GraphicsEngine/Text/TextFactory.h>
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_RenderText.h>
#include <MainSingleton.h>

TextComponent::TextComponent()
{
	myText = std::make_shared<Text>();
	myScale = 1.0f;
	myAnchor = SAnchorPoint::Center;
}

TextComponent::~TextComponent()
{
	myText = nullptr;
}

void TextComponent::InitRelative(CU::Vector2<float> aPosition, float aScale, std::shared_ptr<Font> aFont, const std::wstring& aString, SAnchorPoint aAnchor)
{
	InitRelative(aPosition.x, aPosition.y, aScale, aFont, aString, aAnchor);
}

void TextComponent::InitRelative(float aX, float aY, float aScale, std::shared_ptr<Font> aFont, const std::wstring& aString, SAnchorPoint aAnchor)
{
	auto resolution	= GetViewportSize(GraphicsEngine::Get().GetViewPort());
	CU::Vector2f floatResolution = { static_cast<float>(resolution.x), static_cast<float>(resolution.y) };
	InitAbsolute(aX * floatResolution.x, aY * floatResolution.y, aScale, aFont, aString, aAnchor);
} 

void TextComponent::InitRelative(CU::Vector2<float> aPosition, float aScale, std::filesystem::path& aFontPath, const std::wstring& aString, SAnchorPoint aAnchor)
{
	AssetManager::Get().RegisterAsset(aFontPath);
	auto font = AssetManager::Get().GetAsset<FontAsset>(aFontPath.c_str());

	InitRelative(aPosition.x, aPosition.y, aScale, font->myFont, aString, aAnchor);
}

void TextComponent::InitRelative(float aX, float aY, float aScale, std::filesystem::path& aFontPath, const std::wstring& aString, SAnchorPoint aAnchor)
{
	InitRelative({ aX, aY }, aScale, aFontPath, aString, aAnchor);
}

void TextComponent::InitAbsolute(CU::Vector2<float> aPosition, float aScale, std::shared_ptr<Font> aFont, const std::wstring& aString, SAnchorPoint aAnchor)
{
	InitAbsolute(aPosition.x, aPosition.y, aScale, aFont, aString, aAnchor);
}

void TextComponent::InitAbsolute(float aX, float aY, float aScale, std::shared_ptr<Font> aFont, const std::wstring& aString, SAnchorPoint aAnchor)
{
	myText = nullptr;
	myText = TextFactory::GetInstance()->CreateText(aString, 1, aFont);
	myText->SetScale({ aScale*0.425f, aScale * 0.425f, aScale * 0.425f });

	float width = myText->GetWidth();

	float verticalOffset = 5.0f;

	if (aAnchor == SAnchorPoint::TopLeft || aAnchor == SAnchorPoint::Left || aAnchor == SAnchorPoint::BottomLeft)
	{
		myText->SetPosition({ aX, aY + verticalOffset * aScale, 0 });
	} 
	else if (aAnchor == SAnchorPoint::Top || aAnchor == SAnchorPoint::Center || aAnchor == SAnchorPoint::Bottom)
	{
		myText->SetPosition({ aX - (0.5f * width), aY + verticalOffset * aScale, 0 });

	}
	else if (aAnchor == SAnchorPoint::TopRight || aAnchor == SAnchorPoint::Right || aAnchor == SAnchorPoint::BottomRight)
	{
		myText->SetPosition({ aX - width, aY + verticalOffset * aScale, 0 });

	}

	myPosition = CU::Vector2f(aX, aY);
	myScale = aScale;
	myString = aString;
}

void TextComponent::InitAbsolute(CU::Vector2<float> aPosition, float aScale, std::filesystem::path& aFontPath, const std::wstring& aString, SAnchorPoint aAnchor)
{
	AssetManager::Get().RegisterAsset(aFontPath);
	auto font = AssetManager::Get().GetAsset<FontAsset>(aFontPath.c_str());

	InitAbsolute(aPosition.x, aPosition.y, aScale, font->myFont, aString, aAnchor);
}

//void TextComponent::InitAbsolute(float aX, float aY, float aScale, std::filesystem::path& aFontPath, const std::wstring& aString, SAnchorPoint aAnchor)
//{
//	InitAbsolute(aX, aY, aScale, aFontPath, aString, aAnchor);
//}

void TextComponent::SetPositionRelative(const CU::Vector2<float>& aPosition)
{
	SetPositionRelative(aPosition.x, aPosition.y);
}

void TextComponent::SetPositionRelative(const float aX, const float aY)
{
	auto resolution = GetViewportSize(GraphicsEngine::Get().GetViewPort());
	CU::Vector2f floatResolution = { static_cast<float>(resolution.x), static_cast<float>(resolution.y) };
	myText->SetPosition({ aX * floatResolution.x, aY * floatResolution.y, 0 });

	myPosition = CU::Vector2f(aX * floatResolution.x, aY * floatResolution.y);
}

void TextComponent::SetPositionAbsolute(const CU::Vector2<float>& aPosition)
{
	SetPositionAbsolute(aPosition.x, aPosition.y);
}

void TextComponent::SetPositionAbsolute(const float aX, const float aY)
{
	myText->SetPosition({ aX, aY, 0 });

	myPosition = CU::Vector2f(aX, aY);
}

void TextComponent::SetScale(const float aScale)
{
	myText->SetScale({ aScale, aScale, aScale });
	myScale = aScale;
}

const float TextComponent::GetTextWidth() const
{
	return myText->GetWidth();
}

void TextComponent::SetColor(const CU::Vector4f& aColor)
{
	myText->SetColor(aColor);
}

void TextComponent::SetColor(const CU::Vector3f& aColor)
{
	SetColor({ aColor.x, aColor.y, aColor.z, 1 });
}

void TextComponent::SetColor(const float aR, const float aG, const float aB)
{
	SetColor({ aR, aG, aB });
}

void TextComponent::SetColor(const float aR, const float aG, const float aB, const float aA)
{
	SetColor({ aR, aG, aB, aA });
}


void TextComponent::SetText(const std::wstring& aText)
{
	myString = aText;
	const Font& fontCart = myText->GetFont();
	myText = nullptr;
	myText = TextFactory::GetInstance()->CreateText(myString, 1, std::make_shared<Font>(fontCart));
}

void TextComponent::SetAnchor(const SAnchorPoint aAnchor)
{
	aAnchor;

	//Change position based on text size
}

void TextComponent::Draw()
{
	//Needs to call renderer instead
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_RenderText>(myText);
	//darn it
}
