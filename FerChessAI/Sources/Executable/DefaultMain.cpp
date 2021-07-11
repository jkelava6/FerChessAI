#include <Executable/MainSwitch.h>
#if MAIN_USE_DEFAULT_MAIN

#include <Utils/IncludeStdio.h>




int main()
{
	FStdio::ConsoleOut("Hello World!\n\n\nExecuting default main, Executables/DefaultMain.cpp\n\n\n");

	return 0;
}

#endif