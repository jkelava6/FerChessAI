#pragma once



#include <GenericAI.h>
#include <Array.h>

constexpr float MINMAX_EVAL_LOSS = -1e9f;
constexpr float MINMAX_EVAL_WIN = 1e9f;

struct FEvaluatedMove
{
public:
	FEvaluatedMove(int MoveRowFrom = -1, int MoveFileFrom = -1,
		int MoveRowTo = -1, int MoveFileTo = -1, float BoardEval = 2.0f * MINMAX_EVAL_LOSS);
public:
	float Evaluation;
	__int8 RowFrom;
	__int8 FileFrom;
	__int8 RowTo;
	__int8 FileTo;
};

class FMinMaxAI : public IChessAI
{
public:
	FMinMaxAI();
public:
	virtual bool PlayMove(FDoubleBoard& Board) override;
	void SetDepths(int Normal, int Volatile);
private:
	float Evaluate(FDoubleBoard& Board);
	float MinMax(FDoubleBoard& Board, float Alfa, float Beta, int Depth, bool bVolatile);
public:
	FEvaluatedMove LastPlayedMove;
private:
	int MaxDepth = 4;
	int MaxVolatileDepth = 8;
};


