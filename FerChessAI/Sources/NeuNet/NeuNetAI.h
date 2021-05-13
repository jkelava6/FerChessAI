#pragma once

#include <GenericAI.h>
#include <NeuNet/Network.h>

class FNeuNetFullAI : public IChessAI
{
public:
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
	void LoadDna(FDna& Dna);

	void InitiateFeedback();
	/** MUST be called AFTER ChooseMove() or PlayMove() */
	void ReinforceMove(FEvaluatedMove Move, float Feedback);
	void ReinforceByDistance(FEvaluatedMove Move, FEvaluatedMove CurrentMove, float BaseFeedback, float ScalingFeedback);
	void ClearNetworkStateMemory();
	void EvaluateFeedback(float BiasStep, float BiasMax, float LinkStep, float LinkMax);

	void StartGame();
public:
	ELastMoveResult LastMoveVerdict = ELastMoveResult::None;
private:
	float Iterations = 5;
	FNetwork Network;
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
	int Inputs = RANKS * FILES;
	int Outputs = 4;
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


