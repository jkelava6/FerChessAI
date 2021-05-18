#pragma once



#include <GenericAI.h>
#include <Array.h>

constexpr float MINMAX_EVAL_LOSS = -1e9f;
constexpr float MINMAX_EVAL_WIN = 1e9f;

class FMinMaxAI : public IChessAI
{
public:
	FMinMaxAI();
public:
	static float Eval(FDoubleBoard& Board, int NormalDepth, int VolatileDepth);

	virtual FEvaluatedMove ChooseMove(FDoubleBoard& Board) override;
	virtual bool PlayMove(FDoubleBoard& Board) override;
	void SetDepths(int Normal, int Volatile);
protected:
	virtual float Evaluate(FDoubleBoard& Board);
private:
	float MinMax(FDoubleBoard& Board, float Alfa, float Beta, int Depth, bool bVolatile);
public:
	FEvaluatedMove LastPlayedMove;
private:
	int MaxDepth = 4;
	int MaxVolatileDepth = 8;
};


