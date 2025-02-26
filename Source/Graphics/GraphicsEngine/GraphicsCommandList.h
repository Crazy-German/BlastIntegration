#pragma once
#include <functional>
#include <stdexcept>

struct GraphicsCommandBase
{
	virtual ~GraphicsCommandBase() = default;
	virtual void Execute() = 0;
	virtual void Destroy() = 0;

	GraphicsCommandBase* Next = nullptr;
};

using GraphicsCommandFunction = std::function<void()>;

struct LambdaGraphicsCommand  final : public GraphicsCommandBase
{
	GraphicsCommandFunction myLambda;

	LambdaGraphicsCommand(GraphicsCommandFunction&& aLambda)
		: myLambda(std::move(aLambda))
	{
		
	}

	void Execute() override
	{
		myLambda();
	}
	void Destroy() override
	{
		
	}
};

class GraphicsCommandList final
{
public:
	friend class Renderer;
	GraphicsCommandList(size_t aMemorySize = 1048576);
	~GraphicsCommandList();

	//static GraphicsCommandList& Get();
	GraphicsCommandList(const GraphicsCommandList&) = delete;
	GraphicsCommandList(GraphicsCommandList&&) noexcept = delete;
	GraphicsCommandList& operator=(GraphicsCommandList&&) noexcept = delete;
	GraphicsCommandList& operator=(const GraphicsCommandList&) = delete;

	void Execute();
	void Reset();

	__forceinline bool IsFinished() const { return !isExecuteing && isFinished; }
	__forceinline bool IsExecuting() const { return isExecuteing; }
	__forceinline bool HasCommands() const { return myNumCommands != 0; }

	template<typename CommandClass, typename ...Args>
	std::enable_if_t<std::is_base_of_v<GraphicsCommandBase, CommandClass>>
		Enqueue(Args... args)
	{
		const size_t commandSize = sizeof(CommandClass);
		if (myCursor + commandSize > mySize)
		{
			throw std::out_of_range("Graphics Command List ran out of memory. Consider increasing its size");
		}

		GraphicsCommandBase* ptr = reinterpret_cast<GraphicsCommandBase*>(myData + myCursor);
		myCursor += commandSize;
		::new(ptr) CommandClass(std::forward<Args>(args)...);
		*myLink = ptr;
		myLink = &ptr->Next;
		myNumCommands++;
	}

	void Enqueue(GraphicsCommandFunction&& aLambda)
	{
		if (aLambda)
		{
			Enqueue<LambdaGraphicsCommand>(std::move(aLambda));
		}
	}

private:
	friend class GraphicsCommandListIterator;

	size_t mySize = 0;
	size_t myCursor = 0;
	size_t myNumCommands = 0;

	uint8_t* myData = nullptr;
	GraphicsCommandBase* myRoot = nullptr;
	GraphicsCommandBase** myLink = nullptr;

	bool isExecuteing = false;
	bool isFinished = false;

};

class GraphicsCommandListIterator
{
public:
	GraphicsCommandListIterator(const GraphicsCommandList* aCommandList);

	GraphicsCommandBase* Next();

	__forceinline operator bool() const
	{
		return !!myPtr;
	}

private:
	GraphicsCommandBase* myPtr = nullptr;

};

