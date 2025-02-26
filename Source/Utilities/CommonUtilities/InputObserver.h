#pragma once
#include "ActionEvent.h"
class InputObserver
{
public:
	virtual ~InputObserver() = default;
	virtual void RecieveEvent(const ActionEvent& anEvent) = 0;
};
