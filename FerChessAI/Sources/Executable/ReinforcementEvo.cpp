
#include <Executable/MainSwitch.h>
#if MAIN_USE_REINFORCEMENT_BASIC_EVO

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
	FNeuNetFullMutator Mutator(80, 0, 0, 300, 1, 3, 2.0f, 2.0f, 0.1f, 0.1f, 3, 0.1f, 0.1f, 3, 0.001f);

	FDna* DnaPtr = Mutator.CreateDna();
	NNAI.LoadDna(*DnaPtr);
	delete DnaPtr;

	while (true)
	{
		Board.DefaultBoard();
		
		int Move;
		int Reinforcements = 0;
		for (Move = 0; Move < 60; ++Move)
		{
			FEvaluatedMove PlayedMove = MMAI.ChooseMove(Board);

			while (NNAI.ChooseMove(Board) != PlayedMove)
			{
				++Reinforcements;
				NNAI.ReinforceMove(PlayedMove, 0.005f, 0.05f, 0.001f, 0.001f, 2.0f, 2.0f);
			}

			Board.MovePiece(PlayedMove.RankFrom, PlayedMove.FileFrom, PlayedMove.RankTo, PlayedMove.FileTo);
			if (Board.GetGameState() > EGameState::ActiveBlack)
			{
				break;
			}

			MMAI.PlayMove(Board);
			if (Board.GetGameState() > EGameState::ActiveBlack)
			{
				break;
			}
		}

		printf("%d | %d\n", Move, Reinforcements);
	}

	return 0;
}

#endif
