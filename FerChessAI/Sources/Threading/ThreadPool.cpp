#include <StdH.h>
#include <Threading/ThreadPool.h>

#include <Threading/ThreadingUtils.h>

FThreadPool::FThreadPool() = default;
FThreadPool::~FThreadPool() = default;
IMPLEMENT_MOVE(FThreadPool);

static void ExecuteTaskWrapper(FThreadPool* Pool, int Index, FunctionPointer(void, Task, void*), void* Arg)
{
	Task(Arg);

	Pool->Busy[Index] = false;
}

void FThreadPool::ResizeBlocking(int Size)
{
	WaitUntilDone();

	for (int Index = 0; Index < Threads.Count(); ++Index)
	{
		while (Busy[Index])
		{
			ChessThreads::Sleep(ChessThreads::DefaultWaitTime);
		}
		if (Threads[Index].joinable())
		{
			Threads[Index].join();
		}
	}

	Threads.Clear();
	Threads.Prealocate(Size);
	for (int Index = 0; Index < Size; ++Index)
	{
		Threads.Push();
	}

	Busy.Clear();
	Busy.Prealocate(Size);
	for (int Index = 0; Index < Size; ++Index)
	{
		Busy.Push() = false;
	}
}

bool FThreadPool::AllThreadsFree()
{
	for (int Index = 0; Index < Threads.Count(); ++Index)
	{
		if (Busy[Index])
		{
			return false;
		}
	}

	return true;
}

void FThreadPool::WaitUntilDone()
{
	for (int Index = 0; Index < Threads.Count(); ++Index)
	{
		while (Busy[Index])
		{
			ChessThreads::Sleep(ChessThreads::DefaultWaitTime);
		}
	}
}

int FThreadPool::ExecuteTaskThreaded(FunctionPointer(void, Task, void*), void* Arg)
{
	for (int Index = 0; Index < Threads.Count(); ++Index)
	{
		if (Busy[Index])
		{
			continue;
		}
		Busy[Index] = true;

		if (Threads[Index].joinable())
		{
			Threads[Index].join();
		}
		Threads[Index] = FCppThread(ExecuteTaskWrapper, this, Index, Task, Arg);
		return Index;
	}

	return -1;
}
