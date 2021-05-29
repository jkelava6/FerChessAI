#include <Executable/MainSwitch.h>
#if MAIN_USE_LEAGUE_PLAY

#include <StdH.h>

#include <Array.h>
#include <ChessBoard.h>
#include <ThreadInclude.h>
#include <Assisted/NetEvalMinMax.h>
#include <Executable/DebugUtils.h>
#include <League/League.h>
#include <Threading/ThreadingUtils.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>


int main()
{
	ChessThreads::InitializeThreadPool(8);
	FirstThreadId = std::this_thread::get_id();
	FLeague League;
	const int PopCount = 3;
	const int PopSize = 4;
	League.Initialize(PopCount, PopSize, 40, 10);
	FMinMaxAI BenchmarkAIs[4];
	for (int Index = 0; Index < ARRAY_SIZE(BenchmarkAIs); ++Index)
	{
		BenchmarkAIs[Index].SetDepths(Index + 1, 2 * (Index + 1));
	}

	const int RatingsPeriod = 1;
	const int BenchmarkPeriod = 10;
	FDoubleBoard Board;
	TArray<char> BMResults;

	for (int Generation = 1; ; ++Generation)
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
			TArray<FNetEvalMinMax> LeagueAIs;
			for (int PopIndex = 0; PopIndex < PopCount; ++PopIndex)
			{
				FNetEvalMinMax& AI = LeagueAIs.Push();
				FDna DNA = League.GetDna(PopIndex, 0);
				AI.AccesNetwork().FromDna(DNA);
			}

			BMResults.Clear();
			for (int PopIndex = 0; PopIndex < LeagueAIs.Count(); ++PopIndex)
			{
				printf("(%d) Rating: %d\n", PopIndex, League.Ratings[PopIndex]);
				for (int BMIndex = 0; BMIndex < ARRAY_SIZE(BenchmarkAIs); ++BMIndex)
				{
					Board.DefaultBoard();
					int Moves;
					bool bFlippedOnEnd = false;
					for (Moves = 0; Moves < 120; ++Moves)
					{
						LeagueAIs[PopIndex].PlayMove(Board);
						Board.FlipBoard();

						if (Board.GetGameState() > EGameState::ActiveBlack)
						{
							bFlippedOnEnd = true;
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
					EGameState EndState = Board.GetGameState();
					switch (EndState)
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
					BMResults.Push() = Result;

					if (bFlippedOnEnd)
					{
						Board.FlipBoard();
					}
					DebugBoard(Board);
				}
			}
			for (int BMIndex = 0; BMIndex < BMResults.Count(); ++BMIndex)
			{
				printf("%c", BMResults[BMIndex]);
			}
			printf("\n");
		}
	}

	return 0;
}

#endif