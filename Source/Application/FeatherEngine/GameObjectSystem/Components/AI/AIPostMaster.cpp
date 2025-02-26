#include "FeatherEngine.pch.h"
#include "AIPostMaster.h"



AIPostMaster::AIPostMaster()
{
	/*if (myInstance != nullptr)
	{
		throw;
	}*/

	static const int last = static_cast<int>(eAILetterRegister::_Last);
	for (int registerIndex = 0; registerIndex < last; registerIndex++)
	{
		myAIPostBoxes.push_back(std::vector<AIPostBox*>());
	}
}

AIPostMaster::~AIPostMaster()
{
}

std::shared_ptr<AIPostMaster> AIPostMaster::Get()
{
	static std::shared_ptr<AIPostMaster> postMaster = std::make_shared<AIPostMaster>();
	return postMaster;
}

void AIPostMaster::Register(eAILetterRegister aRegister, AIPostBox* aAIPostBox)
{
	myAIPostBoxes[static_cast<int>(aRegister)].push_back(aAIPostBox);
}

void AIPostMaster::UnRegister(eAILetterRegister aRegister, AIPostBox& aAIPostBox)
{
	AIPostBox* aiPostBox = &aAIPostBox;
	std::vector<AIPostBox*>& workVector = myAIPostBoxes[static_cast<int>(aRegister)];
	for (int AIPostBoxIndex = 0; AIPostBoxIndex < workVector.size(); AIPostBoxIndex++)
	{
		if (workVector[AIPostBoxIndex] == aiPostBox)
		{
			workVector[AIPostBoxIndex] = workVector[workVector.size() - 1];
			workVector.pop_back();
			return;
		}
	}
}

void AIPostMaster::Send(AILetter aLetter)
{
	std::vector<AIPostBox*>& workVector = myAIPostBoxes[static_cast<int>(aLetter.myType)];

	for (int mailBoxIndex = 0; mailBoxIndex < workVector.size(); mailBoxIndex++)
	{
		workVector[mailBoxIndex]->Put(aLetter);
	}
}

