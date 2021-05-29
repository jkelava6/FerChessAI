#pragma once

#include <Array.h>

class FDna;
class FNode;

extern float NetworkDerivativeEpsilon;

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
	DECLARE_COPY(FNetwork);
	DECLARE_MOVE(FNetwork);
public:
	void FromDna(FDna& Dna);
	void ToDna(FDna& Dna);
	void SetInput(int Index, float Value);
	float GetOutput(int Index);

	void Update();
	void ResetRecurrent(int Level);

	void InitiateReinforcement();
	void SeedReinforcement(int Output, float Target, float FeedbackAmount, EReinforcementType Type, float TypeValue = 0.0f);
	void ClearReinforcementStates();
	void ExecuteReinforcement(float MinBiasStep, float MaxBiasStep, float MaxBiasValue,
		float MinLinkStep, float MaxLinkStep, float MaxLinkValue);

	int GetInputCount();

private:
	float SigmaDerivative(const FNode& Node);
private:
	int Inputs = 0;
	int Outputs = 0;
	TArray<int> RecurrentPerLevel;
	TArray<FNode> Nodes;

	int FirstOutput = 0;
	int TotalRecurrent = 0;

	bool bTrackReinforcement = false;
	TArray<TArray<float>> ReinforcementStates;
	TArray<float> BiasReinforcements;
	TArray<TArray<float>> LinkReinforcements;
};





