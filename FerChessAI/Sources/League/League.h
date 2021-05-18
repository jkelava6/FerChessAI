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
	void PlayAI(IChessAI& Challenger, FPopulation* Population, int UnitId);
private:
	EGameState PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount);
private:
	TArray<FPopulation> Populations;
};
