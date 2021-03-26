

#include <StdH.h>
#include <ChessBoard.h>

#include <Math.h>


FRevertSquare::FRevertSquare() = default;

FRevertSquare::FRevertSquare(int ChangedRank, int ChangedFile, FChessPiece OldPiece) :
	Rank(ChangedRank),
	File(ChangedFile),
	Piece(OldPiece)
{
}

#include <JavaIO/JavaTokenIO.h>
void FRevertSquare::Restore(FChessBoard& Board)
{
	if (Piece < FChessPiece::BlackKing || Piece > FChessPiece::WhiteKing)
	{
		WriteJavaToken("error", "invalid piece");
	}
	Board(Rank, File) = Piece;
}

FRevertMove::FRevertMove() = default;

void FRevertMove::Change(FChessBoard& Board, int Rank, int File, FChessPiece Piece)
{
	ChangedInOrder.Push() = FRevertSquare(Rank, File, Board(Rank, File));
	Board(Rank, File) = Piece;
}

void FRevertMove::Revert(FChessBoard& Board)
{
	bool bCastle = ChangedInOrder.Count() == 4;
	for (; ChangedInOrder.Count() > 0;)
	{
		ChangedInOrder.Top().Restore(Board);
		ChangedInOrder.Pop();
	}

	Board.SetMoved(MovedMask);
}

void FRevertMove::SaveMask(__int64 BitMask)
{
	MovedMask = BitMask;
}

void FRevertMove::SaveEnPassant(int Rank, int File)
{
	EnPassantRank = Rank;
	EnPassantFile = File;
}

FChessBoard::FChessBoard()
{
	Pieces = new FChessPiece[64];
}

FChessBoard::~FChessBoard()
{
	delete[] Pieces;
}

void FChessBoard::EmptyBoard()
{
	for (int Rank = 0; Rank < 8; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Rank, File) = FChessPiece::None;
		}
	}
}

void FChessBoard::DefaultBoard()
{
	// empty space
	for (int Rank = 2; Rank < 6; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Rank, File) = FChessPiece::None;
		}
	}

	// pawns
	for (int File = 0; File < 8; ++File)
	{
		Square(1, File) = FChessPiece::WhitePawn;
		Square(6, File) = FChessPiece::BlackPawn;
	}

	// white pieces
	Square(0, 0) = FChessPiece::WhiteRook;
	Square(0, 1) = FChessPiece::WhiteKnight;
	Square(0, 2) = FChessPiece::WhiteBishop;
	Square(0, 3) = FChessPiece::WhiteQueen;
	Square(0, 4) = FChessPiece::WhiteKing;
	Square(0, 5) = FChessPiece::WhiteBishop;
	Square(0, 6) = FChessPiece::WhiteKnight;
	Square(0, 7) = FChessPiece::WhiteRook;

	// black pieces
	Square(7, 0) = FChessPiece::BlackRook;
	Square(7, 1) = FChessPiece::BlackKnight;
	Square(7, 2) = FChessPiece::BlackBishop;
	Square(7, 3) = FChessPiece::BlackQueen;
	Square(7, 4) = FChessPiece::BlackKing;
	Square(7, 5) = FChessPiece::BlackBishop;
	Square(7, 6) = FChessPiece::BlackKnight;
	Square(7, 7) = FChessPiece::BlackRook;
}

FChessPiece& FChessBoard::operator()(int Rank, int File)
{
	return Pieces[8 * Rank + File];
}

FChessPiece& FChessBoard::Square(int Rank, int File)
{
	return Pieces[8 * Rank + File];
}

void FChessBoard::Move(int RankFrom, int FileFrom, int RankTo, int FileTo)
{
	FRevertMove& MoveLog = MoveStack.Push();
	FChessPiece MovedPiece = Square(RankFrom, FileFrom);
	if (MovedPiece == FChessPiece::WhitePawn && RankTo == 7)
	{
		MoveLog.Change(*this, RankTo, FileTo, FChessPiece::WhiteQueen);
	}
	else if (MovedPiece == FChessPiece::BlackPawn && RankTo == 0)
	{
		MoveLog.Change(*this, RankTo, FileTo, FChessPiece::BlackQueen);
	}
	else
	{
		MoveLog.Change(*this, RankTo, FileTo, MovedPiece);
	}
	MoveLog.Change(*this, RankFrom, FileFrom, FChessPiece::None);

	// en passant
	if (RankTo == EnPassantRank && FileTo == EnPassantFile)
	{
		MoveLog.Change(*this, RankFrom, FileTo, FChessPiece::None);
	}

	MoveLog.SaveEnPassant(EnPassantRank, EnPassantFile);
	if ((MovedPiece == FChessPiece::WhitePawn || MovedPiece == FChessPiece::BlackPawn)
		&& AbsI(RankTo - RankFrom) == 2)
	{
		SetEnPassant((RankFrom + RankTo) / 2, FileTo);
	}
	else
	{
		SetEnPassant(-1, -1);
	}

	MoveLog.SaveMask(MovedMask);
	LogMoved(RankTo, FileTo);

	// castling
	if (MovedPiece == FChessPiece::WhiteKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 0, 5, FChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 7, FChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 0, 3, FChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 0, FChessPiece::None);
		}
	}
	if (MovedPiece == FChessPiece::BlackKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 7, 5, FChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 7, FChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 7, 3, FChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 0, FChessPiece::None);
		}
	}
}

