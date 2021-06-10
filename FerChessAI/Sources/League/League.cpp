#include <StdH.h>
#include <League/League.h>

#include <ChessBoard.h>
#include <NeuNet/Node.h>
#include <Threading/ThreadingUtils.h>

FThreadData::FThreadData() = default;
IMPLEMENT_MOVE(FThreadData);

FLeague::FLeague() = default;
FLeague::~FLeague() = default;

void FLeague::Initialize(int PopCount, int InPopSize, int MaxMiddleNodes, int MaxRecurrentNodes, int MaxLinksPerNode, float LinkCutChance,
	float NodeAnomalyChance, int NodeEquilibriumCount, float NodeDisruptionChance,
	float RecurrentAnomalyChance, int EquilibriumRecurrentCount, float RecurrentDisruptionChance
#if USE_CONSUMER_FUNCTIONS
	, float ConsumerAnomalyChance
#endif
#if USE_MAPPING_FUNCTIONS
	, float MappingAnomalyChance
#endif
)
{
	Populations.Clear();
	Populations.Prealocate(PopCount);
	Ratings.Clear();
	Ratings.Prealocate(PopCount);
	GenPoints.Clear();
	GenPoints.Prealocate(PopCount);
#if USE_BEST_PRESERVATION
	BestSwitched.Clear();
	BestSwitched.Prealocate(PopCount);
#endif
	for (int Index = 0; Index < PopCount; ++Index)
	{
		FPopulation& Pop = Populations.Push();
		Pop.Initialize(InPopSize, MaxMiddleNodes, MaxRecurrentNodes, MaxLinksPerNode, LinkCutChance,
			NodeAnomalyChance, NodeEquilibriumCount, NodeDisruptionChance,
			RecurrentAnomalyChance, EquilibriumRecurrentCount, RecurrentDisruptionChance
#if USE_CONSUMER_FUNCTIONS
			, ConsumerAnomalyChance
#endif
#if USE_MAPPING_FUNCTIONS
			, MappingAnomalyChance
#endif
		);
		Ratings.Push() = 1000;
		Locks.Push() = false;
		GenPoints.Push() = -1e9f;
#if USE_BEST_PRESERVATION
		BestSwitched.Push();
#endif
	}
	PopSize = InPopSize;

	// if thread pool isn't initialize, initialize one with an arbitrary number of threads
	if (ChessThreads::ThreadPoolCount() == 0)
	{
		ChessThreads::InitializeThreadPool(4);
	}
}

