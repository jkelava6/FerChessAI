#pragma once

#include <Threading/ThreadInclude.h> // FCppThread
#include <cassert> // assert()

extern FCppThread::id MainThreadId;
#define THREAD_GUARD() assert(MainThreadId == FCppThread::id() || std::this_thread::get_id() == MainThreadId)
#define SET_THIS_THREAD_AS_MAIN() MainThreadId = std::this_thread::get_id();

namespace AiThreads
{
	extern int DefaultWaitTime;

	extern int InitializeThreadPool(int ThreadCount);
	extern int ThreadPoolCount();

	extern int QueueTaskBlocking(int PoolId, FunctionPointer(void, Task, void*), void* Arg = nullptr);
	extern bool AllTasksDone(int PoolId);
	extern void WaitForAllTasks(int PoolId);

	extern void Sleep(int Milliseconds);
}
