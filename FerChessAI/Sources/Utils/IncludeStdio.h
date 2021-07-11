#pragma once

#include <cstdio>
#include <StdH.h>

namespace FConsoleC
{
	FunctionPointer(int, Write, const char* Format, ...) = printf;
	FunctionPointer(int, Read, const char* Format, ...) = scanf_s;
}
