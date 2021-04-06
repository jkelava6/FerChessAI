#pragma once


class FDoubleBoard;

struct FEvaluatedMove
{
public:
	FEvaluatedMove(int MoveRankFrom = -1, int MoveFileFrom = -1,
		int MoveRankTo = -1, int MoveFileTo = -1, float BoardEval = 0.0f);
public:
	float Evaluation;
	int RankFrom;
	int FileFrom;
	int RankTo;
	int FileTo;

public:
	bool operator== (const FEvaluatedMove& Other) const;
	bool operator!= (const FEvaluatedMove& Other) const;
};

class IChessAI
{
public:
	virtual FEvaluatedMove ChooseMove(FDoubleBoard& Board) = 0;
	virtual bool PlayMove(FDoubleBoard& Board) = 0;
};







