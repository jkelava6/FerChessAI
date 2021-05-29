

#include <StdH.h>
#include <ChessBoard.h>

#include <ChessMath.h>
#include <MinMaxAI.h>
#include <JavaIO/JavaTokenIO.h>

#ifdef _DEBUG
#include <cassert>
#endif


FRevertSquare::FRevertSquare() = default;

FRevertSquare::FRevertSquare(int ChangedRank, int ChangedFile, EChessPiece OldPiece) :
	Rank(ChangedRank),
	File(ChangedFile),
	Piece(OldPiece)
{
}

void FRevertSquare::Restore(FChessBoard& Board)
{
#ifdef _DEBUG
	assert(EChessPiece::BlackKing <= Piece && Piece <= EChessPiece::WhiteKing);
#endif
	Board(Rank, File) = Piece;
}

FRevertMove::FRevertMove() :
	ChangedInOrder(4)
{
}

void FRevertMove::Change(FChessBoard& Board, int Rank, int File, EChessPiece Piece)
{
	ChangedInOrder.Push() = FRevertSquare(Rank, File, Board(Rank, File));
	Board(Rank, File) = Piece;
}

void FRevertMove::Revert(FChessBoard& Board)
{
	for (; ChangedInOrder.Count() > 0;)
	{
		ChangedInOrder.Top().Restore(Board);
		ChangedInOrder.Pop();
	}

	Board.SetMoved(MovedMask);
	Board.SetEnPassant(EnPassantRank, EnPassantFile);
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

FRevertMove FRevertMove::Flipped()
{
	FRevertMove FlipMove;
	const int NumOfChanges = ChangedInOrder.Count();
	for (int Change = 0; Change < NumOfChanges; ++Change)
	{
		FRevertSquare& Turned = FlipMove.ChangedInOrder.Push();
		FRevertSquare& Own = ChangedInOrder[Change];
		Turned.Rank = RANKS - 1 - Own.Rank;
		Turned.File = Own.File;
		Turned.Piece = Own.Piece;
	}
	FlipMove.EnPassantRank = EnPassantRank;
	FlipMove.EnPassantFile = EnPassantFile;
	FlipMove.MovedMask = FChessBoard::FlipMovedMask(FlipMove.MovedMask);

	return Move(FlipMove);
}

FChessBoard::FChessBoard()
{
	const int NumOfSquares = RANKS * FILES;
	Pieces.Prealocate(NumOfSquares);
	for (int Square = 0; Square < NumOfSquares; ++Square)
	{
		Pieces.Push() = EChessPiece::None;
	}
}

FChessBoard::~FChessBoard()
{
}

FChessBoard& FChessBoard::operator= (FChessBoard&& Moved)
{
	TArray<EChessPiece> MyPieces = Move(Pieces);
	Pieces = Move(Moved.Pieces);
	Moved.Pieces = Move(MyPieces);

	MoveStack = Move(Moved.MoveStack);
	MovedMask = Moved.MovedMask;
	SetEnPassant(Moved.EnPassantRank, Moved.EnPassantFile);

	return *this;
}

FChessBoard::FChessBoard(FChessBoard&& Moved)
{
	*this = Move(Moved);
}

void FChessBoard::EmptyBoard()
{
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			Square(Rank, File) = EChessPiece::None;
		}
	}
}

void FChessBoard::DefaultBoard()
{
#if DEFAULT_BOARD
	// empty space
	for (int Rank = 2; Rank < 6; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			Square(Rank, File) = EChessPiece::None;
		}
	}

	// pawns
	for (int File = 0; File < 8; ++File)
	{
		Square(1, File) = EChessPiece::WhitePawn;
		Square(6, File) = EChessPiece::BlackPawn;
	}

	// white pieces
	Square(0, 0) = EChessPiece::WhiteRook;
	Square(0, 1) = EChessPiece::WhiteKnight;
	Square(0, 2) = EChessPiece::WhiteBishop;
	Square(0, 3) = EChessPiece::WhiteQueen;
	Square(0, 4) = EChessPiece::WhiteKing;
	Square(0, 5) = EChessPiece::WhiteBishop;
	Square(0, 6) = EChessPiece::WhiteKnight;
	Square(0, 7) = EChessPiece::WhiteRook;

	// black pieces
	Square(7, 0) = EChessPiece::BlackRook;
	Square(7, 1) = EChessPiece::BlackKnight;
	Square(7, 2) = EChessPiece::BlackBishop;
	Square(7, 3) = EChessPiece::BlackQueen;
	Square(7, 4) = EChessPiece::BlackKing;
	Square(7, 5) = EChessPiece::BlackBishop;
	Square(7, 6) = EChessPiece::BlackKnight;
	Square(7, 7) = EChessPiece::BlackRook;
