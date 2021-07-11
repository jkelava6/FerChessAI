#include <StdH.h>
#include <NeuNet/MappingFunctions.h>

#include <AiMath.h>
#include <NeuNet/Node.h>

static FunctionPointer(float, MappingFunctions[], float) =
{
	MapSigmoid,
	MapSine,
	MapStep
};

extern int RandomMappingIndex()
{
	return (int)(RandomF() * ARRAY_SIZE(MappingFunctions));
}

extern FunctionPointer(float, MappingFunctionFromIndex(int Index), float)
{
	return MappingFunctions[Index];
}

extern int MappingFunctionToIndex(FunctionPointer(float, Mapper, float))
{
	for (int Index = 0; Index < ARRAY_SIZE(MappingFunctions); ++Index)
	{
		if (MappingFunctions[Index] == Mapper)
		{
			return Index;
		}
	}
	return -1;
}

// --------------------------------------------------------------------------------------------- //
//                                 Mapping functions list                                        //
// --------------------------------------------------------------------------------------------- //

extern float MapSigmoid(float PreMappedValue)
{
	return SigmoidFunction(PreMappedValue);
}

extern float MapSine(float PreMappedValue)
{
	return sin(PreMappedValue);
}

extern float MapStep(float PreMappedValue)
{
	return SignF(PreMappedValue);
}
