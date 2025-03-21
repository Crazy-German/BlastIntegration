#include "GraphicsEngine.pch.h"
#include "GraphicsCommandList.h"


GraphicsCommandList::GraphicsCommandList(size_t aMemorySize)
	: mySize(aMemorySize)
	, myData(new uint8_t[mySize])
	, myRoot(reinterpret_cast<GraphicsCommandBase*>(myData))
	, myLink(&myRoot)
{
	memset(myData, 0, mySize);
}

GraphicsCommandList::~GraphicsCommandList()
{
	Reset();

	delete[]  myData;
	myData = nullptr;
	myRoot = nullptr;
	myLink = nullptr;
}

//GraphicsCommandList& GraphicsCommandList::Get()
//{
//	static GraphicsCommandList myInstance;
//	return myInstance;
//}

void GraphicsCommandList::Execute()
{
	if(!isExecuteing && !isFinished)
	{
		isExecuteing = true;
		GraphicsCommandListIterator it(this);

		while(it)
		{
			GraphicsCommandBase* cmd = it.Next();
			cmd->Execute();
 		}

		isFinished = true;
		isExecuteing = false;
	}
}

void GraphicsCommandList::Reset()
{
	if(isExecuteing)
	{
		throw std::runtime_error("Cannot reset a command list while it  is executing");
	}

	if(myNumCommands > 0)
	{
		if(isFinished)
		{
			GraphicsCommandListIterator it(this);
			while(it)
			{
				GraphicsCommandBase* cmd = it.Next();
				cmd->Destroy();
			}
		}

		memset(myData, 0, mySize);
		myRoot = reinterpret_cast<GraphicsCommandBase*>(myData);
		myLink = &myRoot;
		myCursor = 0;
		myNumCommands = 0;
	}
	isExecuteing = false;
	isFinished = false;
}

GraphicsCommandListIterator::GraphicsCommandListIterator(const GraphicsCommandList* aCommandList)
{
	myPtr = aCommandList->myRoot;
}

GraphicsCommandBase* GraphicsCommandListIterator::Next()
{
	GraphicsCommandBase* cmd = myPtr;
	myPtr = cmd->Next;
	return cmd;
}

