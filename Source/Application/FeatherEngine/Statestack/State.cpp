#include "FeatherEngine.pch.h"
#include "State.h"

State::~State()
{
}

bool State::Update()
{
	return true;
}

void State::Render()
{
}

bool State::LetThroughRender()
{
	return false;
}
