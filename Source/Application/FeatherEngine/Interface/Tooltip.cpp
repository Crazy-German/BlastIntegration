#include "FeatherEngine.pch.h"
#include "SpriteInstance.h"
#include <AssetManagement/AssetManager.h>
#include "Tooltip.h"
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include "TextComponent.h"
#include <AssetManagement/AssetTypes/FontAsset.h>
#include "../GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include <GraphicsEngine/Text/TextFactory.h>

Tooltip::Tooltip()
{

}

Tooltip::~Tooltip()
{
	for (size_t i = 0; i < myTextLines.size(); i++)
	{
		myTextLines[i] = nullptr;
	}
}

void Tooltip::Initialize(const std::wstring& aText, CU::Vector2f aPosition)
{
	std::wstring tempText = aText;
	myLineAmount = 1;

	std::vector<std::wstring> textLines;
	size_t readPos;
	while ((readPos = tempText.find(DELIMITER)) != std::wstring::npos)
	{
		std::wstring token = tempText.substr(0, readPos);
		textLines.push_back(token);
		tempText.erase(0, readPos + std::wstring(DELIMITER).length());
		myLineAmount++;
	}

	float tooltipWidth = 0.0f;
	float firstLineVerticalOffset = 0.0f;
	for (unsigned i = 0; i < myLineAmount - 1; i++)
	{
		float horizontalOffset = 8.0f;
		if (i > 0)
		{
			horizontalOffset = 2.0f;
			firstLineVerticalOffset = 4.0f;
		}
		std::shared_ptr<TextComponent> text = std::make_shared<TextComponent>();
		text->InitAbsolute(aPosition + CU::Vector2f(horizontalOffset, 18.0f + -24.0f * static_cast<float>(myLineAmount - i) + firstLineVerticalOffset), 1.0f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, textLines[i], SAnchorPoint::Left);
		myTextLines.push_back(text);
		float width = TextFactory::GetInstance()->GetWidthOfText(textLines[i], AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont) * 0.5f;
		if (width > tooltipWidth)
		{
			tooltipWidth = width + 16.0f;
		}
	}

	mySprite = std::make_shared<Sprite2D>();
	mySprite->InitAbsolute(aPosition.x, aPosition.y, tooltipWidth, 36.0f * (myLineAmount - 1), AssetManager::Get().GetAsset<SpriteAsset>("sprite_void")->myTexture, AnchorPoint::BottomLeft);
	mySprite->SetAlpha(0.75f);
}

void Tooltip::SetActive(const bool aState)
{
	myShouldDraw = aState;
}

const bool Tooltip::IsActive() const
{
	return myShouldDraw;
}

void Tooltip::Draw()
{
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	//GraphicsCommandList::Get().Enqueue<GfxCmd_SetTextureResource>(PIPELINE_STAGE_PIXEL_SHADER, 0, mySprite->GetTexture());
	mySprite->Draw();


	for (int i = 0; i < myTextLines.size(); i++)
	{
		myTextLines[i]->Draw();
	}
	//if (myText)
	//{
		//myText->Draw();
	//}
}
