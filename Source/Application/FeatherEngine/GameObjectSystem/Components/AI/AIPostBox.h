#pragma once
#include "AILetter.h"

class AIPostBox
{
public:
	virtual void Put(AILetter aLetter) = 0;
private:
};