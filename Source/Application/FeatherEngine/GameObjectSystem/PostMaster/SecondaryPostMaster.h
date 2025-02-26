#pragma once
#include "Letter.h"
#include "PostBox.h"
#include <vector>
#include <memory>

class SecondaryPostMaster
{
public:
	SecondaryPostMaster();
	~SecondaryPostMaster();
	static std::shared_ptr<SecondaryPostMaster> Get();
	void Register(eLetterRegister aRegister, PostBox* aPostBox);
	void UnRegister(eLetterRegister aRegister, PostBox& aPostBox);
	virtual void Send(Letter aLetter);
private:
	std::vector<std::vector<PostBox*>> myPostBoxes;
};