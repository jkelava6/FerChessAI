


#include <StdH.h>
#include <MinMaxAI.h>

#include <ChessBoard.h>
#include <Math.h>


EvaluatedMove::EvaluatedMove(int MoveRowFrom /*= -1*/, int MoveFileFrom /*= -1*/,
	int MoveRowTo /*= -1*/, int MoveFileTo /*= -1*/, float BoardEval /*= MINMAX_EVAL_LOSS*/) :
	Evaluation(BoardEval),
	RowFrom(MoveRowFrom),
	FileFrom(MoveFileFrom),
	RowTo(MoveRowTo),
	FileTo(MoveFileTo)
{
}

MinMaxAI::MinMaxAI()
{
}

void MinMaxAI::PlayMove(DoubleBoard& Board)
{
	float Evaluation = MinMax(Board, -100.0f, -100.0f, 0, false);
	Board.Move(LastPlayedMove.RowFrom, LastPlayedMove.FileFrom, LastPlayedMove.RowTo, LastPlayedMove.FileTo);
}

float MinMaxAI::Evaluate(DoubleBoard& Board)
{
	float MaterialScore = 0.0f;
	float PositionalScore = 0.0f;
	constexpr float MaterialScoreMultiplier = 1000.0f;

	constexpr float RookRowScores[] = { 0.3f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.8f };
	bool bWhiteKingPresent = false;
	bool bBlackKingPresent = false;

	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			ChessPiece Piece = Board(Row, File);
			if (Piece == ChessPiece::None)
			{
				continue;
			}

			bool bWhite = Piece > ChessPiece::None;
			ChessPiece PieceValue = bWhite ? Piece : (ChessPiece)-(int)Piece;
			float PieceColor = bWhite ? 1.0f : -1.0f;

			switch (Piece)
			{
			case ChessPiece::WhitePawn:
			{
				MaterialScore += 1.0f;
				const float CentralizedScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float AdvancedScore = (Row - 1.0f) / 5.0f;
				PositionalScore += (CentralizedScore + AdvancedScore + CentralizedScore * AdvancedScore) / 3.0f;
				break;
			}
			case ChessPiece::WhiteKnight:
			{
				MaterialScore += 3.0f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore += (RowScore + FileScore + RowScore * FileScore) / 3.0f;
				break;
			}
			case ChessPiece::WhiteBishop:
			{
				MaterialScore += 3.0;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RowScore);
				PositionalScore += (2.0f * RowScore + FileScore + 2.0f * DiagonalScore) / 5.0f;
				break;
			}
			case ChessPiece::WhiteRook:
			{
				MaterialScore += 5.0f;
				PositionalScore += RookRowScores[Row];
				break;
			}
			case ChessPiece::WhiteQueen:
			{
				MaterialScore += 9.0f;
				break;
			}
			case ChessPiece::WhiteKing:
			{
				bWhiteKingPresent = true;
				break;
			}
			case ChessPiece::BlackPawn:
			{
				MaterialScore -= 1.0f;
				const float CentralizedScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float AdvancedScore = (6.0f - Row) / 5.0f;
				PositionalScore -= (CentralizedScore + AdvancedScore + CentralizedScore * AdvancedScore) / 3.0f;
				break;
			}
			case ChessPiece::BlackKnight:
			{
				MaterialScore -= 3.0f;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore -= (RowScore + FileScore + RowScore * FileScore) / 3.0f;
				break;
			}
			case ChessPiece::BlackBishop:
			{
				MaterialScore -= 3.0;
				const float RowScore = (3.5f - AbsF(3.5f - Row)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RowScore);
				PositionalScore -= (2.0f * RowScore + FileScore + 2.0f * DiagonalScore) / 5.0f;
				break;
			}
			case ChessPiece::BlackRook:
				MaterialScore -= 5.0f;
				PositionalScore -= RookRowScores[7 - Row];
				break;
			case ChessPiece::BlackQueen:
			{
				MaterialScore -= 9.0f;
				break;
			}
			case ChessPiece::BlackKing:
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

float MinMaxAI::MinMax(DoubleBoard& Board, float Alfa, float Beta, int Depth, bool bVolatile)
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

	EvaluatedMove BestMove;

	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			if (Board(Row, File) <= ChessPiece::None)
			{
				continue;
			}

			Array<int> Rows;
			Array<int> Files;
			Board.CollectMoves(Row, File, Rows, Files);

			const int MoveCount = Rows.Count();
			for (int Move = 0; Move < MoveCount; ++Move)
			{
				bool bPieceTaken = Board(Rows[Move], Files[Move]) < ChessPiece::None;
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










