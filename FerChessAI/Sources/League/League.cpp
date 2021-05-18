#include <StdH.h>
#include <League/League.h>

#include <ChessBoard.h>
#include <NeuNet/Node.h>

void FLeague::Initialize(int PopCount, int PopSize, int MaxMiddleNodes, int MaxRecurrentNodes)
{
	Populations.Clear();
	Populations.Prealocate(PopCount);
	for (int Index = 0; Index < PopCount; ++Index)
	{
		FPopulation& Pop = Populations.Push();
		Pop.Initialize(PopSize, MaxMiddleNodes, MaxRecurrentNodes);
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
		Populations[Index].NextGeneration();
	}
}

void FLeague::PlayAI(IChessAI& Challenger, FPopulation* Population, int UnitId)
{
	FDoubleBoard Board;
	for (int Index = 0; Index < Populations.Count(); ++Index)
	{
		IChessAI& Representative = Populations[Index].Representative();
		int WhiteMoves;
		EGameState WhiteResult = PlayGame(Board, Challenger, Representative, WhiteMoves);
		int BlackMoves;
		EGameState BlackResult = PlayGame(Board, Representative, Challenger, BlackMoves);

		Population->GradeMatch(UnitId, WhiteResult, WhiteMoves, BlackResult, BlackMoves);
	}
}

EGameState FLeague::PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount)
{
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