void FChessBoard::Undo()
{
	MoveStack.Top().Revert(*this);
	MoveStack.Pop();
}

void FChessBoard::CollectMoves(int Rank, int File, TArray<int>& Ranks, TArray<int>& Files)
{
	switch (Square(Rank, File))
	{
	case FChessPiece::WhitePawn:
	{
		if (Square(Rank + 1, File) == FChessPiece::None)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File;
			if (Rank == 1 && Square(Rank + 2, File) == FChessPiece::None)
			{
				Ranks.Push() = Rank + 2;
				Files.Push() = File;
			}
		}
		if (File > 0 && Square(Rank + 1, File - 1) < FChessPiece::None
			|| Rank + 1 == EnPassantRank && File - 1 == EnPassantFile)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File - 1;
		}
		if (File < 7 && Square(Rank + 1, File + 1) < FChessPiece::None
			|| Rank + 1 == EnPassantRank && File + 1 == EnPassantFile)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File + 1;
		}
		break;
	}
	case FChessPiece::WhiteKnight:
	{
		constexpr int DeltaRank[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
		constexpr int DeltaFile[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
		for (int Move = 0; Move < ARRAY_SIZE(DeltaRank); ++Move)
		{
			const int NewRank = Rank + DeltaRank[Move];
			const int NewFile = File + DeltaFile[Move];
			if (!AreCoordsValid(NewRank, NewFile) || Square(NewRank, NewFile) > FChessPiece::None)
			{
				continue;
			}
			Ranks.Push() = NewRank;
			Files.Push() = NewFile;
		}
		break;
	}
	case FChessPiece::WhiteBishop:
	{
		constexpr int DeltaRankAll[] = { -1, -1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 1, -1, 1 };
		ARRAY_MATCH(DeltaRankAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRankAll); ++Direction)
		{
			CollectLineMovement(Rank, File, DeltaRankAll[Direction], DeltaFileAll[Direction], Ranks, Files);
		}
		break;
	}
	case FChessPiece::WhiteRook:
	{
		constexpr int DeltaRankAll[] = { -1, 0, 1, 0 };
		constexpr int DeltaFileAll[] = { 0, -1, 0, 1 };
		ARRAY_MATCH(DeltaRankAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRankAll); ++Direction)
		{
			CollectLineMovement(Rank, File, DeltaRankAll[Direction], DeltaFileAll[Direction], Ranks, Files);
		}
		break;
	}
	case FChessPiece::WhiteQueen:
	{
		constexpr int DeltaRankAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRankAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRankAll); ++Direction)
		{
			CollectLineMovement(Rank, File, DeltaRankAll[Direction], DeltaFileAll[Direction], Ranks, Files);
		}
		break;
	}
	case FChessPiece::WhiteKing:
	{
		constexpr int DeltaRankAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRankAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRankAll); ++Direction)
		{
			const int NewRank = Rank + DeltaRankAll[Direction];
			const int NewFile = File + DeltaFileAll[Direction];
			if (AreCoordsValid(NewRank, NewFile) && Square(NewRank, NewFile) <= FChessPiece::None)
			{
				Ranks.Push() = NewRank;
				Files.Push() = NewFile;
			}
		}
		// castling
		if (!IsMoved(Rank, File) && !IsAttacked(Rank, File))
		{
			if (Square(0, 0) == FChessPiece::WhiteRook && !IsMoved(0, 0)
				&& Square(0, 3) == FChessPiece::None && Square(0, 2) == FChessPiece::None && !IsAttacked(0, 3))
			{
				Ranks.Push() = 0;
				Files.Push() = 2;
			}
			if (Square(0, 7) == FChessPiece::WhiteRook && !IsMoved(0, 7)
				&& Square(0, 5) == FChessPiece::None && Square(0, 6) == FChessPiece::None && !IsAttacked(0, 5))
			{
				Ranks.Push() = 0;
				Files.Push() = 6;
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

bool FChessBoard::AreCoordsValid(int Rank, int File)
{
	return 0 <= Rank && Rank < 8 && 0 <= File && File < 8;
}

void FChessBoard::CollectLineMovement(int Rank, int File, int DeltaRank, int DeltaFile, TArray<int>& Ranks, TArray<int>& Files)
{
	int NewRank = Rank + DeltaRank;
	int NewFile = File + DeltaFile;
	while (AreCoordsValid(NewRank, NewFile))
	{
		if (Square(NewRank, NewFile) <= FChessPiece::None)
		{
			Ranks.Push() = NewRank;
			Files.Push() = NewFile;
		}
		if (Square(NewRank, NewFile) != FChessPiece::None)
		{
			break;
		}
		NewRank += DeltaRank;
		NewFile += DeltaFile;
	}
}

bool FChessBoard::IsMoved(int Rank, int File)
{
	return MovedMask & ((__int64)1 << (8 * Rank + File));
}

void FChessBoard::LogMoved(int Rank, int File)
{
	MovedMask |= ((__int64)1 << (8 * Rank + File));
}

void FChessBoard::SetMoved(__int64 BitMask)
{
	MovedMask = BitMask;
}

bool FChessBoard::IsAttacked(int Rank, int File)
{
	const int DeltaRankDiagAll[] = { -1, -1, 1, 1 };
	const int DeltaFileDiagAll[] = { -1, 1, -1, 1 };
	ARRAY_MATCH(DeltaRankDiagAll, DeltaFileDiagAll);
	const int DeltaRankStraightAll[] = { -1, 0, 1, 0 };
	const int DeltaFileStraightAll[] = { 0, -1, 0, 1 };
	ARRAY_MATCH(DeltaRankStraightAll, DeltaFileStraightAll);
	const int DeltaRankKnightAll[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
	const int DeltaFileKnightAll[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
	ARRAY_MATCH(DeltaRankKnightAll, DeltaFileKnightAll);

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRankDiagAll); ++Dir)
	{
		const int DeltaRank = DeltaRankDiagAll[Dir];
		const int DeltaFile = DeltaFileDiagAll[Dir];
		int CheckRank = Rank + DeltaRank;
		int CheckFile = File + DeltaFile;
		if (AreCoordsValid(CheckRank, CheckFile))
		{
			if (Square(CheckRank, CheckFile) == FChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRank, CheckFile))
			{
				const FChessPiece Piece = Square(CheckRank, CheckFile);
				if (Piece == FChessPiece::BlackBishop || Piece == FChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != FChessPiece::None)
				{
					break;
				}
				CheckRank += DeltaRank;
				CheckFile += DeltaFile;
			}
		}
	}

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRankStraightAll); ++Dir)
	{
		const int DeltaRank = DeltaRankStraightAll[Dir];
		const int DeltaFile = DeltaFileStraightAll[Dir];
		int CheckRank = Rank + DeltaRank;
		int CheckFile = File + DeltaFile;
		if (AreCoordsValid(CheckRank, CheckFile))
		{
			if (Square(CheckRank, CheckFile) == FChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRank, CheckFile))
			{
				const FChessPiece Piece = Square(CheckRank, CheckFile);
				if (Piece == FChessPiece::BlackRook || Piece == FChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != FChessPiece::None)
				{
					break;
				}
				CheckRank += DeltaRank;
				CheckFile += DeltaFile;
			}
		}
	}

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRankKnightAll); ++Dir)
	{
		const int CheckRank = Rank + DeltaRankKnightAll[Dir];
		const int CheckFile = File + DeltaFileKnightAll[Dir];
		if (AreCoordsValid(CheckRank, CheckFile) && Square(CheckRank, CheckFile) == FChessPiece::BlackKnight)
		{
			return true;
		}
	}

	return false;
}