#elif RANKS == 6 && FILES == 6
	// empty space
	for (int Rank = 2; Rank < 4; ++Rank)
	{
		for (int File = 0; File < 6; ++File)
		{
			Square(Rank, File) = EChessPiece::None;
}
	}

	// pawns
	for (int File = 0; File < 6; ++File)
	{
		Square(1, File) = EChessPiece::WhitePawn;
		Square(4, File) = EChessPiece::BlackPawn;
	}

	// white pieces
	Square(0, 0) = EChessPiece::WhiteRook;
	Square(0, 1) = EChessPiece::WhiteKnight;
	Square(0, 2) = EChessPiece::None;
	Square(0, 3) = EChessPiece::WhiteKing;
	Square(0, 4) = EChessPiece::WhiteKnight;
	Square(0, 5) = EChessPiece::WhiteRook;

	// black pieces
	Square(5, 0) = EChessPiece::BlackRook;
	Square(5, 1) = EChessPiece::BlackKnight;
	Square(5, 2) = EChessPiece::None;
	Square(5, 3) = EChessPiece::BlackKing;
	Square(5, 4) = EChessPiece::BlackKnight;
	Square(5, 5) = EChessPiece::BlackRook;
#else
	EmptyBoard();
	Square(0, 0) = EChessPiece::WhiteKing;
	Square(RANKS - 1, FILES - 1) = EChessPiece::BlackKing;
#endif
}

EChessPiece& FChessBoard::operator()(int Rank, int File)
{
	return Pieces[FILES * Rank + File];
}

EChessPiece& FChessBoard::Square(int Rank, int File)
{
	return Pieces[FILES * Rank + File];
}

void FChessBoard::MovePiece(int RankFrom, int FileFrom, int RankTo, int FileTo)
{
	FRevertMove& MoveLog = MoveStack.Push();
	EChessPiece MovedPiece = Square(RankFrom, FileFrom);
	if (MovedPiece == EChessPiece::WhitePawn && RankTo == RANKS - 1)
	{
		MoveLog.Change(*this, RankTo, FileTo, EChessPiece::WhiteQueen);
	}
	else if (MovedPiece == EChessPiece::BlackPawn && RankTo == 0)
	{
		MoveLog.Change(*this, RankTo, FileTo, EChessPiece::BlackQueen);
	}
	else
	{
		MoveLog.Change(*this, RankTo, FileTo, MovedPiece);
	}
	MoveLog.Change(*this, RankFrom, FileFrom, EChessPiece::None);

	// en passant
	if (RankTo == EnPassantRank && FileTo == EnPassantFile)
	{
		MoveLog.Change(*this, RankFrom, FileTo, EChessPiece::None);
	}

	MoveLog.SaveEnPassant(EnPassantRank, EnPassantFile);
	if ((MovedPiece == EChessPiece::WhitePawn || MovedPiece == EChessPiece::BlackPawn)
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

#if DEFAULT_BOARD
	// castling
	if (MovedPiece == EChessPiece::WhiteKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 0, 5, EChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 7, EChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 0, 3, EChessPiece::WhiteRook);
			MoveLog.Change(*this, 0, 0, EChessPiece::None);
		}
	}
	if (MovedPiece == EChessPiece::BlackKing && AbsI(FileTo - FileFrom) == 2)
	{
		if (FileTo == 6)
		{
			if (FileFrom != 4)
			{
				WriteJavaToken("error", "where from?!");
			}
			MoveLog.Change(*this, 7, 5, EChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 7, EChessPiece::None);
		}
		else
		{
			MoveLog.Change(*this, 7, 3, EChessPiece::BlackRook);
			MoveLog.Change(*this, 7, 0, EChessPiece::None);
		}
	}
