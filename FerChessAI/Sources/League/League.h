#pragma once

#include <Array.h>
#include <League/Population.h>

class IChessAI;
enum class EGameState : int;

class FLeague
{
public:
	void Initialize(int PopCount, int PopSize, int MaxMiddleNodes, int MaxRecurrentNodes);
	void Iterate();
	void PlayAI(IChessAI& Challenger, FPopulation* Population, int UnitId, bool bRated);
	void LogSwap(FPopulation* Population);
	/** Generated pointers lose validity when this objects is destroyed, or Initialize() is called. */
	void GetAIs(TArray<IChessAI*>& OutTempAIs);
private:
	EGameState PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount);
	int GameScore(EGameState FinishingState);
	void RateGame(EGameState FinishingState, int White, int Black);
public:
	TArray<int> BestSwaps;
	TArray<int> Ratings;
private:
	TArray<FPopulation> Populations;

	TArray<EGameState> GameResults;
	int NextGameResult = 0;
	int PoolSize = 100;
	int GlobalScore = 0;

};