void FLeague::Iterate()
{
	const int PopCount = Populations.Count();
	TArray<FThreadData> Games;
	for (int LeftPop = 0; LeftPop < PopCount; ++LeftPop)
	{
		GenPoints[LeftPop] = 0.0f;
		const int MaxUnit = Locks[LeftPop] ? 1 : PopSize;
		for (int LeftUnit = 0; LeftUnit < MaxUnit; ++LeftUnit)
		{
			for (int RightPop = (LeftUnit == 0) ? LeftPop + 1 : 0; RightPop < PopCount; ++RightPop)
			{
				if (RightPop == LeftPop)
				{
					continue;
				}

				SetupGame(Games, LeftPop, LeftUnit, RightPop, 0);
				SetupGame(Games, RightPop, 0, LeftPop, LeftUnit);
			}
		}
	}

	for (int Index = 0; Index < Games.Count(); ++Index)
	{
		ChessThreads::QueueTaskBlocking(0, ExecPlay, (void*)&Games[Index]);
	}
	ChessThreads::WaitForAllTasks(0);

	NewRatings = Ratings;
	for (int Index = 0; Index < Games.Count(); ++Index)
	{
		FThreadData& Game = Games[Index];
		if (Game.UnitIndexWhite == 0 && Game.UnitIndexBlack == 0)
		{
			RateGame(Game.Board, Game.PopIndexWhite, Game.PopIndexBlack);
			EGameState FinalState = Game.Board.GetGameState();
			switch (FinalState)
			{
			case EGameState::ActiveWhite:
				GenPoints[Game.PopIndexWhite] += 0.5f;
				GenPoints[Game.PopIndexBlack] += 0.5f;
				break;
			case EGameState::ActiveBlack:
				GenPoints[Game.PopIndexWhite] += 0.5f;
				GenPoints[Game.PopIndexBlack] += 0.5f;
				break;
			case EGameState::OverWhite:
				GenPoints[Game.PopIndexWhite] += 1.0f;
				GenPoints[Game.PopIndexBlack] += 0.0f;
				break;
			case EGameState::OverDraw:
				GenPoints[Game.PopIndexWhite] += 0.5f;
				GenPoints[Game.PopIndexBlack] += 0.5f;
				break;
			case EGameState::OverBlack:
				GenPoints[Game.PopIndexWhite] += 0.0f;
				GenPoints[Game.PopIndexBlack] += 1.0f;
				break;
			default:
				break;
			}
		}
		const float BoardScore = GameScore(Game.Board);
		if (Game.UnitIndexWhite == 0)
		{
			Populations[Game.PopIndexBlack].GradeMatch(Game.UnitIndexBlack, -BoardScore, Game.MoveCount);
		}
		if (Game.UnitIndexBlack == 0)
		{
			Populations[Game.PopIndexWhite].GradeMatch(Game.UnitIndexWhite, BoardScore, Game.MoveCount);
		}
	}
	Ratings = Move(NewRatings);

	for (int Index = 0; Index < PopCount; ++Index)
	{
		if (!Locks[Index])
		{
			Populations[Index].NextGeneration(*this);
		}
		else
		{
#if USE_BEST_PRESERVATION
			BestSwitched[Index] = false;
#endif
		}
	}
}

const FDna& FLeague::GetDna(int PopulationIndex, int UnitIndex)
{
	return Populations[PopulationIndex].GetDna(UnitIndex);
}

EGameState FLeague::PlayGame(FDoubleBoard& Board, IChessAI& White, IChessAI& Black, int& MoveCount, int MaxMoves)
{
	for (MoveCount = 1; MoveCount <= MaxMoves; ++MoveCount)
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

void FLeague::SetLocked(int PopIndex, bool bLocked)
{
	Locks[PopIndex] = bLocked;
}

int FLeague::IndexOf(FPopulation* Pop)
{
	return Populations.IndexOf(Pop);
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
	const float WeightedDiff = SigmoidFunction(RatingDiff / 300.0f);
	const float ScoreOffset = -0.5f * (WeightedDiff + WeightedGlobalScore);
	const float FinalScore = BoardScore + ScoreOffset;

	const int RatingChange = (int)ClampF(20.0f * FinalScore, -100.0f, 100.0f);
	NewRatings[White] += RatingChange;
	NewRatings[Black] -= RatingChange;
}

void FLeague::SetupGame(TArray<FThreadData>& Games, int WhitePop, int WhiteUnit, int BlackPop, int BlackUnit)
{
	FThreadData& Game = Games.Push();

	Game.League = this;

	Game.Board.DefaultBoard();
	Game.Board.AllocateStack(128);
	Game.PopIndexWhite = WhitePop;
	Game.UnitIndexWhite = WhiteUnit;
	Game.PopIndexBlack = BlackPop;
	Game.UnitIndexBlack = BlackUnit;

	FNetEvalMinMax& White = Game.WhiteAI;
	White.SetDepths(NormalDepth, VolatileDepth);
	FDna DnaWhite = GetDna(WhitePop, WhiteUnit);
	White.AccesNetwork().FromDna(DnaWhite);

	FNetEvalMinMax& Black = Game.BlackAI;
	Black.SetDepths(NormalDepth, VolatileDepth);
	FDna DnaBlack = GetDna(BlackPop, BlackUnit);
	Black.AccesNetwork().FromDna(DnaBlack);
}

extern void ExecPlay(void* Arg)
{
	FThreadData& Data = *(FThreadData*)Arg;
	FLeague* League = Data.League;

	Data.MoveCount = 0;
	League->PlayGame(Data.Board, Data.WhiteAI, Data.BlackAI, Data.MoveCount, Data.MaxMoves);
}
