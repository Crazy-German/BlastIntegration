#include "FeatherEngine.pch.h"
#include "GameObjectPostMaster.h"



GameObjectPostMaster::GameObjectPostMaster()
{
	/*if (myInstance != nullptr)
	{
		throw;
	}*/

	static const int last = static_cast<int>(eLetterRegister::_Last);
	for (int registerIndex = 0; registerIndex < last; registerIndex++)
	{
		myPostBoxes.push_back(std::vector<PostBox*>());
	}
}

GameObjectPostMaster::~GameObjectPostMaster()
{
}

std::shared_ptr<GameObjectPostMaster> GameObjectPostMaster::Get()
{
	static std::shared_ptr<GameObjectPostMaster> postMaster = std::make_shared<GameObjectPostMaster>();
	return postMaster;
}

void GameObjectPostMaster::Register(eLetterRegister aRegister, PostBox* aAIPostBox)
{
	myPostBoxes[static_cast<int>(aRegister)].push_back(aAIPostBox);
}

void GameObjectPostMaster::UnRegister(eLetterRegister aRegister, PostBox& aAIPostBox)
{
	PostBox* postBox = &aAIPostBox;
	std::vector<PostBox*>& workVector = myPostBoxes[static_cast<int>(aRegister)];
	for (int AIPostBoxIndex = 0; AIPostBoxIndex < workVector.size(); AIPostBoxIndex++)
	{
		if (workVector[AIPostBoxIndex] == postBox)
		{
			workVector[AIPostBoxIndex] = workVector[workVector.size() - 1];
			workVector.pop_back();
			return;
		}
	}
}

void GameObjectPostMaster::Send(Letter aLetter)
{
	std::vector<PostBox*>& workVector = myPostBoxes[static_cast<int>(aLetter.myType)];

	for (int mailBoxIndex = 0; mailBoxIndex < workVector.size(); mailBoxIndex++)
	{
		workVector[mailBoxIndex]->Put(aLetter);
	}
}