#endif
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
	case EChessPiece::WhitePawn:
	{
		if (Square(Rank + 1, File) == EChessPiece::None)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File;
			if (Rank == 1 && Square(Rank + 2, File) == EChessPiece::None)
			{
				Ranks.Push() = Rank + 2;
				Files.Push() = File;
			}
		}
		if (File > 0 && Square(Rank + 1, File - 1) < EChessPiece::None
			|| Rank + 1 == EnPassantRank && File - 1 == EnPassantFile)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File - 1;
		}
		if (File < RANKS - 1 && Square(Rank + 1, File + 1) < EChessPiece::None
			|| Rank + 1 == EnPassantRank && File + 1 == EnPassantFile)
		{
			Ranks.Push() = Rank + 1;
			Files.Push() = File + 1;
		}
		break;
	}
	case EChessPiece::WhiteKnight:
	{
		constexpr int DeltaRank[] = { -2, -1, 1, 2, 2, 1, -1, -2 };
		constexpr int DeltaFile[] = { -1, -2, -2, -1, 1, 2, 2, 1 };
		for (int Move = 0; Move < ARRAY_SIZE(DeltaRank); ++Move)
		{
			const int NewRank = Rank + DeltaRank[Move];
			const int NewFile = File + DeltaFile[Move];
			if (!AreCoordsValid(NewRank, NewFile) || Square(NewRank, NewFile) > EChessPiece::None)
			{
				continue;
			}
			Ranks.Push() = NewRank;
			Files.Push() = NewFile;
		}
		break;
	}
	case EChessPiece::WhiteBishop:
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
	case EChessPiece::WhiteRook:
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
	case EChessPiece::WhiteQueen:
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
	case EChessPiece::WhiteKing:
	{
		constexpr int DeltaRankAll[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		constexpr int DeltaFileAll[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		ARRAY_MATCH(DeltaRankAll, DeltaFileAll);
		for (int Direction = 0; Direction < ARRAY_SIZE(DeltaRankAll); ++Direction)
		{
			const int NewRank = Rank + DeltaRankAll[Direction];
			const int NewFile = File + DeltaFileAll[Direction];
			if (AreCoordsValid(NewRank, NewFile) && Square(NewRank, NewFile) <= EChessPiece::None)
			{
				Ranks.Push() = NewRank;
				Files.Push() = NewFile;
			}
		}
#if DEFAULT_BOARD
		// castling
		if (!IsMoved(Rank, File) && !IsAttacked(Rank, File))
		{
			if (Square(0, 0) == EChessPiece::WhiteRook && !IsMoved(0, 0)
				&& Square(0, 3) == EChessPiece::None && Square(0, 2) == EChessPiece::None && !IsAttacked(0, 3))
			{
				Ranks.Push() = 0;
				Files.Push() = 2;
			}
			if (Square(0, 7) == EChessPiece::WhiteRook && !IsMoved(0, 7)
				&& Square(0, 5) == EChessPiece::None && Square(0, 6) == EChessPiece::None && !IsAttacked(0, 5))
			{
				Ranks.Push() = 0;
				Files.Push() = 6;
			}
		}
#endif
		break;
	}
	default:
	{
		break;
	}
	}
}

__int64 FChessBoard::FlipMovedMask(__int64 OgMask)
{
	__int64 FlippedMask = 0;
	__int64 Bit = 1;
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			int Square = Rank * FILES + File;
			int RevSquare = (FILES - 1 - Rank) * FILES + File;
			if (OgMask & (Bit << Square))
			{
				FlippedMask |= (Bit << (RANKS * FILES - 1 - Square));
			}
		}
	}
	return FlippedMask;
}

bool FChessBoard::AreCoordsValid(int Rank, int File)
{
	return 0 <= Rank && Rank < RANKS && 0 <= File && File < FILES;
}

void FChessBoard::CollectLineMovement(int Rank, int File, int DeltaRank, int DeltaFile, TArray<int>& Ranks, TArray<int>& Files)
{
	int NewRank = Rank + DeltaRank;
	int NewFile = File + DeltaFile;
	while (AreCoordsValid(NewRank, NewFile))
	{
		if (Square(NewRank, NewFile) <= EChessPiece::None)
		{
			Ranks.Push() = NewRank;
			Files.Push() = NewFile;
		}
		if (Square(NewRank, NewFile) != EChessPiece::None)
		{
			break;
		}
		NewRank += DeltaRank;
		NewFile += DeltaFile;
	}
}

bool FChessBoard::IsMoved(int Rank, int File)
{
	return MovedMask & ((__int64)1 << (FILES * Rank + File));
}

void FChessBoard::LogMoved(int Rank, int File)
{
	MovedMask |= ((__int64)1 << (FILES * Rank + File));
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
			if (Square(CheckRank, CheckFile) == EChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRank, CheckFile))
			{
				const EChessPiece Piece = Square(CheckRank, CheckFile);
				if (Piece == EChessPiece::BlackBishop || Piece == EChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != EChessPiece::None)
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
			if (Square(CheckRank, CheckFile) == EChessPiece::BlackKing)
			{
				return true;
			}
			while (AreCoordsValid(CheckRank, CheckFile))
			{
				const EChessPiece Piece = Square(CheckRank, CheckFile);
				if (Piece == EChessPiece::BlackRook || Piece == EChessPiece::BlackQueen)
				{
					return true;
				}
				if (Piece != EChessPiece::None)
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
		if (AreCoordsValid(CheckRank, CheckFile) && Square(CheckRank, CheckFile) == EChessPiece::BlackKnight)
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

void FChessBoard::CopyPositionFrom(FChessBoard& Board, bool bFlipSides/* = false*/)
{
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			Square(Rank, File) = !bFlipSides ? Board(Rank, File) : Board(7 - Rank, File);
		}
	}

	if (!bFlipSides)
	{
		SetMoved(Board.MovedMask);
		SetEnPassant(Board.EnPassantRank, Board.EnPassantFile);
		MoveStack = Board.MoveStack;
	}
	else
	{
		SetMoved(FChessBoard::FlipMovedMask(Board.MovedMask));
		SetEnPassant(FILES - 1 - Board.EnPassantRank, Board.EnPassantFile);
		MoveStack.PopAll();
		const int NumOfMoves = Board.MoveStack.Count();
		MoveStack.Prealocate(NumOfMoves);
		for (int Move = 0; Move < NumOfMoves; ++Move)
		{
			MoveStack.Push() = Board.MoveStack[Move].Flipped();
		}
	}
}

