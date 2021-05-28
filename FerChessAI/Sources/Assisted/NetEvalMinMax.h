#pragma once

#include <MinMaxAI.h>
#include <NeuNet/Network.h>

class FNetEvalMinMax : public FMinMaxAI
{
public:
	FNetwork& AccesNetwork();
	void SetIterationCount(int InIterationCount);
protected:
	virtual float Evaluate(FDoubleBoard& Board) override;
private:
	FNetwork Network;
	int IterationCount = 5;
};
