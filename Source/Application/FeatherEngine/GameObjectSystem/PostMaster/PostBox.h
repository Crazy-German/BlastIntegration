#pragma once
#include "Letter.h"
class PostBox
{
public:
	virtual void Put(Letter aLetter) = 0;
private:
};