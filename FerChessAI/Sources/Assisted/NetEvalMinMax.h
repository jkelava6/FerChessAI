#pragma once

#include <MinMaxAI.h>
#include <NeuNet/Network.h>

class FNetEvalMinMax : public FMinMaxAI
{
public:
	FNetwork& AccesNetwork();
protected:
	virtual float Evaluate(FDoubleBoard& Board) override;
private:
	FNetwork Network;
	int IterationCount;
};
