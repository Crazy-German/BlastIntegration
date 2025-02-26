#include "FeatherEngine.pch.h"
#include "Renderer.h"

Renderer::Renderer()
{
	myUpdatePtr = &myFirstCmdList;
	myIntermediatePtr = &mySecondCmdList;
	myRenderPtr = &myThirdCmdList;
}

Renderer::~Renderer()
{
	myUpdatePtr = nullptr;
	myIntermediatePtr = nullptr;
	myRenderPtr = nullptr;
}


void Renderer::SwitchRenderIntermediate()
{
	myBufferLock.lock();
	GraphicsCommandList* tempIntermediate = myRenderPtr;
	myRenderPtr = myIntermediatePtr;
	myIntermediatePtr = tempIntermediate;
	myHasUpdated = false;
	myBufferLock.unlock();
}

void Renderer::SwitchUpdateIntermediate()
{
	myBufferLock.lock();
	GraphicsCommandList* tempIntermediate = myUpdatePtr;
	myUpdatePtr = myIntermediatePtr;
	myIntermediatePtr = tempIntermediate;
	myUpdatePtr->Reset();
	myHasUpdated = true;
	myBufferLock.unlock();
}

void Renderer::RenderFrame()
{

	while (!myHasUpdated)
	{

	}
	SwitchRenderIntermediate();

	if (myRenderPtr->HasCommands())
	{
		myRenderPtr->Execute();
	}
	myRenderPtr->Reset();
}
