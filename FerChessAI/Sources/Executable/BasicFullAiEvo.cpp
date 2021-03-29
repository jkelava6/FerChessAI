
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

	TArray<FChessBoard> GradingBoards(16);

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.DefaultBoard();
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(1, 4) = EChessPiece::WhiteKing;
		Board(2, 6) = EChessPiece::WhiteRook;
		Board(2, 3) = EChessPiece::WhiteQueen;
		Board(6, 4) = EChessPiece::BlackKing;
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(2, 6) = EChessPiece::WhitePawn;
		Board(2, 7) = EChessPiece::WhitePawn;
		Board(3, 5) = EChessPiece::WhitePawn;
		Board(1, 5) = EChessPiece::WhiteKing;
		Board(0, 4) = EChessPiece::WhiteRook;

		Board(2, 0) = EChessPiece::WhitePawn;
		Board(3, 1) = EChessPiece::WhiteBishop;

		Board(1, 3) = EChessPiece::WhitePawn;
		Board(2, 3) = EChessPiece::WhiteKnight;

		Board(6, 0) = EChessPiece::BlackPawn;
		Board(6, 1) = EChessPiece::BlackPawn;
		Board(6, 3) = EChessPiece::BlackKnight;

		Board(5, 6) = EChessPiece::BlackPawn;
		Board(6, 7) = EChessPiece::BlackPawn;
		Board(6, 6) = EChessPiece::BlackKing;
		Board(6, 5) = EChessPiece::BlackKnight;
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(1, 4) = EChessPiece::WhiteKing;
		Board(3, 4) = EChessPiece::WhitePawn;
		Board(4, 3) = EChessPiece::WhitePawn;
		Board(0, 3) = EChessPiece::WhiteRook;
		Board(1, 2) = EChessPiece::WhitePawn;

		Board(4, 2) = EChessPiece::BlackPawn;
		Board(5, 3) = EChessPiece::BlackPawn;
		Board(6, 4) = EChessPiece::BlackPawn;
		Board(7, 3) = EChessPiece::BlackRook;
		Board(5, 4) = EChessPiece::BlackKing;
	}

	FDna DnaLib[3];
	FDna& Best = DnaLib[0];
	FDna& Changed = DnaLib[1];
	FDna& Random = DnaLib[2];
	FDna* DnaPtr;
	
	DnaPtr = Mutator.CreateDna();
	Best = Move(*DnaPtr);
	delete DnaPtr;

	while (true)
	{
		Best.CloneTo(Changed);
		Mutator.MutateDna(Changed);
		DnaPtr = Mutator.CreateDna();
		Random = Move(*DnaPtr);
		delete DnaPtr;

		float Scores[] = { 0, 0, 0 };
		ARRAY_MATCH(DnaLib, Scores);

		for (int DnaIndex = 0; DnaIndex < ARRAY_SIZE(DnaLib); ++DnaIndex)
		{
			NNAI.LoadDna(DnaLib[DnaIndex]);
			float& Score = Scores[DnaIndex];
			Score = 0;
			int TotalMoves = 0;

			for (int BoardIndex = 0; BoardIndex < GradingBoards.Count(); ++BoardIndex)
			{
				FDoubleBoard Board;
				Board.CopyPositionFrom(GradingBoards[BoardIndex]);

				for (int Rank = 0; Rank < 8; ++Rank)
				{
					for (int File = 0; File < 8; ++File)
					{
						if (Board(Rank, File) < EChessPiece::BlackKing || Board(Rank, File) > EChessPiece::WhiteKing)
						{
							continue;
						}
					}
				}

				NNAI.StartGame();
				EGameState State = EGameState::ActiveWhite;
				int GameMoves = 0;
				while ((State == EGameState::ActiveWhite || State == EGameState::ActiveBlack) && GameMoves++ < 120)
				{
					if (State == EGameState::ActiveWhite)
					{
						if (!NNAI.PlayMove(Board))
						{
							break;
						}
						++TotalMoves;
					}
					else
					{
						MMAI.PlayMove(Board);
					}

					State = Board.GetGameState();
				}

				if (NNAI.LastMoveVerdict == ELastMoveResult::FailedTimeOut)
				{
					continue;
				}
				Score += 100.0f;
				if (NNAI.LastMoveVerdict == ELastMoveResult::FailedInvalid)
				{
					continue;
				}
				Score += 30.0f;

				if (State == EGameState::OverWhite)
				{
					Score += 30.0f;
				}
				else if (State == EGameState::OverDraw)
				{
					Score += 15.0f;
				}
			}
			
			Score += TotalMoves / 5.0f;
		}

		const int Best = Scores[0] > Max(Scores[1], Scores[2]) ? 0 : Scores[1] > Scores[2] ? 1 : 2;
		if (Best != 0)
		{
			DnaLib[0] = Move(DnaLib[Best]);
			printf("%d> %f\n", Best, Scores[Best]);
		}
	}

	return 0;
}

#endif
