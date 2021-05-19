#pragma once

#include <Array.h>
#include <Assisted/NetEvalMinMax.h>
#include <NeuNet/Dna.h>

class FLeague;
enum class EGameState : int;

class FUnit
{
public:
	FDna Dna;
	float Fitness = 0.0f;
};

class FPopulation
{
public:
	void Initialize(int Size, int InMaxMiddleNodes, int InMaxRecurrentNodes);
	void PlayInLeague(FLeague& League);
	void NextGeneration();
	void GradeMatch(int UnitId, EGameState WhiteResult, int WhiteMoves, EGameState BlackResult, int BlackMoves);
	IChessAI& Representative();
private:
	void MutateDna(FDna& InDna, FDna& OutDna);
private:
	FNetEvalMinMax BestInPop;
	TArray<FUnit> Units;
	int MaxMiddleNodes;
	int MaxRecurrentNodes;
	int BestIndexInPop = -1;
};

extern void GenerateDna(FDna& Dna, int MiddleNodes, int RecurrentNodes);
