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
	void SaveMask(__int64 BitMask);
	void SaveEnPassant(int Row, int File);
private:
	Array<RevertSquare> ChangedInOrder;
	__int64 MovedMask = 0;
	int EnPassantRow = -1;
	int EnPassantFile = -1;
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
	void SetMoved(__int64 BitMask);
	void SetEnPassant(int Row, int File);
private:
	bool AreCoordsValid(int Row, int File);
	void CollectLineMovement(int Row, int File, int DeltaRow, int DeltaFile, Array<int>& Rows, Array<int>& Files);
	bool IsMoved(int Row, int File);
	void LogMoved(int Row, int File);
	bool IsAttacked(int Row, int File);
private:
	ChessPiece* Pieces = nullptr;
	Array<RevertMove> MoveStack;
	__int64 MovedMask = 0;
	int EnPassantRow = -1;
	int EnPassantFile = -1;
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




