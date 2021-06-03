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
	ChessThreads::InitializeThreadPool(12);
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
	const int BenchmarkPeriod = 10;
	TArray<char> BMResults;

	for (int Generation = 1; ; ++Generation)
	{
		League.Iterate();

		if (Generation % RatingsPeriod == 0)
		{
			printf("[Gen%4d] Ratings: ", Generation);
			for (int Index = 0; Index < League.Ratings.Count(); ++Index)
			{
				printf("%d ", League.Ratings[Index]);
			}
			printf("\n");
		}

		if (Generation % BenchmarkPeriod == 0)
		{
			TArray<FThreadData> Games;

			for (int PopIndex = 0; PopIndex < PopCount; ++PopIndex)
			{
				for (int BMIndex = 0; BMIndex < BMCount; ++BMIndex)
				{
					FThreadData& Game = Games.Push();

					Game.League = &League;
					Game.Board.DefaultBoard();
					Game.Board.AllocateStack(256);

					FDna DNA = League.GetDna(PopIndex, 0);
					Game.WhiteAI.AccesNetwork().FromDna(DNA);
					Game.WhiteAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game.BlackAI.SetDepths(BMIndex + 1, BMIndex * 2 + 2);

					Game.MaxMoves = 120;
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
					FThreadData& Game = Games[PopIndex * BMCount + BMIndex];
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

					printf(" - vs MinMax(%d, %d) : [Result: %c] [Moves: %d]\n", BMIndex + 1, 2 * BMIndex + 2, Result, Game.MoveCount);
					BMResults.Push() = Result;

					if (Board.IsFlipped())
					{
						Board.FlipBoard();
					}
					DebugBoard(Board);
				}
				BMResults.Push() = ' ';
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