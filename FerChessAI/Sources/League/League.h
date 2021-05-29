#pragma once

#include <Array.h>
#include <League/Population.h>

class IChessAI;
enum class EGameState : int;
class FDoubleBoard;
class FPopulation;
struct FThreadData;

class FLeague
{
public:
	FLeague();
	~FLeague();
	DECLARE_NOCOPY(FLeague);
	DECLARE_NOMOVE(FLeague);
public:
	void Initialize(int PopCount, int InPopSize, int MaxMiddleNodes, int MaxRecurrentNodes);
	void Iterate();
	const FDna& GetDna(int PopulationIndex, int UnitIndex);
	EGameState PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount);
private:
	float GameScore(FDoubleBoard& Board);
	void RateGame(FDoubleBoard& Board, int White, int Black);
	void SetupGame(TArray<FThreadData>& Games, int WhitePop, int WhiteUnit, int BlackPop, int BlackUnit);
public:
	TArray<int> Ratings;
	int NormalDepth = 2;
	int VolatileDepth = 4;
private:
	TArray<FPopulation> Populations;
	int PopSize = -1;

	TArray<float> GameResults;
	int NextGameResult = 0;
	int PoolSize = 100;
	float GlobalScore = 0.0f;

};
