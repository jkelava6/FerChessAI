#pragma once



#include <GenericAI.h>
#include <Array.h>

constexpr float MINMAX_EVAL_LOSS = -1e9f;
constexpr float MINMAX_EVAL_WIN = 1e9f;

struct EvaluatedMove
{
public:
	EvaluatedMove(int MoveRowFrom = -1, int MoveFileFrom = -1,
		int MoveRowTo = -1, int MoveFileTo = -1, float BoardEval = 2.0f * MINMAX_EVAL_LOSS);
public:
	float Evaluation;
	__int8 RowFrom;
	__int8 FileFrom;
	__int8 RowTo;
	__int8 FileTo;
};

class MinMaxAI : public ChessAI
{
public:
	MinMaxAI();
public:
	virtual void PlayMove(DoubleBoard& Board) override;
private:
	float Evaluate(DoubleBoard& Board);
	float MinMax(DoubleBoard& Board, float Alfa, float Beta, int Depth, bool bVolatile);
public:
	EvaluatedMove LastPlayedMove;
private:
	int MaxDepth = 4;
	int MaxVolatileDepth = 6;
};


