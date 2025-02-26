#include "FeatherEngine.pch.h"
#include "SecondaryPostMaster.h"



SecondaryPostMaster::SecondaryPostMaster()
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

SecondaryPostMaster::~SecondaryPostMaster()
{
}

std::shared_ptr<SecondaryPostMaster> SecondaryPostMaster::Get()
{
	static std::shared_ptr<SecondaryPostMaster> postMaster = std::make_shared<SecondaryPostMaster>();
	return postMaster;
}

void SecondaryPostMaster::Register(eLetterRegister aRegister, PostBox* aPostBox)
{
	myPostBoxes[static_cast<int>(aRegister)].push_back(aPostBox);
}

void SecondaryPostMaster::UnRegister(eLetterRegister aRegister, PostBox& aPostBox)
{
	PostBox* postBox = &aPostBox;
	std::vector<PostBox*>& workVector = myPostBoxes[static_cast<int>(aRegister)];
	for (int postBoxIndex = 0; postBoxIndex < workVector.size(); postBoxIndex++)
	{
		if (workVector[postBoxIndex] == postBox)
		{
			workVector[postBoxIndex] = workVector[workVector.size() - 1];
			workVector.pop_back();
			return;
		}
	}
}

void SecondaryPostMaster::Send(Letter aLetter)
{
	std::vector<PostBox*>& workVector = myPostBoxes[static_cast<int>(aLetter.myType)];

	for (int mailBoxIndex = 0; mailBoxIndex < workVector.size(); mailBoxIndex++)
	{
		workVector[mailBoxIndex]->Put(aLetter);
	}
}

