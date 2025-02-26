#pragma once
#include <atomic>
#include <mutex>

#include "../Graphics/GraphicsEngine/GraphicsCommandList.h"

constexpr unsigned MAX_RENDERED_OBJECTS = 2048;
#define RENDERER MainSingleton::Get().GetRenderer();

class Renderer
{
public:
	Renderer();
	~Renderer();

	template<typename CommandClass, typename ...Args>
	void Enqueue(Args&&... args)
	{
		myUpdatePtr->Enqueue<CommandClass>(std::forward<Args>(args)...);
	}


	void SwitchUpdateIntermediate(); // When Update is done
	void SwitchRenderIntermediate(); // When render is done
	void RenderFrame();
private:
	GraphicsCommandList myFirstCmdList;
	GraphicsCommandList mySecondCmdList;
	GraphicsCommandList myThirdCmdList;

	GraphicsCommandList* myUpdatePtr;
	GraphicsCommandList* myIntermediatePtr;
	GraphicsCommandList* myRenderPtr;

	std::atomic<bool> myHasUpdated = false;
	std::mutex myBufferLock;
};