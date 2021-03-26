#pragma once

#include <Array.h>

class FChessBoard;

enum class FChessPiece : int
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

class FRevertSquare
{
public:
	FRevertSquare();
	FRevertSquare(int ChangedRow, int ChangedFile, FChessPiece OldPiece);
public:
	void Restore(FChessBoard& Board);
private:
	int Row = -1;
	int File = -1;
	FChessPiece Piece = FChessPiece::None;
};

class FRevertMove
{
public:
	FRevertMove();
public:
	void Change(FChessBoard& Board, int Row, int File, FChessPiece Piece);
	void Revert(FChessBoard& Board);
	void SaveMask(__int64 BitMask);
	void SaveEnPassant(int Row, int File);
private:
	TArray<FRevertSquare> ChangedInOrder;
	__int64 MovedMask = 0;
	int EnPassantRow = -1;
	int EnPassantFile = -1;
};

class FChessBoard
{
public:
	FChessBoard();
	~FChessBoard();
	DECLARE_NOCOPY(FChessBoard);
	DECLARE_NOMOVE(FChessBoard);
public:
	void EmptyBoard();
	void DefaultBoard();
	FChessPiece& operator()(int Row, int File);
	FChessPiece& Square(int Row, int File);

	void Move(int RowFrom, int FileFrom, int RowTo, int FileTo);
	void Undo();
	void CollectMoves(int Row, int File, TArray<int>& Rows, TArray<int>& Files);
	void SetMoved(__int64 BitMask);
	void SetEnPassant(int Row, int File);
	void CopyPositionFrom(FChessBoard& Board);
private:
	bool AreCoordsValid(int Row, int File);
	void CollectLineMovement(int Row, int File, int DeltaRow, int DeltaFile, TArray<int>& Rows, TArray<int>& Files);
	bool IsMoved(int Row, int File);
	void LogMoved(int Row, int File);
	bool IsAttacked(int Row, int File);
private:
	FChessPiece* Pieces = nullptr;
	TArray<FRevertMove> MoveStack;
	__int64 MovedMask = 0;
	int EnPassantRow = -1;
	int EnPassantFile = -1;
};

class FDoubleBoard
{
public:
	FDoubleBoard();
	DECLARE_NOCOPY(FDoubleBoard);
	DECLARE_NOMOVE(FDoubleBoard);
public:
	void EmptyBoard();
	void DefaultBoard();
	const FChessPiece& operator()(int Row, int File);
	const FChessPiece& Square(int Row, int File);

	void Move(int RowFrom, int FileFrom, int RowTo, int FileTo);
	void Undo();
	void CollectMoves(int Row, int File, TArray<int>& Rows, TArray<int>& Files);

	void FlipBoard();
private:
	FChessBoard WhiteBoard;
	FChessBoard BlackBoard;
	bool bFlipped = false;
};




