#pragma once
#include "Letter.h"
#include "PostBox.h"
#include <vector>
#include <memory>

class GameObjectPostMaster
{
public:
	GameObjectPostMaster();
	~GameObjectPostMaster();
	static std::shared_ptr<GameObjectPostMaster> Get();
	void Register(eLetterRegister aRegister, PostBox* aPostBox);
	void UnRegister(eLetterRegister aRegister, PostBox& aPostBox);
	virtual void Send(Letter aLetter);
private:
	std::vector<std::vector<PostBox*>> myPostBoxes;
};