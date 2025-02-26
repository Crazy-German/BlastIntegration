#pragma once
#include "GameObjectSystem/Components/AI/AIPostBox.h"
#include "GameObjectSystem/Components/AI/AILetter.h"
#include <vector>
#include <memory>

class AIPostMaster
{
public:
	AIPostMaster();
	~AIPostMaster();
	static std::shared_ptr<AIPostMaster> Get();
	void Register(eAILetterRegister aRegister, AIPostBox* aAIPostBox);
	void UnRegister(eAILetterRegister aRegister, AIPostBox& aAIPostBox);
	virtual void Send(AILetter aLetter);
private:
	std::vector<std::vector<AIPostBox*>> myAIPostBoxes;
};