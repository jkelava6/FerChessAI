#pragma once

#include <Array.h>
#include <League/Population.h>
#include <ChessBoard.h>

class IChessAI;
enum class EGameState : int;
class FDoubleBoard;
class FPopulation;

struct FThreadData
{
	FThreadData();
	DECLARE_MOVE(FThreadData);
	FLeague* League = nullptr;
	int MaxMoves = 60;
	FDoubleBoard Board;
	int PopIndexWhite = -1;
	int PopIndexBlack = -1;
	int UnitIndexWhite = -1;
	int UnitIndexBlack = -1;
	int MoveCount = -1;
	FNetEvalMinMax WhiteAI;
	FNetEvalMinMax BlackAI;
};

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
	EGameState PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount, int MaxMoves);
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

extern void ExecPlay(void* Arg);