void FChessBoard::SetEnPassant(int Rank, int File)
{
	EnPassantRank = Rank;
	EnPassantFile = File;
}

void FChessBoard::CopyPositionFrom(FChessBoard& Board)
{
	for (int Rank = 0; Rank < 8; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Rank, File) = Board(Rank, File);
		}
	}

	SetMoved(Board.MovedMask);
	SetEnPassant(Board.EnPassantRank, Board.EnPassantFile);
	MoveStack = Board.MoveStack;
}

FDoubleBoard::FDoubleBoard() = default;

void FDoubleBoard::EmptyBoard()
{
	WhiteBoard.EmptyBoard();
	BlackBoard.EmptyBoard();
}

void FDoubleBoard::DefaultBoard()
{
	WhiteBoard.DefaultBoard();
	BlackBoard.DefaultBoard();
}

const FChessPiece& FDoubleBoard::operator()(int Rank, int File)
{
	return !bFlipped ? WhiteBoard(Rank, File) : BlackBoard(Rank, File);
}

const FChessPiece& FDoubleBoard::Square(int Rank, int File)
{
	return !bFlipped ? WhiteBoard(Rank, File) : BlackBoard(Rank, File);
}

void FDoubleBoard::Move(int RankFrom, int FileFrom, int RankTo, int FileTo)
{
	if (!bFlipped)
	{
		WhiteBoard.Move(RankFrom, FileFrom, RankTo, FileTo);
		BlackBoard.Move(7 - RankFrom, FileFrom, 7 - RankTo, FileTo);
	}
	else
	{
		WhiteBoard.Move(7 - RankFrom, FileFrom, 7 - RankTo, FileTo);
		BlackBoard.Move(RankFrom, FileFrom, RankTo, FileTo);
	}
}

void FDoubleBoard::Undo()
{
	WhiteBoard.Undo();
	BlackBoard.Undo();
}

void FDoubleBoard::CollectMoves(int Rank, int File, TArray<int>& Ranks, TArray<int>& Files)
{
	if (!bFlipped)
	{
		WhiteBoard.CollectMoves(Rank, File, Ranks, Files);
	}
	else
	{
		BlackBoard.CollectMoves(Rank, File, Ranks, Files);
	}
}

void FDoubleBoard::FlipBoard()
{
	bFlipped = !bFlipped;
}

