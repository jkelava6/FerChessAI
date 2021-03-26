


#include <StdH.h>
#include <MinMaxAI.h>

#include <ChessBoard.h>
#include <Math.h>


FEvaluatedMove::FEvaluatedMove(int MoveRankFrom /*= -1*/, int MoveFileFrom /*= -1*/,
	int MoveRankTo /*= -1*/, int MoveFileTo /*= -1*/, float BoardEval /*= MINMAX_EVAL_LOSS*/) :
	Evaluation(BoardEval),
	RankFrom(MoveRankFrom),
	FileFrom(MoveFileFrom),
	RankTo(MoveRankTo),
	FileTo(MoveFileTo)
{
}

FMinMaxAI::FMinMaxAI()
{
}

bool FMinMaxAI::PlayMove(FDoubleBoard& Board)
{
	float Evaluation = MinMax(Board, -100.0f, -100.0f, 0, false);
	Board.Move(LastPlayedMove.RankFrom, LastPlayedMove.FileFrom, LastPlayedMove.RankTo, LastPlayedMove.FileTo);
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

	constexpr float RookRankScores[] = { 0.02f, 0.01f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.8f };
	bool bWhiteKingPresent = false;
	bool bBlackKingPresent = false;
	bool WhitePawns[8] = { false };
	bool BlackPawns[8] = { false };
	bool WhiteRooks[8] = { false };
	bool BlackRooks[8] = { false };
	constexpr float DoublePawnsPenalty = 3.0f;
	constexpr float DoubleRooksReward = 3.0f;

	for (int Rank = 0; Rank < 8; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			FChessPiece Piece = Board(Rank, File);
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
				const float AdvancedScore = (Rank - 1.0f) / 5.0f;
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
				const float RankScore = (3.5f - AbsF(3.5f - Rank)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore += (RankScore + FileScore + RankScore * FileScore) / 3.0f;
				break;
			}
			case FChessPiece::WhiteBishop:
			{
				MaterialScore += 3.1f;
				const float RankScore = (3.5f - AbsF(3.5f - Rank)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RankScore);
				PositionalScore += 0.8f * (RankScore + DiagonalScore) / 2.0f;
				break;
			}
			case FChessPiece::WhiteRook:
			{
				MaterialScore += 5.0f;
				PositionalScore += RookRankScores[Rank];
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
				const float AdvancedScore = (6.0f - Rank) / 5.0f;
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
				const float RankScore = (3.5f - AbsF(3.5f - Rank)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				PositionalScore -= (RankScore + FileScore + RankScore * FileScore) / 3.0f;
				break;
			}
			case FChessPiece::BlackBishop:
			{
				MaterialScore -= 3.1f;
				const float RankScore = (3.5f - AbsF(3.5f - Rank)) / 3.0f;
				const float FileScore = (3.5f - AbsF(3.5f - File)) / 3.0f;
				const float DiagonalScore = 1.0f - AbsF(FileScore - RankScore);
				PositionalScore -= 0.8f * (RankScore + DiagonalScore) / 2.0f;
				break;
			}
			case FChessPiece::BlackRook:
				MaterialScore -= 5.0f;
				PositionalScore -= RookRankScores[7 - Rank];
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

	for (int Rank = 7; Rank >= 0; --Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			if (Board(Rank, File) <= FChessPiece::None)
			{
				continue;
			}

			TArray<int> Ranks;
			TArray<int> Files;
			Board.CollectMoves(Rank, File, Ranks, Files);

			const int MoveCount = Ranks.Count();
			for (int Move = 0; Move < MoveCount; ++Move)
			{
				bool bPieceTaken = Board(Ranks[Move], Files[Move]) < FChessPiece::None;
				Board.Move(Rank, File, Ranks[Move], Files[Move]);
				Board.FlipBoard();
				const float MoveEval = -MinMax(Board, Max(BestMove.Evaluation, Beta), Alfa, Depth + 1, bPieceTaken);
				Board.FlipBoard();
				Board.Undo();

				if (MoveEval > BestMove.Evaluation)
				{
					BestMove.Evaluation = MoveEval;
					BestMove.RankFrom = Rank;
					BestMove.FileFrom = File;
					BestMove.RankTo = Ranks[Move];
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










