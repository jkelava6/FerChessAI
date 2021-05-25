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
		EGameState WhiteResult = PlayGame(Board, Challenger, Representative, WhiteMoves);
		int BlackMoves;
		EGameState BlackResult = PlayGame(Board, Representative, Challenger, BlackMoves);

		if (bRated)
		{
			RateGame(WhiteResult, ChallengingPop, Index);
			RateGame(BlackResult, Index, ChallengingPop);
		}

		Population->GradeMatch(UnitId, WhiteResult, WhiteMoves, BlackResult, BlackMoves);
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

		if (Board.GetGameState() > EGameState::ActiveBlack)
		{
			return Board.GetGameState();
		}

		Black.PlayMove(Board);

		if (Board.GetGameState() > EGameState::ActiveBlack)
		{
			return Board.GetGameState();
		}
	}

	--MoveCount;
	return Board.GetGameState();
}

int FLeague::GameScore(EGameState FinishingState)
{
	if (FinishingState == EGameState::OverWhite)
	{
		return 1;
	}
	if (FinishingState == EGameState::OverBlack)
	{
		return -1;
	}
	return 0;
}

void FLeague::RateGame(EGameState FinishingState, int White, int Black)
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
		GlobalScore -= GameScore(GameResults[NextGameResult]);
	}

	GlobalScore += GameScore(FinishingState);
	GameResults[NextGameResult++] = FinishingState;

	if (bIsPoolFull)
	{
		const float WeightedGlobalScore = GlobalScore / (float)PoolSize;
		const int RatingDiff = Ratings[White] - Ratings[Black];
		const float WeightedDiff = SigmoidFunction((float)RatingDiff);
		const float ScoreOffset = -0.5f * (WeightedDiff + WeightedGlobalScore);
		const float FinalScore = (float)GameScore(FinishingState) + ScoreOffset;

		const int RatingChange = (int)(20.0f * FinalScore);
		Ratings[White] += RatingChange;
		Ratings[Black] -= RatingChange;
	}
}
