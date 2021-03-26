#pragma once


class DoubleBoard;


class ChessAI
{
public:
	ChessAI();
public:
	virtual bool PlayMove(DoubleBoard& Board) = 0;
};







