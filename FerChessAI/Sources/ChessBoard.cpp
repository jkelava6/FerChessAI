

#include <StdH.h>
#include <ChessBoard.h>

#include <Math.h>


RevertSquare::RevertSquare() = default;

RevertSquare::RevertSquare(int ChangedRow, int ChangedFile, ChessPiece OldPiece) :
	Row(ChangedRow),
	File(ChangedFile),
	Piece(OldPiece)
{
}

#include <JavaIO/JavaTokenIO.h>
void RevertSquare::Restore(ChessBoard& Board)
{
	if (Piece < ChessPiece::BlackKing || Piece > ChessPiece::WhiteKing)
	{
		WriteJavaToken("error", "invalid piece");
	}
	Board(Row, File) = Piece;
}

RevertMove::RevertMove() = default;

void RevertMove::Change(ChessBoard& Board, int Row, int File, ChessPiece Piece)
{
	ChangedInOrder.Push() = RevertSquare(Row, File, Board(Row, File));
	Board(Row, File) = Piece;
}

void RevertMove::Revert(ChessBoard& Board)
{
	bool bCastle = ChangedInOrder.Count() == 4;
	for (; ChangedInOrder.Count() > 0;)
	{
		ChangedInOrder.Top().Restore(Board);
		ChangedInOrder.Pop();
	}

	Board.SetMoved(MovedMask);
}

void RevertMove::SaveMask(__int64 BitMask)
{
	MovedMask = BitMask;
}

void RevertMove::SaveEnPassant(int Row, int File)
{
	EnPassantRow = Row;
	EnPassantFile = File;
}

ChessBoard::ChessBoard()
{
	Pieces = new ChessPiece[64];
}

ChessBoard::~ChessBoard()
{
	delete[] Pieces;
}

void ChessBoard::EmptyBoard()
{
	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Row, File) = ChessPiece::None;
		}
	}
}

void ChessBoard::DefaultBoard()
{
	// empty space
	for (int Row = 2; Row < 6; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Row, File) = ChessPiece::None;
		}
	}

	// pawns
	for (int File = 0; File < 8; ++File)
	{
		Square(1, File) = ChessPiece::WhitePawn;
		Square(6, File) = ChessPiece::BlackPawn;
	}

	// white pieces
	Square(0, 0) = ChessPiece::WhiteRook;
	Square(0, 1) = ChessPiece::WhiteKnight;
	Square(0, 2) = ChessPiece::WhiteBishop;
	Square(0, 3) = ChessPiece::WhiteQueen;
	Square(0, 4) = ChessPiece::WhiteKing;
	Square(0, 5) = ChessPiece::WhiteBishop;
	Square(0, 6) = ChessPiece::WhiteKnight;
	Square(0, 7) = ChessPiece::WhiteRook;

	// black pieces
	Square(7, 0) = ChessPiece::BlackRook;
	Square(7, 1) = ChessPiece::BlackKnight;
	Square(7, 2) = ChessPiece::BlackBishop;
	Square(7, 3) = ChessPiece::BlackQueen;
	Square(7, 4) = ChessPiece::BlackKing;
	Square(7, 5) = ChessPiece::BlackBishop;
	Square(7, 6) = ChessPiece::BlackKnight;
	Square(7, 7) = ChessPiece::BlackRook;
}

ChessPiece& ChessBoard::operator()(int Row, int File)
{
	return Pieces[8 * Row + File];
}

ChessPiece& ChessBoard::Square(int Row, int File)
{
	return Pieces[8 * Row + File];
}

void ChessBoard::Move(int RowFrom, int FileFrom, int RowTo, int FileTo)
{
	RevertMove& MoveLog = MoveStack.Push();
	ChessPiece MovedPiece = Square(RowFrom, FileFrom);
	if (MovedPiece == ChessPiece::WhitePawn && RowTo == 7)
	{
		MoveLog.Change(*this, RowTo, FileTo, ChessPiece::WhiteQueen);
	}
	else if (MovedPiece == ChessPiece::BlackPawn && RowTo == 0)
	{
		MoveLog.Change(*this, RowTo, FileTo, ChessPiece::BlackQueen);
	}
	else
	{
		MoveLog.Change(*this, RowTo, FileTo, MovedPiece);
	}
	MoveLog.Change(*this, RowFrom, FileFrom, ChessPiece::None);

	// en passant
	if (RowTo == EnPassantRow && FileTo == EnPassantFile)
	{
		MoveLog.Change(*this, RowFrom, FileTo, ChessPiece::None);
	}

	MoveLog.SaveEnPassant(EnPassantRow, EnPassantFile);
	if ((MovedPiece == ChessPiece::WhitePawn || MovedPiece == ChessPiece::BlackPawn)
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
	if (MovedPiece == ChessPiece::WhiteKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 0, 5, ChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 7, ChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 0, 3, ChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 0, ChessPiece::None);
		}
	}
	if (MovedPiece == ChessPiece::BlackKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 7, 5, ChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 7, ChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 7, 3, ChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 0, ChessPiece::None);
		}
	}
}

void ChessBoard::Undo()
{
	MoveStack.Top().Revert(*this);
	MoveStack.Pop();
}

