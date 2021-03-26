


#include <StdH.h>
#include <MinMaxAI.h>

#include <ChessBoard.h>
#include <Math.h>


FEvaluatedMove::FEvaluatedMove(int MoveRowFrom /*= -1*/, int MoveFileFrom /*= -1*/,
	int MoveRowTo /*= -1*/, int MoveFileTo /*= -1*/, float BoardEval /*= MINMAX_EVAL_LOSS*/) :
	Evaluation(BoardEval),
	RowFrom(MoveRowFrom),
	FileFrom(MoveFileFrom),
	RowTo(MoveRowTo),
	FileTo(MoveFileTo)
{
}

FMinMaxAI::FMinMaxAI()
{
}

bool FMinMaxAI::PlayMove(FDoubleBoard& Board)
{
	float Evaluation = MinMax(Board, -100.0f, -100.0f, 0, false);
	Board.Move(LastPlayedMove.RowFrom, LastPlayedMove.FileFrom, LastPlayedMove.RowTo, LastPlayedMove.FileTo);
	return true;
}

void FMinMaxAI::SetDepths(int Normal, int Volatile)
{
	MaxDepth = Normal;
	MaxVolatileDepth = Volatile;
}

float FMinMaxAI::Evaluate(FDoubleBoard& Board)
{
	float MaterialScore = 0.0f;
	float PositionalScore = 0.0f;
	constexpr float MaterialScoreMultiplier = 1000.0f;

	constexpr float RookRowScores[] = { 0.02f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.8f };
	bool bWhiteKingPresent = false;
	bool bBlackKingPresent = false;
	bool WhitePawns[8] = { false };
	bool BlackPawns[8] = { false };
	bool WhiteRooks[8] = { false };
	bool BlackRooks[8] = { false };
	constexpr float DoublePawnsPenalty = 3.0f;
	constexpr float DoubleRooksReward = 3.0f;

	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			FChessPiece Piece = Board(Row, File);
			if (Piece == FChessPiece::None)
			{
				continue;
			}

			bool bWhite = Piece > FChessPiece::None;
			FChessPiece PieceValue = bWhite ? Piece : (FChessPiece)-(int)Piece;
			float PieceColor = bWhite ? 1.0f : -1.0f;

			switch (Piece)
			{
			case FChessPiece::WhitePawn:
			{
				MaterialScore += 1.0f;
				const float CentralizedScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float AdvancedScore = (Row - 1.0f) / 5.0f;
				PositionalScore += 1.5f * (CentralizedScore + AdvancedScore + CentralizedScore * 3 * AdvancedScore) / 5.0f;
				if (WhitePawns[File])
				{
					PositionalScore -= DoublePawnsPenalty;
				}
				WhitePawns[File] = true;
				break;
			}
			case FChessPiece::WhiteKnight:
			{
				MaterialScore += 3.0f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore += (RowScore + FileScore + RowScore * FileScore) / 3.0f;
				break;
			}
			case FChessPiece::WhiteBishop:
			{
				MaterialScore += 3.1f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RowScore);
				PositionalScore += 0.8f * (RowScore + DiagonalScore) / 2.0f;
				break;
			}
			case FChessPiece::WhiteRook:
			{
				MaterialScore += 5.0f;
				PositionalScore += RookRowScores[Row];
				if (WhiteRooks[File])
				{
					PositionalScore += DoubleRooksReward;
				}
				WhiteRooks[File] = true;
				break;
			}
			case FChessPiece::WhiteQueen:
			{
				MaterialScore += 9.0f;
				break;
			}
			case FChessPiece::WhiteKing:
			{
				bWhiteKingPresent = true;
				break;
			}
			case FChessPiece::BlackPawn:
			{
				MaterialScore -= 1.0f;
				const float CentralizedScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float AdvancedScore = (6.0f - Row) / 5.0f;
				PositionalScore -= 1.5f * (CentralizedScore + AdvancedScore + CentralizedScore * 3 * AdvancedScore) / 5.0f;
				if (BlackPawns[File])
				{
					PositionalScore += DoublePawnsPenalty;
				}
				BlackPawns[File] = true;
				break;
			}
			case FChessPiece::BlackKnight:
			{
				MaterialScore -= 3.0f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore -= (RowScore + FileScore + RowScore * FileScore) / 3.0f;
				break;
			}
			case FChessPiece::BlackBishop:
			{
				MaterialScore -= 3.1f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RowScore);
				PositionalScore -= 0.8f * (RowScore + DiagonalScore) / 2.0f;
				break;
			}
			case FChessPiece::BlackRook:
				MaterialScore -= 5.0f;
				PositionalScore -= RookRowScores[7 - Row];
				if (BlackRooks[File])
				{
					PositionalScore -= DoubleRooksReward;
				}
				BlackRooks[File] = true;
				break;
			case FChessPiece::BlackQueen:
			{
				MaterialScore -= 9.0f;
				break;
			}
			case FChessPiece::BlackKing:
			{
				bBlackKingPresent = true;
				break;
			}
			default:
				break;
			}
		}
	}

	if (!bWhiteKingPresent)
	{
		return MINMAX_EVAL_LOSS;
	}

	if (!bBlackKingPresent)
	{
		return MINMAX_EVAL_WIN;
	}

	return MaterialScore + PositionalScore / MaterialScoreMultiplier;
}

float FMinMaxAI::MinMax(FDoubleBoard& Board, float Alfa, float Beta, int Depth, bool bVolatile)
{
	if (Depth >= (bVolatile ? MaxVolatileDepth : MaxDepth))
	{
		return Evaluate(Board);
	}

	const float CurrentBoardEval = Evaluate(Board);
	if (AbsF(CurrentBoardEval) == MINMAX_EVAL_WIN)
	{
		return CurrentBoardEval;
	}

	FEvaluatedMove BestMove;

	for (int Row = 7; Row >= 0; --Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			if (Board(Row, File) <= FChessPiece::None)
			{
				continue;
			}

			TArray<int> Rows;
			TArray<int> Files;
			Board.CollectMoves(Row, File, Rows, Files);

			const int MoveCount = Rows.Count();
			for (int Move = 0; Move < MoveCount; ++Move)
			{
				bool bPieceTaken = Board(Rows[Move], Files[Move]) < FChessPiece::None;
				Board.Move(Row, File, Rows[Move], Files[Move]);
				Board.FlipBoard();
				const float MoveEval = -MinMax(Board, Max(BestMove.Evaluation, Beta), Alfa, Depth + 1, bPieceTaken);
				Board.FlipBoard();
				Board.Undo();

				if (MoveEval > BestMove.Evaluation)
				{
					BestMove.Evaluation = MoveEval;
					BestMove.RowFrom = Row;
					BestMove.FileFrom = File;
					BestMove.RowTo = Rows[Move];
					BestMove.FileTo = Files[Move];

					if (-MoveEval < Alfa)
					{
						if (Depth == 0)
						{
							LastPlayedMove = BestMove;
						}
						return MoveEval;
					}
				}
			}
		}
	}

	if (Depth == 0)
	{
		LastPlayedMove = BestMove;
	}
	return BestMove.Evaluation;
}










