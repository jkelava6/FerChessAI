#include <StdH.h>
#include <Assisted/NetEvalMinMax.h>

#include <ChessBoard.h>

FNetwork& FNetEvalMinMax::AccesNetwork()
{
	return Network;
}

void FNetEvalMinMax::SetIterationCount(int InIterationCount)
{
	IterationCount = InIterationCount;
}

float FNetEvalMinMax::Evaluate(FDoubleBoard& Board)
{
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			Network.SetInput(Rank * FILES + File, (float)Board(Rank, File));
		}
	}

	Network.ResetRecurrent(0);
	for (int Iteration = 0; Iteration < IterationCount; ++Iteration)
	{
		Network.Update();
	}
	return FMinMaxAI::Evaluate(Board) + Network.GetOutput(0) * 10.0f;
}
