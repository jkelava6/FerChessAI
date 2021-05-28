#include <Executable/MainSwitch.h>
#if MAIN_USE_LEAGUE_PLAY

#include <StdH.h>

#include <ChessBoard.h>
#include <Assisted/NetEvalMinMax.h>
#include <Executable/DebugUtils.h>
#include <League/League.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>


int main()
{
	FLeague League;
	League.Initialize(3, 4, 40, 10);
	FMinMaxAI BenchmarkAIs[4];
	for (int Index = 0; Index < ARRAY_SIZE(BenchmarkAIs); ++Index)
	{
		BenchmarkAIs->SetDepths(Index + 1, 2 * (Index + 1));
	}

	const int RatingsPeriod = 1;
	const int BenchmarkPeriod = 3;
	TArray<IChessAI*> LeagueAIs;
	League.GetAIs(LeagueAIs);
	FDoubleBoard Board;

	for (int Generation = 1; ; Generation++)
	{
		League.Iterate();

		if (Generation % RatingsPeriod == 0)
		{
			printf("Ratings: ");
			for (int Index = 0; Index < League.Ratings.Count(); ++Index)
			{
				printf("%d ", League.Ratings[Index]);
			}
			printf("\n");
		}

		if (Generation % BenchmarkPeriod == 0)
		{
			for (int PopIndex = 0; PopIndex < LeagueAIs.Count(); ++PopIndex)
			{
				printf("(%d) Rating: %d\n", PopIndex, League.Ratings[PopIndex]);
				for (int BMIndex = 0; BMIndex < ARRAY_SIZE(BenchmarkAIs); ++BMIndex)
				{
					Board.DefaultBoard();
					int Moves;
					for (Moves = 0; Moves < 120; ++Moves)
					{
						LeagueAIs[PopIndex]->PlayMove(Board);
						Board.FlipBoard();

						if (Board.GetGameState() > EGameState::ActiveBlack)
						{
							break;
						}

						BenchmarkAIs[BMIndex].PlayMove(Board);
						Board.FlipBoard();

						if (Board.GetGameState() > EGameState::ActiveBlack)
						{
							break;
						}
					}

					char Result = ' ';
					switch (Board.GetGameState())
					{
					case EGameState::ActiveWhite:
						// fal through
					case EGameState::ActiveBlack:
						Result = '-';
						break;
					case EGameState::OverWhite:
						Result = 'W';
						break;
					case EGameState::OverDraw:
						Result = 'D';
						break;
					case EGameState::OverBlack:
						Result = 'L';
						break;
					default:
						break;
					}

					printf(" - vs MinMax(%d, %d) : [Result: %c] [Moves: %d]\n", BMIndex + 1, 2 * BMIndex + 2, Result, Moves);
					DebugBoard(Board);
				}
			}
		}
	}

	return 0;
}

#endif