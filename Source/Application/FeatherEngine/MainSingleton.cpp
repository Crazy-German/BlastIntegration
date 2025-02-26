#include "FeatherEngine.pch.h"
#include "MainSingleton.h"

#include "GraphicsEngine/MiscBuffer.h"


void MainSingleton::Awake()
{
	//myAudioEngine.Awake();
	mySceneManager.Awake();
	
}

MainSingleton& MainSingleton::Get()
{
	static MainSingleton myInstance;
	return myInstance;
}

void MainSingleton::Update(float aDeltaTime)
{
	//myAudioEngine.Update();
	//AudioEngine::Get().Update()
	mySceneManager.UpdateScene(aDeltaTime);
}

MiscBuffer* MainSingleton::GetMiscBuffer()
{
	return myMiscBuffer;
}

void MainSingleton::InitMiscBuffer(MiscBuffer* aBuffer)
{
	myMiscBuffer = aBuffer;
}