FDoubleBoard::FDoubleBoard() = default;

void FDoubleBoard::EmptyBoard()
{
	bFlipped = false;
	WhiteBoard.EmptyBoard();
	BlackBoard.EmptyBoard();
}

void FDoubleBoard::DefaultBoard()
{
	bFlipped = false;
	WhiteBoard.DefaultBoard();
	BlackBoard.DefaultBoard();
}

const EChessPiece& FDoubleBoard::operator()(int Rank, int File)
{
	return !bFlipped ? WhiteBoard(Rank, File) : BlackBoard(Rank, File);
}

const EChessPiece& FDoubleBoard::Square(int Rank, int File)
{
	return !bFlipped ? WhiteBoard(Rank, File) : BlackBoard(Rank, File);
}

EGameState FDoubleBoard::GetGameState()
{
#if !OPTIMIZED_GAME_STATE_CHECK
	// This shouldn't happen in minmax AI play, unless 1-depth AI is used.
	if (FMinMaxAI::Eval(*this, 1, 1) == MINMAX_EVAL_WIN)
	{
		return !bFlipped ? EGameState::OverWhite : EGameState::OverBlack;
	}
#endif
	const float Eval = FMinMaxAI::Eval(*this, 2, 2);
	if (Eval != MINMAX_EVAL_LOSS)
	{
		return !bFlipped ? EGameState::ActiveWhite : EGameState::ActiveBlack;
	}

	int KingRank = -1;
	int KingFile = -1;
#ifdef _DEBUG
	int BlackKingRank = -1;
	int BlackKingFile = -1;
#endif
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			if (Square(Rank, File) == EChessPiece::WhiteKing)
			{
				KingRank = Rank;
				KingFile = File;
				
#ifdef _DEBUG
				continue;
#else
				Rank = RANKS;
				break;
#endif
			}
#ifdef _DEBUG
			if (Square(Rank, File) == EChessPiece::BlackKing)
			{
				BlackKingRank = Rank;
				BlackKingFile = File;
			}
#endif
		}
	}

#ifdef _DEBUG
	assert(KingRank != -1 && BlackKingRank != -1);
#endif

	bool bMated = !bFlipped ? WhiteBoard.IsAttacked(KingRank, KingFile) : BlackBoard.IsAttacked(KingRank, KingFile);
	if (bMated)
	{
		return bFlipped ? EGameState::OverWhite : EGameState::OverBlack;
	}

	return EGameState::OverDraw;
}
#undef _DEBUG

void FDoubleBoard::MovePiece(int RankFrom, int FileFrom, int RankTo, int FileTo)
{
	if (!bFlipped)
	{
		WhiteBoard.MovePiece(RankFrom, FileFrom, RankTo, FileTo);
		BlackBoard.MovePiece(RANKS - 1 - RankFrom, FileFrom, RANKS - 1 - RankTo, FileTo);
	}
	else
	{
		WhiteBoard.MovePiece(RANKS - 1 - RankFrom, FileFrom, RANKS - 1 - RankTo, FileTo);
		BlackBoard.MovePiece(RankFrom, FileFrom, RankTo, FileTo);
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

void FDoubleBoard::CopyPositionFrom(FChessBoard& Board)
{
	bFlipped = false;
	WhiteBoard.CopyPositionFrom(Board);
	BlackBoard.CopyPositionFrom(Board, false);
}

void FDoubleBoard::CopyPositionFrom(FDoubleBoard& Board)
{
	bFlipped = Board.bFlipped;
	WhiteBoard.CopyPositionFrom(Board.WhiteBoard);
	BlackBoard.CopyPositionFrom(Board.BlackBoard);
}

void FDoubleBoard::FlipBoard()
{
	bFlipped = !bFlipped;
}

