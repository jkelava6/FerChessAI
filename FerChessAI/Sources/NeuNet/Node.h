#pragma once


#include <Array.h>

class FNode;

struct FNodeInput
{
public:
	FNodeInput(FNode* InHarvestNode = nullptr, float InLinkStrength = 0.0f);
public:
	FNode* HarvestNode;
	float LinkStrength;
};

/**
 * Neural network node.
 * 
 * Implements behaviour of summing inputs and mapping them into <-1, 1> using sigmoid function.
 */
class FNode
{
public:
	FNode(int InputCount = 0, float InBias = 0.0f);
public:
	void SetState(float InState);
	float GetState() const;
	void AddLink(FNode* Node, float Strength);
	void Update();
public:
#if USE_CONSUMER_FUNCTIONS
	FunctionPointer(float, Consumer, float, float) = nullptr;
#endif
#if USE_MAPPING_FUNCTIONS
	FunctionPointer(float, Mapper, float) = nullptr;
#endif
private:
	friend class FNetwork;
	float Bias;
	float State;
	TArray<FNodeInput> Inputs;
};

extern float SigmoidFunction(float Input);