void ChessBoard::CollectMoves(int Row, int File, Array<int>& Rows, Array<int>& Files)
{
	switch (Square(Row, File))
	{
	case ChessPiece::WhitePawn:
	{
		if (Square(Row + 1, File) == ChessPiece::None)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File;
			if (Row == 1 && Square(Row + 2, File) == ChessPiece::None)
			{
				Rows.Push() = Row + 2;
				Files.Push() = File;
			}
		}
		if (File > 0 && Square(Row + 1, File - 1) < ChessPiece::None
			|| Row + 1 == EnPassantRow && File - 1 == EnPassantFile)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File - 1;
		}
		if (File < 7 && Square(Row + 1, File + 1) < ChessPiece::None
			|| Row + 1 == EnPassantRow && File + 1 == EnPassantFile)
		{
			Rows.Push() = Row + 1;
			Files.Push() = File + 1;
		}
		break;
	}
	case ChessPiece::WhiteKnight:
	{
		constexpr int DeltaRow[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
		constexpr int DeltaFile[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
		for (int Move = 0; Move < ARRAY_SIZE(DeltaRow); ++Move)
		{
			const int NewRow = Row + DeltaRow[Move];
			const int NewFile = File + DeltaFile[Move];
			if (!AreCoordsValid(NewRow, NewFile) || Square(NewRow, NewFile) > ChessPiece::None)
			{
				continue;
			}
			Rows.Push() = NewRow;
			Files.Push() = NewFile;
		}
		break;
	}
	case ChessPiece::WhiteBishop:
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
	case ChessPiece::WhiteRook:
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
	case ChessPiece::WhiteQueen:
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
	case ChessPiece::WhiteKing:
	{
		constexpr int DeltaRowAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRowAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRowAll); ++Direction)
		{
			const int NewRow = Row + DeltaRowAll[Direction];
			const int NewFile = File + DeltaFileAll[Direction];
			if (AreCoordsValid(NewRow, NewFile) && Square(NewRow, NewFile) <= ChessPiece::None)
			{
				Rows.Push() = NewRow;
				Files.Push() = NewFile;
			}
		}
		// castling
		if (!IsMoved(Row, File) && !IsAttacked(Row, File))
		{
			if (Square(0, 0) == ChessPiece::WhiteRook && !IsMoved(0, 0)
				&& Square(0, 3) == ChessPiece::None && Square(0, 2) == ChessPiece::None && !IsAttacked(0, 3))
			{
				Rows.Push() = 0;
				Files.Push() = 2;
			}
			if (Square(0, 7) == ChessPiece::WhiteRook && !IsMoved(0, 7)
				&& Square(0, 5) == ChessPiece::None && Square(0, 6) == ChessPiece::None && !IsAttacked(0, 5))
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

bool ChessBoard::AreCoordsValid(int Row, int File)
{
	return 0 <= Row && Row < 8 && 0 <= File && File < 8;
}

void ChessBoard::CollectLineMovement(int Row, int File, int DeltaRow, int DeltaFile, Array<int>& Rows, Array<int>& Files)
{
	int NewRow = Row + DeltaRow;
	int NewFile = File + DeltaFile;
	while (AreCoordsValid(NewRow, NewFile))
	{
		if (Square(NewRow, NewFile) <= ChessPiece::None)
		{
			Rows.Push() = NewRow;
			Files.Push() = NewFile;
		}
		if (Square(NewRow, NewFile) != ChessPiece::None)
		{
			break;
		}
		NewRow += DeltaRow;
		NewFile += DeltaFile;
	}
}

bool ChessBoard::IsMoved(int Row, int File)
{
	return MovedMask & ((__int64)1 << (8 * Row + File));
}

void ChessBoard::LogMoved(int Row, int File)
{
	MovedMask |= ((__int64)1 << (8 * Row + File));
}

void ChessBoard::SetMoved(__int64 BitMask)
{
	MovedMask = BitMask;
}

bool ChessBoard::IsAttacked(int Row, int File)
{
	return false;
}

void ChessBoard::SetEnPassant(int Row, int File)
{
	EnPassantRow = Row;
	EnPassantFile = File;
}

DoubleBoard::DoubleBoard() = default;

void DoubleBoard::EmptyBoard()
{
	WhiteBoard.EmptyBoard();
	BlackBoard.EmptyBoard();
}

void DoubleBoard::DefaultBoard()
{
	WhiteBoard.DefaultBoard();
	BlackBoard.DefaultBoard();
}

const ChessPiece& DoubleBoard::operator()(int Row, int File)
{
	return !bFlipped ? WhiteBoard(Row, File) : BlackBoard(Row, File);
}

const ChessPiece& DoubleBoard::Square(int Row, int File)
{
	return !bFlipped ? WhiteBoard(Row, File) : BlackBoard(Row, File);
}

void DoubleBoard::Move(int RowFrom, int FileFrom, int RowTo, int FileTo)
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

void DoubleBoard::Undo()
{
	WhiteBoard.Undo();
	BlackBoard.Undo();
}

void DoubleBoard::CollectMoves(int Row, int File, Array<int>& Rows, Array<int>& Files)
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

void DoubleBoard::FlipBoard()
{
	bFlipped = !bFlipped;
}

