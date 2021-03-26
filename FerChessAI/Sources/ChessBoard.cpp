

#include <StdH.h>
#include <ChessBoard.h>

#include <Math.h>


FRevertSquare::FRevertSquare() = default;

FRevertSquare::FRevertSquare(int ChangedRow, int ChangedFile, FChessPiece OldPiece) :
	Row(ChangedRow),
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
	Board(Row, File) = Piece;
}

FRevertMove::FRevertMove() = default;

void FRevertMove::Change(FChessBoard& Board, int Row, int File, FChessPiece Piece)
{
	ChangedInOrder.Push() = FRevertSquare(Row, File, Board(Row, File));
	Board(Row, File) = Piece;
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

void FRevertMove::SaveEnPassant(int Row, int File)
{
	EnPassantRow = Row;
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
	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Row, File) = FChessPiece::None;
		}
	}
}

void FChessBoard::DefaultBoard()
{
	// empty space
	for (int Row = 2; Row < 6; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Row, File) = FChessPiece::None;
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

FChessPiece& FChessBoard::operator()(int Row, int File)
{
	return Pieces[8 * Row + File];
}

FChessPiece& FChessBoard::Square(int Row, int File)
{
	return Pieces[8 * Row + File];
}

void FChessBoard::Move(int RowFrom, int FileFrom, int RowTo, int FileTo)
{
	FRevertMove& MoveLog = MoveStack.Push();
	FChessPiece MovedPiece = Square(RowFrom, FileFrom);
	if (MovedPiece == FChessPiece::WhitePawn && RowTo == 7)
	{
		MoveLog.Change(*this, RowTo, FileTo, FChessPiece::WhiteQueen);
	}
	else if (MovedPiece == FChessPiece::BlackPawn && RowTo == 0)
	{
		MoveLog.Change(*this, RowTo, FileTo, FChessPiece::BlackQueen);
	}
	else
	{
		MoveLog.Change(*this, RowTo, FileTo, MovedPiece);
	}
	MoveLog.Change(*this, RowFrom, FileFrom, FChessPiece::None);

	// en passant
	if (RowTo == EnPassantRow && FileTo == EnPassantFile)
	{
		MoveLog.Change(*this, RowFrom, FileTo, FChessPiece::None);
	}

	MoveLog.SaveEnPassant(EnPassantRow, EnPassantFile);
	if ((MovedPiece == FChessPiece::WhitePawn || MovedPiece == FChessPiece::BlackPawn)
		&& AbsI(RowTo - RowFrom) == 2)
	{
		SetEnPassant((RowFrom + RowTo) / 2, FileTo);
	}
	else
	{
		SetEnPassant(-1, -1);
	}

	MoveLog.SaveMask(MovedMask);
	LogMoved(RowTo, FileTo);

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

void FChessBoard::CollectMoves(int Row, int File, TArray<int>& Rows, TArray<int>& Files)
{
	switch (Square(Row, File))
	{
	case FChessPiece::WhitePawn:
	{
		if (Square(Row + 1, File) == FChessPiece::None)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File;
			if (Row == 1 && Square(Row + 2, File) == FChessPiece::None)
			{
				Rows.Push() = Row + 2;
				Files.Push() = File;
			}
		}
		if (File > 0 && Square(Row + 1, File - 1) < FChessPiece::None
			|| Row + 1 == EnPassantRow && File - 1 == EnPassantFile)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File - 1;
		}
		if (File < 7 && Square(Row + 1, File + 1) < FChessPiece::None
			|| Row + 1 == EnPassantRow && File + 1 == EnPassantFile)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File + 1;
		}
		break;
	}
	case FChessPiece::WhiteKnight:
	{
		constexpr int DeltaRow[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
		constexpr int DeltaFile[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
		for (int Move = 0; Move < ARRAY_SIZE(DeltaRow); ++Move)
		{
			const int NewRow = Row + DeltaRow[Move];
			const int NewFile = File + DeltaFile[Move];
			if (!AreCoordsValid(NewRow, NewFile) || Square(NewRow, NewFile) > FChessPiece::None)
			{
				continue;
			}
			Rows.Push() = NewRow;
			Files.Push() = NewFile;
		}
		break;
	}
	case FChessPiece::WhiteBishop:
	{
		constexpr int DeltaRowAll[] = { -1, -1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 1, -1, 1 };
		ARRAY_MATCH(DeltaRowAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRowAll); ++Direction)
		{
			CollectLineMovement(Row, File, DeltaRowAll[Direction], DeltaFileAll[Direction], Rows, Files);
		}
		break;
	}
	case FChessPiece::WhiteRook:
	{
		constexpr int DeltaRowAll[] = { -1, 0, 1, 0 };
		constexpr int DeltaFileAll[] = { 0, -1, 0, 1 };
		ARRAY_MATCH(DeltaRowAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRowAll); ++Direction)
		{
			CollectLineMovement(Row, File, DeltaRowAll[Direction], DeltaFileAll[Direction], Rows, Files);
		}
		break;
	}
	case FChessPiece::WhiteQueen:
	{
		constexpr int DeltaRowAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRowAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRowAll); ++Direction)
		{
			CollectLineMovement(Row, File, DeltaRowAll[Direction], DeltaFileAll[Direction], Rows, Files);
		}
		break;
	}
	case FChessPiece::WhiteKing:
	{
		constexpr int DeltaRowAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRowAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRowAll); ++Direction)
		{
			const int NewRow = Row + DeltaRowAll[Direction];
			const int NewFile = File + DeltaFileAll[Direction];
			if (AreCoordsValid(NewRow, NewFile) && Square(NewRow, NewFile) <= FChessPiece::None)
			{
				Rows.Push() = NewRow;
				Files.Push() = NewFile;
			}
		}
		// castling
		if (!IsMoved(Row, File) && !IsAttacked(Row, File))
		{
			if (Square(0, 0) == FChessPiece::WhiteRook && !IsMoved(0, 0)
				&& Square(0, 3) == FChessPiece::None && Square(0, 2) == FChessPiece::None && !IsAttacked(0, 3))
			{
				Rows.Push() = 0;
				Files.Push() = 2;
			}
			if (Square(0, 7) == FChessPiece::WhiteRook && !IsMoved(0, 7)
				&& Square(0, 5) == FChessPiece::None && Square(0, 6) == FChessPiece::None && !IsAttacked(0, 5))
			{
				Rows.Push() = 0;
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

bool FChessBoard::AreCoordsValid(int Row, int File)
{
	return 0 <= Row && Row < 8 && 0 <= File && File < 8;
}

void FChessBoard::CollectLineMovement(int Row, int File, int DeltaRow, int DeltaFile, TArray<int>& Rows, TArray<int>& Files)
{
	int NewRow = Row + DeltaRow;
	int NewFile = File + DeltaFile;
	while (AreCoordsValid(NewRow, NewFile))
	{
		if (Square(NewRow, NewFile) <= FChessPiece::None)
		{
			Rows.Push() = NewRow;
			Files.Push() = NewFile;
		}
		if (Square(NewRow, NewFile) != FChessPiece::None)
		{
			break;
		}
		NewRow += DeltaRow;
		NewFile += DeltaFile;
	}
}

bool FChessBoard::IsMoved(int Row, int File)
{
	return MovedMask & ((__int64)1 << (8 * Row + File));
}

void FChessBoard::LogMoved(int Row, int File)
{
	MovedMask |= ((__int64)1 << (8 * Row + File));
}

void FChessBoard::SetMoved(__int64 BitMask)
{
	MovedMask = BitMask;
}

bool FChessBoard::IsAttacked(int Row, int File)
{
	const int DeltaRowDiagAll[] = { -1, -1, 1, 1 };
	const int DeltaFileDiagAll[] = { -1, 1, -1, 1 };
	ARRAY_MATCH(DeltaRowDiagAll, DeltaFileDiagAll);
	const int DeltaRowStraightAll[] = { -1, 0, 1, 0 };
	const int DeltaFileStraightAll[] = { 0, -1, 0, 1 };
	ARRAY_MATCH(DeltaRowStraightAll, DeltaFileStraightAll);
	const int DeltaRowKnightAll[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
	const int DeltaFileKnightAll[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
	ARRAY_MATCH(DeltaRowKnightAll, DeltaFileKnightAll);

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRowDiagAll); ++Dir)
	{
		const int DeltaRow = DeltaRowDiagAll[Dir];
		const int DeltaFile = DeltaFileDiagAll[Dir];
		int CheckRow = Row + DeltaRow;
		int CheckFile = File + DeltaFile;
		if (AreCoordsValid(CheckRow, CheckFile))
		{
			if (Square(CheckRow, CheckFile) == FChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRow, CheckFile))
			{
				const FChessPiece Piece = Square(CheckRow, CheckFile);
				if (Piece == FChessPiece::BlackBishop || Piece == FChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != FChessPiece::None)
				{
					break;
				}
				CheckRow += DeltaRow;
				CheckFile += DeltaFile;
			}
		}
	}

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRowStraightAll); ++Dir)
	{
		const int DeltaRow = DeltaRowStraightAll[Dir];
		const int DeltaFile = DeltaFileStraightAll[Dir];
		int CheckRow = Row + DeltaRow;
		int CheckFile = File + DeltaFile;
		if (AreCoordsValid(CheckRow, CheckFile))
		{
			if (Square(CheckRow, CheckFile) == FChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRow, CheckFile))
			{
				const FChessPiece Piece = Square(CheckRow, CheckFile);
				if (Piece == FChessPiece::BlackRook || Piece == FChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != FChessPiece::None)
				{
					break;
				}
				CheckRow += DeltaRow;
				CheckFile += DeltaFile;
			}
		}
	}

	for (int Dir = 0; Dir < ARRAY_SIZE(DeltaRowKnightAll); ++Dir)
	{
		const int CheckRow = Row + DeltaRowKnightAll[Dir];
		const int CheckFile = File + DeltaFileKnightAll[Dir];
		if (AreCoordsValid(CheckRow, CheckFile) && Square(CheckRow, CheckFile) == FChessPiece::BlackKnight)
		{
			return true;
		}
	}

	return false;
}

