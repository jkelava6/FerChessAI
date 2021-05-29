#pragma once

#include <Array.h>

#include <thread>
typedef std::thread FCppThread;

class FThreadPool
{
public:
	FThreadPool();
	~FThreadPool();
	DECLARE_MOVE(FThreadPool);
	DECLARE_NOCOPY(FThreadPool);

public:
	void ResizeBlocking(int Size);
	bool AllThreadsFree();
	void WaitUntilDone();
	int ExecuteTaskThreaded(FunctionPointer(void, Task, void*), void* Arg = nullptr);

private:
	void ExecuteTaskWrapper(int Index, FunctionPointer(void, Task, void*), void* Arg);

private:
	TArray<FCppThread> Threads;
	TArray<bool> Busy;
};
