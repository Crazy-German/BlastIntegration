#pragma once

class StateStack;

class State // Inherit from this to other states
{
public:
	virtual ~State();
	virtual bool Update();
	virtual bool LetThroughRender();
	virtual void Render();
protected:

};