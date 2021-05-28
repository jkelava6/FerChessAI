#include <StdH.h>
#include <League/League.h>

#include <ChessBoard.h>
#include <NeuNet/Node.h>

void FLeague::Initialize(int PopCount, int PopSize, int MaxMiddleNodes, int MaxRecurrentNodes)
{
	Populations.Clear();
	Populations.Prealocate(PopCount);
	Ratings.Clear();
	Ratings.Prealocate(PopCount);
	for (int Index = 0; Index < PopCount; ++Index)
	{
		FPopulation& Pop = Populations.Push();
		Pop.Initialize(PopSize, MaxMiddleNodes, MaxRecurrentNodes);
		Ratings.Push() = 1000;
	}
}

void FLeague::Iterate()
{
	const int PopCount = Populations.Count();
	for (int Index = 0; Index < PopCount; ++Index)
	{
		Populations[Index].PlayInLeague(*this);
	}
	for (int Index = 0; Index < PopCount; ++Index)
	{
		Populations[Index].NextGeneration(*this);
	}
}

void FLeague::PlayAI(IChessAI& Challenger, FPopulation* Population, int UnitId, bool bRated)
{
	const int ChallengingPop = Populations.IndexOf(Population);

	FDoubleBoard Board;
	for (int Index = 0; Index < Populations.Count(); ++Index)
	{
		if (Index == ChallengingPop)
		{
			continue;
		}

		IChessAI& Representative = Populations[Index].Representative();
		int WhiteMoves;
		PlayGame(Board, Challenger, Representative, WhiteMoves);
		const float WhiteScore = GameScore(Board);
		int BlackMoves;
		PlayGame(Board, Representative, Challenger, BlackMoves);
		const float BlackScore = GameScore(Board);

		if (bRated)
		{
			RateGame(Board, ChallengingPop, Index);
			RateGame(Board, Index, ChallengingPop);
		}

		Population->GradeMatch(UnitId, 0.5f * (1.0f + WhiteScore), WhiteMoves, 0.5f * (1.0f + BlackScore), BlackMoves);
	}
}

void FLeague::GetAIs(TArray<IChessAI*>& OutTempAIs)
{
	for (int Index = 0; Index < Populations.Count(); ++Index)
	{
		OutTempAIs.Push() = &Populations[Index].Representative();
	}
}

EGameState FLeague::PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount)
{
	Board.DefaultBoard();

	for (MoveCount = 1; MoveCount <= 60; ++MoveCount)
	{
		White.PlayMove(Board);
		Board.FlipBoard();

		if (Board.GetGameState() > EGameState::ActiveBlack)
		{
			return Board.GetGameState();
		}

		Black.PlayMove(Board);
		Board.FlipBoard();

		if (Board.GetGameState() > EGameState::ActiveBlack)
		{
			return Board.GetGameState();
		}
	}

	--MoveCount;
	return Board.GetGameState();
}

float FLeague::GameScore(FDoubleBoard& Board)
{
	EGameState FinishingState = Board.GetGameState();
	if (FinishingState == EGameState::OverWhite)
	{
		return 1.0f;
	}
	if (FinishingState == EGameState::OverBlack)
	{
		return -1.0f;
	}
	if (FinishingState == EGameState::OverDraw)
	{
		return 0.0f;
	}

	bool bFlip = false;
	if (FinishingState == EGameState::ActiveBlack)
	{
		bFlip = true;
		Board.FlipBoard();
	}
	int Score = 0;
	const int Values[] = { -1000, -9, -5, -3, -3, -1, 0, 1, 3, 3, 5, 9, 1000 };
#if _DEBUG
	const float _DebugDummyArray[(int)EChessPiece::WhiteKing + 1 - (int)EChessPiece::BlackKing] = {};
	ARRAY_MATCH(Values, _DebugDummyArray);
#endif
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			Score += Values[(int)Board(Rank, File) - (int)EChessPiece::BlackKing];
		}
	}
	if (bFlip)
	{
		Board.FlipBoard();
	}
	return 0.1f * SigmoidFunction((float)Score);
}

void FLeague::RateGame(FDoubleBoard& Board, int White, int Black)
{
	const bool bIsPoolFull = GameResults.Count() == PoolSize;

	if (NextGameResult >= GameResults.Count())
	{
		if (bIsPoolFull)
		{
			NextGameResult = 0;
		}
		else
		{
			GameResults.Push();
		}
	}

	if (bIsPoolFull)
	{
		GlobalScore -= GameResults[NextGameResult];
	}

	const float BoardScore = GameScore(Board);
	GlobalScore += BoardScore;
	GameResults[NextGameResult++] = BoardScore;

	const float WeightedGlobalScore = bIsPoolFull ? GlobalScore / (float)PoolSize : 0.0f;

	const int RatingDiff = Ratings[White] - Ratings[Black];
	const float WeightedDiff = SigmoidFunction((float)RatingDiff);
	const float ScoreOffset = -0.5f * (WeightedDiff + WeightedGlobalScore);
	const float FinalScore = BoardScore + ScoreOffset;

	const int RatingChange = (int)(20.0f * FinalScore);
	Ratings[White] += RatingChange;
	Ratings[Black] -= RatingChange;
}
