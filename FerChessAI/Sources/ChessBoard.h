#pragma once

#include <Array.h>

class ChessBoard;

enum class ChessPiece : int
{
	None = 0,
	WhitePawn = 1,
	WhiteKnight = 2,
	WhiteBishop = 3,
	WhiteRook = 4,
	WhiteQueen = 5,
	WhiteKing = 6,
	BlackPawn = -1,
	BlackKnight = -2,
	BlackBishop = -3,
	BlackRook = -4,
	BlackQueen = -5,
	BlackKing = -6
};

class RevertSquare
{
public:
	RevertSquare();
	RevertSquare(int ChangedRow, int ChangedFile, ChessPiece OldPiece);
public:
	void Restore(ChessBoard& Board);
private:
	int Row = -1;
	int File = -1;
	ChessPiece Piece = ChessPiece::None;
};

class RevertMove
{
public:
	RevertMove();
public:
	void Change(ChessBoard& Board, int Row, int File, ChessPiece Piece);
	void Revert(ChessBoard& Board);
private:
	Array<RevertSquare> ChangedInOrder;
};

class ChessBoard
{
public:
	ChessBoard();
	~ChessBoard();
	DECLARE_NOCOPY(ChessBoard);
	DECLARE_NOMOVE(ChessBoard);
public:
	void EmptyBoard();
	void DefaultBoard();
	ChessPiece& operator()(int Row, int File);
	ChessPiece& Square(int Row, int File);

	void Move(int RowFrom, int FileFrom, int RowTo, int FileTo);
	void Undo();
	void CollectMoves(int Row, int File, Array<int>& Rows, Array<int>& Files);
private:
	bool AreCoordsValid(int Row, int File);
	void CollectLineMovement(int Row, int File, int DeltaRow, int DeltaFile, Array<int>& Rows, Array<int>& Files);
private:
	ChessPiece* Pieces;
	Array<RevertMove> MoveStack;
};

class DoubleBoard
{
public:
	DoubleBoard();
	DECLARE_NOCOPY(DoubleBoard);
	DECLARE_NOMOVE(DoubleBoard);
public:
	void EmptyBoard();
	void DefaultBoard();
	const ChessPiece& operator()(int Row, int File);
	const ChessPiece& Square(int Row, int File);

	void Move(int RowFrom, int FileFrom, int RowTo, int FileTo);
	void Undo();
	void CollectMoves(int Row, int File, Array<int>& Rows, Array<int>& Files);

	void FlipBoard();
private:
	ChessBoard WhiteBoard;
	ChessBoard BlackBoard;
	bool bFlipped = false;
};




