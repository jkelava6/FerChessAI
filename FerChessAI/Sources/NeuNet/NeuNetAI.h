#pragma once

#include <GenericAI.h>
#include <NeuNet/Network.h>

class FNeuNetFullAI : public IChessAI
{
	enum class ELastMoveResult : __int8
	{
		None,
		Valid,
		FailedTimeOut,
		FailedInvalid
	};
public:
	FNeuNetFullAI();
public:
	virtual FEvaluatedMove ChooseMove(FDoubleBoard& Board) override;
	virtual bool PlayMove(FDoubleBoard& Board) override;
	void SetTimeControl(int InStartTicks, int InTicksPerMove, int InMaxTicks);
	void LoadDna(FDna& Dna);

	void StartGame();
public:
	ELastMoveResult LastMoveVerdict = ELastMoveResult::None;
private:
	FNetwork Network;
	int TicksRemaining = 5;
	int StartTicks = 5;
	int TicksPerMove = 5;
	int MaxTicks = 5;
};

class FNeuNetFullMutator
{
public:
	FNeuNetFullMutator(
		int InMoveRecurrent, int InGameRecurrent, int InLifeRecurrent, int InMiddleNodes,
		int InLowInputs, int InHighInputs, float InMaxBias, float InMaxLinkStrength,
		float InBiasChangeChance, float InBiasChangeRatio, int InBiasChangeResilience,
		float InLinkChangeChance, float InLinkChangeRatio, int InLinkChangeResilience,
		float InLinkRedirectChance);
public:
	FDna* CreateDna();
	void MutateDna(FDna& Dna);
private:
	void MutateF(float& Val, float Chance, float Ratio, int Resilience, float MaxVal);
private:
	int Inputs = 65;
	int Outputs = 5;
	int MoveRecurrent;
	int GameRecurrent;
	int LifeRecurrent;
	int MiddleNodes;
	int LowInputs;
	int HighInputs;
	float MaxBias;
	float MaxLinkStrength;

	float BiasChangeChance;
	float BiasChangeRatio;
	int BiasChangeResilience;
	float LinkChangeChance;
	float LinkChangeRatio;
	int LinkChangeResilience;
	float LinkRedirectionChance;
};


