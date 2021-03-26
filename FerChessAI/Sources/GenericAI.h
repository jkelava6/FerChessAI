#pragma once


class FDoubleBoard;


class IChessAI
{
public:
	virtual bool PlayMove(FDoubleBoard& Board) = 0;
};







