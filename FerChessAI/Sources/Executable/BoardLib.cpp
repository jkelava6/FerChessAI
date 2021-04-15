
#include <StdH.h>
#include <Executable/BoardLib.h>

#include <Array.h>
#include <ChessBoard.h>

extern void GenerateGradingSet(TArray<FChessBoard>& GradingBoards)
{
	GradingBoards.Prealocate(4);

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.DefaultBoard();
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(1, 4) = EChessPiece::WhiteKing;
		Board(2, 6) = EChessPiece::WhiteRook;
		Board(2, 3) = EChessPiece::WhiteQueen;
		Board(6, 4) = EChessPiece::BlackKing;
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(2, 6) = EChessPiece::WhitePawn;
		Board(2, 7) = EChessPiece::WhitePawn;
		Board(3, 5) = EChessPiece::WhitePawn;
		Board(1, 5) = EChessPiece::WhiteKing;
		Board(0, 4) = EChessPiece::WhiteRook;

		Board(2, 0) = EChessPiece::WhitePawn;
		Board(3, 1) = EChessPiece::WhiteBishop;

		Board(1, 3) = EChessPiece::WhitePawn;
		Board(2, 3) = EChessPiece::WhiteKnight;

		Board(6, 0) = EChessPiece::BlackPawn;
		Board(6, 1) = EChessPiece::BlackPawn;
		Board(6, 3) = EChessPiece::BlackKnight;

		Board(5, 6) = EChessPiece::BlackPawn;
		Board(6, 7) = EChessPiece::BlackPawn;
		Board(6, 6) = EChessPiece::BlackKing;
		Board(6, 5) = EChessPiece::BlackKnight;
	}

	{
		FChessBoard& Board = GradingBoards.Push();
		Board.EmptyBoard();
		Board(1, 4) = EChessPiece::WhiteKing;
		Board(2, 4) = EChessPiece::WhitePawn;
		Board(3, 3) = EChessPiece::WhitePawn;
		Board(0, 3) = EChessPiece::WhiteRook;
		Board(1, 2) = EChessPiece::WhitePawn;

		Board(4, 2) = EChessPiece::BlackPawn;
		Board(5, 3) = EChessPiece::BlackPawn;
		Board(6, 4) = EChessPiece::BlackPawn;
		Board(7, 3) = EChessPiece::BlackRook;
		Board(5, 4) = EChessPiece::BlackKing;
	}
}