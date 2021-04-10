#pragma once

#include <Array.h>

class FDna;
class FNode;

class FNetwork
{
public:
	enum class EReinforcementType : int
	{
		Full,
		//RandomLines,
		RandomChance,
		//RandomNumber,
	};
public:
	FNetwork();
	~FNetwork();
public:
	void FromDna(FDna& Dna);
	void ToDna(FDna& Dna);
	void SetInput(int Index, float Value);
	float GetOutput(int Index);

	void Update();
	void ResetRecurrent(int Level);

	void ReinforceOutput(int OutputIndex, float OutputValue, bool bAffectLeftRecurrent,
		float BiasStep, float MaxBias, float LinkStep, float MaxLink,
		int RecurrentDepth, EReinforcementType Type, float RandomTypeParam = 1.0f);
private:
	float SigmaDerivative(const FNode& Node);
private:
	int Inputs = 0;
	int Outputs = 0;
	TArray<int> RecurrentPerLevel;
	TArray<FNode> Nodes;

	int FirstOutput = 0;
	int TotalRecurrent = 0;
};





