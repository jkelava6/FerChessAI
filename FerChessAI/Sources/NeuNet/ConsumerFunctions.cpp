#include <StdH.h>
#include <NeuNet/ConsumerFunctions.h>

#include <ChessMath.h>

static FunctionPointer(float, ConsumerFunctions[], float, float) =
{
	ConsumeAdd,
	ConsumeMultiply,
	ConsumeMax
};

extern int RandomConsumerIndex()
{
	return (int)(RandomF() * ARRAY_SIZE(ConsumerFunctions));
}

extern FunctionPointer(float, ConsumerFunctionFromIndex(int Index), float, float)
{
	return ConsumerFunctions[Index];
}

extern int ConsumerFunctionToIndex(FunctionPointer(float, Consumer, float, float))
{
	for (int Index = 0; Index < ARRAY_SIZE(ConsumerFunctions); ++Index)
	{
		if (ConsumerFunctions[Index] == Consumer)
		{
			return Index;
		}
	}
	return -1;
}

// --------------------------------------------------------------------------------------------- //
//                                Consumer functions list                                        //
// --------------------------------------------------------------------------------------------- //

extern float ConsumeAdd(float Consumer, float Input)
{
	return Consumer + Input;
}

extern float ConsumeMultiply(float Consumer, float Input)
{
	return Consumer * Input;
}

extern float ConsumeMax(float Consumer, float Input)
{
	return Max(Consumer, Input);
}
