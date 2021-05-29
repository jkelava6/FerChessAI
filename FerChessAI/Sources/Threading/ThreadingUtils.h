#pragma once

namespace ChessThreads
{
	extern int DefaultWaitTime;

	extern int InitializeThreadPool(int ThreadCount);
	extern int ThreadPoolCount();

	extern int QueueTaskBlocking(int PoolId, FunctionPointer(void, Task, void*), void* Arg = nullptr);
	extern bool AllTasksDone(int PoolId);
	extern void WaitForAllTasks(int PoolId);

	extern void Sleep(int Milliseconds);
}