void FChessBoard::SetEnPassant(int Row, int File)
{
	EnPassantRow = Row;
	EnPassantFile = File;
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

const FChessPiece& FDoubleBoard::operator()(int Row, int File)
{
	return !bFlipped ? WhiteBoard(Row, File) : BlackBoard(Row, File);
}

const FChessPiece& FDoubleBoard::Square(int Row, int File)
{
	return !bFlipped ? WhiteBoard(Row, File) : BlackBoard(Row, File);
}

void FDoubleBoard::Move(int RowFrom, int FileFrom, int RowTo, int FileTo)
{
	if (!bFlipped)
	{
		WhiteBoard.Move(RowFrom, FileFrom, RowTo, FileTo);
		BlackBoard.Move(7 - RowFrom, FileFrom, 7 - RowTo, FileTo);
	}
	else
	{
		WhiteBoard.Move(7 - RowFrom, FileFrom, 7 - RowTo, FileTo);
		BlackBoard.Move(RowFrom, FileFrom, RowTo, FileTo);
	}
}

void FDoubleBoard::Undo()
{
	WhiteBoard.Undo();
	BlackBoard.Undo();
}

void FDoubleBoard::CollectMoves(int Row, int File, TArray<int>& Rows, TArray<int>& Files)
{
	if (!bFlipped)
	{
		WhiteBoard.CollectMoves(Row, File, Rows, Files);
	}
	else
	{
		BlackBoard.CollectMoves(Row, File, Rows, Files);
	}
}

void FDoubleBoard::FlipBoard()
{
	bFlipped = !bFlipped;
}

