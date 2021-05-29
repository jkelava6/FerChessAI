#pragma once

#include <Array.h>

class FChessBoard;

enum class EChessPiece : int
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
	FRevertSquare(int ChangedRank, int ChangedFile, EChessPiece OldPiece);
public:
	void Restore(FChessBoard& Board);
private:
	friend class FRevertMove;
	int Rank = -1;
	int File = -1;
	EChessPiece Piece = EChessPiece::None;
};

class FRevertMove
{
public:
	FRevertMove();
public:
	void Change(FChessBoard& Board, int Rank, int File, EChessPiece Piece);
	void Revert(FChessBoard& Board);
	void SaveMask(__int64 BitMask);
	void SaveEnPassant(int Rank, int File);
	
	FRevertMove Flipped();
private:
	TArray<FRevertSquare> ChangedInOrder;
	__int64 MovedMask = 0;
	int EnPassantRank = -1;
	int EnPassantFile = -1;
};

class FChessBoard
{
public:
	FChessBoard();
	~FChessBoard();
	DECLARE_NOCOPY(FChessBoard);
	FChessBoard& operator= (FChessBoard&& Moved);
	FChessBoard(FChessBoard&& Moved);
public:
	void EmptyBoard();
	void DefaultBoard();
	EChessPiece& operator()(int Rank, int File);
	EChessPiece& Square(int Rank, int File);

	void MovePiece(int RankFrom, int FileFrom, int RankTo, int FileTo);
	void Undo();
	void CollectMoves(int Rank, int File, TArray<int>& Ranks, TArray<int>& Files);
	void SetMoved(__int64 BitMask);
	void SetEnPassant(int Rank, int File);
	void CopyPositionFrom(FChessBoard& Board, bool bFlipSides = false);
	bool IsAttacked(int Rank, int File);
	void AllocateStack(int Size);

	static __int64 FlipMovedMask(__int64 OgMask);
private:
	bool AreCoordsValid(int Rank, int File);
	void CollectLineMovement(int Rank, int File, int DeltaRank, int DeltaFile, TArray<int>& Ranks, TArray<int>& Files);
	bool IsMoved(int Rank, int File);
	void LogMoved(int Rank, int File);
private:
	TArray<EChessPiece> Pieces;
	TArray<FRevertMove> MoveStack;
	__int64 MovedMask = 0;
	int EnPassantRank = -1;
	int EnPassantFile = -1;
};

enum class EGameState : int
{
	ActiveWhite,
	ActiveBlack,
	OverWhite,
	OverDraw,
	OverBlack
};

class FDoubleBoard
{
public:
	FDoubleBoard();
	DECLARE_NOCOPY(FDoubleBoard);
	DECLARE_MOVE(FDoubleBoard);
public:
	void EmptyBoard();
	void DefaultBoard();
	const EChessPiece& operator()(int Rank, int File);
	const EChessPiece& Square(int Rank, int File);
	EGameState GetGameState();

	void MovePiece(int RankFrom, int FileFrom, int RankTo, int FileTo);
	void Undo();
	void CollectMoves(int Rank, int File, TArray<int>& Ranks, TArray<int>& Files);

	void CopyPositionFrom(FChessBoard& Board);
	void CopyPositionFrom(FDoubleBoard& Board);

	void FlipBoard();
	void AllocateStack(int Size);
private:
	FChessBoard WhiteBoard;
	FChessBoard BlackBoard;
	bool bFlipped = false;
};




