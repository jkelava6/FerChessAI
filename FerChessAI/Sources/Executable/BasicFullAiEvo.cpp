
#include <Executable/MainSwitch.h>
#if MAIN_USE_BASIC_FULL_AI_EVO

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <NeuNet/Dna.h>
#include <NeuNet/NeuNetAI.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

int main()
{
	FDoubleBoard Board;
	FMinMaxAI MMAI;
	MMAI.SetDepths(2, 6);
	FNeuNetFullAI NNAI;
	NNAI.SetTimeControl(5, 5, 5);
	FNeuNetFullMutator Mutator(40, 20, 0, 200, 1, 3, 2.0f, 2.0f, 0.1f, 0.1f, 3, 0.1f, 0.1f, 3, 0.001f);

	return 0;
}

#endif
