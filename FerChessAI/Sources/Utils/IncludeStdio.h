#pragma once

#include <cstdio>
#include <StdH.h>

namespace FStdio
{
	FunctionPointer(int, ConsoleOut, const char* Format, ...) = printf;
	FunctionPointer(int, ConsoleIn, const char* Format, ...) = scanf_s;
}
