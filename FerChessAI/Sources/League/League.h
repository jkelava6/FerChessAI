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
private:
	EGameState PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount);
	int GameScore(EGameState FinishingState);
	void RateGame(EGameState FinishingState, int White, int Black);
public:
	TArray<int> BestSwaps;
private:
	TArray<FPopulation> Populations;
	TArray<int> Ratings;

	TArray<EGameState> GameResults;
	int NextGameResult = 0;
	int PoolSize = 100;
	int GlobalScore = 0;

};
