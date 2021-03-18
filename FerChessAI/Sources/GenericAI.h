#pragma once


class DoubleBoard;


class ChessAI
{
public:
	ChessAI();
public:
	virtual void PlayMove(DoubleBoard& Board) = 0;
};







