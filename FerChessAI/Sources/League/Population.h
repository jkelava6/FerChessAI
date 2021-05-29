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

	FUnit();
	~FUnit();
	DECLARE_MOVE(FUnit);
	DECLARE_COPY(FUnit);
};

class FPopulation
{
public:
	void Initialize(int Size, int InMaxMiddleNodes, int InMaxRecurrentNodes);
	void NextGeneration(FLeague& League);
	void GradeMatch(int UnitId, float Score, int Moves);
	const FDna& GetDna(int UnitIndex);
private:
	void MutateDna(FDna& InDna, FDna& OutDna);
private:
	TArray<FUnit> Units;
	int MaxMiddleNodes;
	int MaxRecurrentNodes;
};

extern void GenerateDna(FDna& Dna, int MiddleNodes, int RecurrentNodes);
