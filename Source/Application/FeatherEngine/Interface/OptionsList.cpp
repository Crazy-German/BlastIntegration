#include "OptionsList.h"
#include "EngineSettings.h"
#include <GraphicsEngine/GraphicsCommandList.h>
#include <GraphicsEngine/GfxCmd_ChangePipelineState.h>
#include <GraphicsEngine/GraphicsEngine.h>
#include <GraphicsEngine/GfxCmd_SetTextureResource.h>
#include "TextComponent.h"
#include "Tooltip.h"
#include <CommonUtilities/Input.h>
#include <AssetManagement/AssetManager.h>
#include <AssetManagement/AssetTypes/FontAsset.h>
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>

OptionsList::OptionsList()
{
}

OptionsList::~OptionsList()
{
	/*for (int i = 0; i < myData->size(); i++)
	{
		delete myData->at(i);
	}

	delete myData;*/
}

void OptionsList::InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<SpriteAsset> aSprite, SAnchorPoint aAnchor)
{
	mySprite = std::make_shared<Sprite2D>();
	mySelectSprite = std::make_shared<Sprite2D>();
	auto ssprite = AssetManager::Get().GetAsset<SpriteAsset>("sprite_options_list_select");

	mySprite->InitRelative(aX, aY, aWidth, aHeight, aSprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	mySelectSprite->InitRelative(aX, aY, aWidth, aHeight, ssprite->myTexture, static_cast<AnchorPoint>(aAnchor));
	myText = std::make_shared<TextComponent>();
	myX = aX;
	myY = aY;
	myText->InitRelative(aX, aY, 1.25f, AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, std::wstring(L"resolution lol"));
}

void OptionsList::Update(float aDeltaTime)
{
	if (myRemainingInputCooldown > 0)	
	{
		myRemainingInputCooldown -= aDeltaTime;
	}
}

void OptionsList::ClickAction(const CU::Vector2f& aClickPosition)
{
	if (myRemainingInputCooldown <= 0.0f)
	{
		myRemainingInputCooldown = myInputCooldown;
		CU::Vector2f mousePos = GetRelativePosition(aClickPosition);

		if (mousePos.x < 64 * EngineSettings::Get().GetResolutionScale())
		{
			myIndex--;
		}

		if (mousePos.x > (512 - 64) * EngineSettings::Get().GetResolutionScale())
		{
			myIndex++;
		}


		if (myIndex < 0)
		{
			myIndex = (int)myData->size() - 1;
		}

		if (myIndex >= myData->size())
		{
			myIndex = 0;
		}

		SetDisplayText();
		GameObjectPostMaster::Get()->Send({ eLetterRegister::SetResolution, myData->at(myIndex) });

		
		//send event
	}
	
}

void OptionsList::SetEventType(eLetterRegister aEventType)
{
	myEventType = aEventType;
}

void OptionsList::HorizontalMoveActionStep(int aHorizontalMove)
{
	if (aHorizontalMove < 0)
	{
		myIndex--;
	}

	if (aHorizontalMove > 0)
	{
		myIndex++;
	}


	if (myIndex < 0)
	{
		myIndex = (int)myData->size() - 1;
	}

	if (myIndex >= myData->size())
	{
		myIndex = 0;
	}

	SetDisplayText();
	GameObjectPostMaster::Get()->Send({ eLetterRegister::SetResolution, myData->at(myIndex) });
}

void OptionsList::SetData(std::vector<Resolution*>* aData)
{
	myData = aData;
}

void OptionsList::SetCurrentOption(const unsigned aIndex)
{
	myIndex = aIndex;
	SetDisplayText();
}

void OptionsList::Draw()
{
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_ChangePipelineState>(GraphicsEngine::Get().GetSpritePSO());
	//GraphicsCommandList::Get().Enqueue<GfxCmd_SetTextureResource>(PIPELINE_STAGE_PIXEL_SHADER, 0, mySprite->GetTexture());

	if (myInteractionState == InteractionState::Selected)
	{
		mySelectSprite->Draw();
	}
	else
	{
		mySprite->Draw();
	}

	if (myText)
	{
		myText->Draw();
	}

	if (myHasTooltip)
	{
		if (myTooltip->IsActive())
		{
			myTooltip->Draw();
		}
	}
}

void OptionsList::SetDisplayText()
{
	std::wstring resWidth = std::to_wstring(myData->at(myIndex)->width);
	std::wstring resHeight = std::to_wstring(myData->at(myIndex)->height);
	std::wstring text = resWidth + std::wstring(L" x ") + resHeight;

	myText->InitRelative(myX, myY, 1.25f * EngineSettings::Get().GetResolutionScale(), AssetManager::Get().GetAsset<FontAsset>("F_ModernAntiqua")->myFont, text);
}

CU::Vector2f OptionsList::GetRelativePosition(const CU::Vector2f aMousePos)
{
	CU::Vector2f topLeft = { mySprite->GetLeft(), mySprite->GetTop() }; // 679, 464
	CU::Vector2f mousePos = aMousePos; //792, 496

	return aMousePos - topLeft;
}
