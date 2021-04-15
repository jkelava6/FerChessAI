
#include <Executable/MainSwitch.h>
#if MAIN_USE_REINFORCEMENT_BASIC_EVO

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <Executable/BoardLib.h>
#include <Executable/DebugUtils.h>
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
	FNeuNetFullMutator Mutator(200, 0, 0, 1001, 1, 3, 2.0f, 2.0f, 0.1f, 0.1f, 3, 0.1f, 0.1f, 3, 0.001f);

	TArray<FChessBoard> TestBoards;
	GenerateGradingSet(TestBoards);

	FDna* DnaPtr = Mutator.CreateDna();
	NNAI.LoadDna(*DnaPtr);
	delete DnaPtr;

	const int UsedBoards = TestBoards.Count();
	const bool AllowedBoards[] = { 0, 0, 1, 0 };
	while (true)
	{
		for (int BoardIndex = 0; BoardIndex < UsedBoards; ++BoardIndex)
		{
			if (!AllowedBoards[BoardIndex])
			{
				continue;
			}
			Board.CopyPositionFrom(TestBoards[BoardIndex]);

			int Move;
			int Reinforcements = 0;
			int CorrectMoves = 0;
			for (Move = 0; Move < 20; ++Move)
			{
				FEvaluatedMove PlayedMove = MMAI.ChooseMove(Board);

				static bool bDebugBoard = false;
				if (bDebugBoard)
				{
					DebugBoard(Board);
					bDebugBoard = false;
				}

				int ThisMoveReinf = 0;
				while (NNAI.ChooseMove(Board) != PlayedMove)
				{
					++ThisMoveReinf;
					++Reinforcements;
					NNAI.ReinforceMove(PlayedMove, 0.01f, 0.01f, 0.001f, 0.001f, 2.0f, 2.0f);
				}
				if (ThisMoveReinf)
				{
					// if we didn't get the move immediately, reinforcements were needed - and got us barely into the move
					// a few more iterations should make the network a bit more resistant from mutating out
					for (int AdditionalReinforcement = 0; AdditionalReinforcement < 15; ++AdditionalReinforcement)
					{
						++ThisMoveReinf;
						++Reinforcements;
						NNAI.ReinforceMove(PlayedMove, 0.01f, 0.01f, 0.001f, 0.001f, 2.0f, 2.0f);
					}
					printf("X");
				}
				else
				{
					++CorrectMoves;
					printf(".");
				}

				Board.MovePiece(PlayedMove.RankFrom, PlayedMove.FileFrom, PlayedMove.RankTo, PlayedMove.FileTo);
				if (Board.GetGameState() > EGameState::ActiveBlack)
				{
					break;
				}

				Board.FlipBoard();
				MMAI.PlayMove(Board);
				Board.FlipBoard();
				if (Board.GetGameState() > EGameState::ActiveBlack)
				{
					break;
				}
			}

			printf("\nMoves %d | Correct %d | Reinforcements: %d\n", Move, CorrectMoves, Reinforcements);
		}
	}

	return 0;
}

#endif
