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
	ChessThreads::InitializeThreadPool(10);
	FirstThreadId = std::this_thread::get_id();
	FLeague League;
	const int PopCount = 6;
	const int PopSize = 5;
	const int BMCount = 4;
	const int MaxMiddleNodes = 40;
	const int MaxRecurrentNodes = 10;
	League.Initialize(PopCount, PopSize, MaxMiddleNodes, MaxRecurrentNodes, RANKS, 0.05f,
		0.7f, (int)(0.8f * MaxMiddleNodes), 0.1f,
		0.7f, (int)(0.8f * MaxRecurrentNodes), 0.1f
		, 0.004f, 0.004f
	);

	const int RatingsPeriod = 1;
	const int BenchmarkPeriod = 100;
	const int LockPeriod = 20;
	const int InitialLocks = 1;
	const int ScoresPeriod = 1;
#if USE_BEST_PRESERVATION
	const int SwapsPeriod = 1;
#endif
	const int LinePeriod = 1;
	int NextLock = 0;
	for (; NextLock < InitialLocks; ++NextLock)
	{
		League.SetLocked(NextLock, true);
	}
	TArray<char> BMResults;

	for (int Generation = 1; ; ++Generation)
	{
		League.Iterate();

		if (RatingsPeriod != -1 && Generation % RatingsPeriod == 0)
		{
			printf("[Gen%4d] Ratings: ", Generation);
			for (int Index = 0; Index < League.Ratings.Count(); ++Index)
			{
				printf("%4d ", League.Ratings[Index]);
			}
			printf("\n");
		}


		if (ScoresPeriod != -1 && Generation % ScoresPeriod == 0)
		{
			printf("[Gen%4d] Scores : ", Generation);
			for (int Index = 0; Index < League.Ratings.Count(); ++Index)
			{
				printf("%4.1f ", League.GenPoints[Index]);
			}
			printf("\n");
		}

#if USE_BEST_PRESERVATION
		if (SwapsPeriod != -1 && Generation % SwapsPeriod == 0)
		{
			printf("[Gen%4d] Swaps  : ", Generation);
			for (int Index = 0; Index < League.Ratings.Count(); ++Index)
			{
				printf(League.BestSwitched[Index] ? "swap " : "     ");
			}
			printf("\n");
		}
#endif

		if (LinePeriod != -1 && Generation % LinePeriod == 0)
		{
			printf("\n");
		}

		if (BenchmarkPeriod != -1 && Generation % BenchmarkPeriod == 0)
		{
			TArray<FThreadData> Games;

			for (int PopIndex = 0; PopIndex < PopCount; ++PopIndex)
			{
				for (int BMIndex = 0; BMIndex < BMCount; ++BMIndex)
				{
					FThreadData& Game = Games.Push();

					Game.Board.DefaultBoard();
					Game.Board.AllocateStack(256);

					FDna DNA = League.GetDna(PopIndex, 0);
					Game.WhiteAI.AccesNetwork().FromDna(DNA);
					Game.WhiteAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game.BlackAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game.MaxMoves = 120;

					FThreadData& Game2 = Games.Push();

					Game2.Board.DefaultBoard();
					Game2.Board.AllocateStack(256);

					DNA = League.GetDna(PopIndex, 0);
					Game2.BlackAI.AccesNetwork().FromDna(DNA);
					Game2.BlackAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game2.WhiteAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game2.MaxMoves = 120;
				}
			}

			for (int Index = 0; Index < Games.Count(); ++Index)
			{
				ChessThreads::QueueTaskBlocking(0, ExecPlay, &Games[Index]);
			}
			ChessThreads::WaitForAllTasks(0);

			BMResults.Clear();
			for (int PopIndex = 0; PopIndex < PopCount; ++PopIndex)
			{
				printf("(%d) Rating: %d\n", PopIndex, League.Ratings[PopIndex]);
				for (int BMIndex = 0; BMIndex < BMCount; ++BMIndex)
				{
					for (int GameIndex = 0; GameIndex < 2; ++GameIndex)
					{
						FThreadData& Game = Games[PopIndex * BMCount * 2 + 2 * BMIndex + GameIndex];
						FDoubleBoard& Board = Game.Board;

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
							Result = GameIndex == 0 ? 'W' : 'L';
							break;
						case EGameState::OverDraw:
							Result = 'D';
							break;
						case EGameState::OverBlack:
							Result = GameIndex == 0 ? 'L' : 'W';
							break;
						default:
							break;
						}

						printf(" - %c vs MinMax(%d, %d) : [Result: %c] [Moves: %d]\n", (GameIndex == 0 ? 'W' : 'B'), BMIndex + 1, 2 * BMIndex + 2, Result, Game.MoveCount);
						BMResults.Push() = Result;

						if (Board.IsFlipped())
						{
							Board.FlipBoard();
						}
						DebugBoard(Board);
					}
				}
				BMResults.Push() = ' ';
			}
			for (int BMIndex = 0; BMIndex < BMResults.Count(); ++BMIndex)
			{
				printf("%c", BMResults[BMIndex]);
			}
			printf("\n");
		}

		if (LockPeriod != -1 && Generation % LockPeriod == 0)
		{
			League.SetLocked(NextLock++, true);
			if (NextLock == PopCount)
			{
				printf("All populations locked from evolving! Ending simulation...\n");
				break;
			}
		}

		fflush(stdout);
	}

	return 0;
}

#endif