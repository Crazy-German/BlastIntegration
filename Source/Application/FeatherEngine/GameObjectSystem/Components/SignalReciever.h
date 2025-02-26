#pragma once
#include "CommonUtilities/InputObserver.h"
#include "Component.h"

#define SIGNALRECIEVER 100
class SignalReciever : public Component, public InputObserver  // <---- This class is what you are looking for
{
public:
	SignalReciever();

	void Put(Letter aLetter) override;
};

