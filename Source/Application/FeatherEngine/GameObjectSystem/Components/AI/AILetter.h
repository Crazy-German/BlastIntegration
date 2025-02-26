#pragma once
enum class eAILetterRegister
{
	AggroGroup,
	_Last
};

struct AILetter
{
	eAILetterRegister myType;
	void* myData;
};