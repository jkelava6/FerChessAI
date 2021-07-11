#include <StdH.h>
#include <Threading/ThreadingUtils.h>

#include <Array.h>
#include <Threading/ThreadPool.h>

static TArray<FThreadPool> ThreadPools;
extern FCppThread::id MainThreadId = FCppThread::id();

namespace AiThreads
{
	extern int DefaultWaitTime = 100;

	extern int InitializeThreadPool(int ThreadCount)
	{
		int Index = ThreadPools.Count();
		FThreadPool& Pool = ThreadPools.Push();
		Pool.ResizeBlocking(ThreadCount);
		return Index;
	}

	extern int ThreadPoolCount()
	{
		return ThreadPools.Count();
	}

	extern int QueueTaskBlocking(int PoolId, FunctionPointer(void, Task, void*), void* Arg/* = nullptr*/)
	{
		FThreadPool& Pool = ThreadPools[PoolId];
		int Index = Pool.ExecuteTaskThreaded(Task, Arg);
		while (Index == -1)
		{
			Sleep(DefaultWaitTime);
			Index = Pool.ExecuteTaskThreaded(Task, Arg);
		}
		return Index;
	}

	extern bool AllTasksDone(int PoolId)
	{
		FThreadPool& Pool = ThreadPools[PoolId];
		return Pool.AllThreadsFree();
	}

	extern void WaitForAllTasks(int PoolId)
	{
		FThreadPool& Pool = ThreadPools[PoolId];
		Pool.WaitUntilDone();
	}

	extern void Sleep(int Milliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(Milliseconds));
	}
}
