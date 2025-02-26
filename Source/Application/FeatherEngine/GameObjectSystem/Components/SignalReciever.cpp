#include "FeatherEngine.pch.h"
#include "SignalReciever.h"
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>


SignalReciever::SignalReciever()
{
	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingLevel, this);
}

void SignalReciever::Put(Letter aLetter)
{
	if (aLetter.myType == eLetterRegister::Scream)
	{
		ScreamStruct scream = *static_cast<ScreamStruct*>(aLetter.myData);
		CU::Vector3f myPos = myGameObject->GetTransform()->GetPosition();
		CU::Vector3f netPos = myPos - scream.myPosition;

		if(netPos.LengthSqr() < scream.myRange*scream.myRange)
		{
			InternalMSG activationMSG;
			activationMSG.myEvent = InternalEvent::eScreamReached;
			myGameObject->SendInternal(activationMSG);
		}
	}
}

