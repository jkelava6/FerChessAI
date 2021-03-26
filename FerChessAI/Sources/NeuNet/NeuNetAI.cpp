
#include <StdH.h>
#include <NeuNet/NeuNetAI.h>

#include <ChessBoard.h>
#include <Math.h>
#include <NeuNet/Dna.h>
#include <NeuNet/Node.h>

FNeuNetFullAI::FNeuNetFullAI() = default;

bool FNeuNetFullAI::PlayMove(FDoubleBoard& Board)
{
	LastMoveVerdict = ELastMoveResult::None;

	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Network.SetInput(Row * 8 + File, (float)Board(Row, File));
		}
	}
	Network.ResetRecurrent(0);

	while (TicksRemaining > 0)
	{
		Network.SetInput(64, (float)TicksRemaining);
		Network.Update();

		if (Network.GetOutput(0) > 0.0f)
		{
			int Values[4];
			for (int Output = 0; Output < 4; ++Output)
			{
				const float OutputVal = Network.GetOutput(Output + 1);
				if (OutputVal < 0 || OutputVal >= SigmoidFunction(8))
				{
					LastMoveVerdict = ELastMoveResult::FailedInvalid;
					return false;
				}

				for (int Coord = 0; Coord < 8; ++Coord)
				{
					if (OutputVal < SigmoidFunction((float)(Coord + 1)))
					{
						Values[Output] = Coord;
						break;
					}
				}
			}

			const int RowFrom = Values[0];
			const int FileFrom = Values[1];
			const int RowTo = Values[2];
			const int FileTo = Values[3];

			TArray<int> Rows(16);
			TArray<int> Files(16);
			Board.CollectMoves(RowFrom, RowTo, Rows, Files);

			const int NumOfMoves = Rows.Count();
			for (int Move = 0; Move < NumOfMoves; ++Move)
			{
				if (RowTo == Rows[Move] && FileTo == Files[Move])
				{
					Board.Move(RowFrom, FileFrom, RowTo, FileTo);
					LastMoveVerdict = ELastMoveResult::Valid;
					return true;
				}
			}

			LastMoveVerdict = ELastMoveResult::FailedInvalid;
			return false;
		}
	}

	LastMoveVerdict = ELastMoveResult::FailedTimeOut;
	return false;
}

void FNeuNetFullAI::SetTimeControl(int InStartTicks, int InTicksPerMove, int InMaxTicks)
{
	StartTicks = InStartTicks;
	TicksPerMove = InTicksPerMove;
	MaxTicks = InMaxTicks;
}

void FNeuNetFullAI::StartGame()
{
	TicksRemaining = StartTicks;
	Network.ResetRecurrent(1);
}

FNeuNetFullMutator::FNeuNetFullMutator(
	int InMoveRecurrent, int InGameRecurrent, int InLifeRecurrent, int InMiddleNodes,
	int InLowInputs, int InHighInputs, float InMaxBias, float InMaxLinkStrength,
	float InBiasChangeChance, float InBiasChangeRatio, int InBiasChangeResilience,
	float InLinkChangeChance, float InLinkChangeRatio, int InLinkChangeResilience,
	float InLinkRedirectChance) :
	MoveRecurrent(InMoveRecurrent),
	GameRecurrent(InGameRecurrent),
	LifeRecurrent(InLifeRecurrent),
	MiddleNodes(InMiddleNodes),
	LowInputs(InLowInputs),
	HighInputs(InHighInputs),
	MaxBias(InMaxBias),
	MaxLinkStrength(InMaxLinkStrength),
	BiasChangeChance(InBiasChangeChance),
	BiasChangeRatio(InBiasChangeRatio),
	BiasChangeResilience(InBiasChangeResilience),
	LinkChangeChance(InLinkChangeChance),
	LinkChangeRatio(InLinkChangeRatio),
	LinkChangeResilience(InLinkChangeResilience),
	LinkRedirectionChance(InLinkRedirectChance)
{
}

FDna&& FNeuNetFullMutator::CreateDna()
{
	FDna Dna;
	Dna.New(200);

	Dna.PushInt(Inputs);
	Dna.PushInt(Outputs);

	Dna.PushInt(3);
	Dna.PushInt(MoveRecurrent);
	Dna.PushInt(GameRecurrent);
	Dna.PushInt(LifeRecurrent);

	Dna.PushInt(MiddleNodes);

	const int TotalRecurrent = MoveRecurrent + GameRecurrent + LifeRecurrent;
	const int FirstMiddleNode = Inputs + TotalRecurrent;
	const int TotalNodes = FirstMiddleNode + MiddleNodes + Outputs + TotalRecurrent;
	for (int Node = Inputs; Node < TotalNodes; ++Node)
	{
		Dna.PushFloat(-MaxBias + 2 * RandomF() * MaxBias);

		if (Node < FirstMiddleNode)
		{
			continue;
		}

		const int Links = LowInputs + (int)(RandomF() * (HighInputs + 1 - LowInputs));
		Dna.PushInt(Links);
		for (int Link = 0; Link < Links; ++Link)
		{
			Dna.PushInt((int)(RandomF() * Node));
			Dna.PushFloat(-MaxLinkStrength + 2 * RandomF() * MaxBias);
		}
	}

	Dna.Pack();
	return Move(Dna);
}

void FNeuNetFullMutator::MutateDna(FDna& Dna)
{
	int Index = 0;
	++Index; // inputs known
	++Index; // outputs known
	++Index; // number of recurrent levels known
	Index += 3; // numbers of nodes per recurrent levels known

	const int TotalRecurrent = MoveRecurrent + GameRecurrent + LifeRecurrent;
	const int FirstMiddleNode = Inputs + TotalRecurrent;
	const int TotalNodes = FirstMiddleNode + MiddleNodes + Outputs + TotalRecurrent;
	for (int Node = Inputs; Node < TotalNodes; ++Node)
	{
		MutateF(Dna.AccesFloat(Index++), BiasChangeChance, BiasChangeRatio, BiasChangeResilience, MaxBias);

		if (Node < FirstMiddleNode)
		{
			continue;
		}

		const int Links = Dna.AccesInt(Index++);

		for (int Link = 0; Link < Links; ++Link)
		{
			if (RandomF() > LinkRedirectionChance)
			{
				Dna.AccesInt(Index) = (int)(RandomF() * Node);
			}
			++Index;

			MutateF(Dna.AccesFloat(Index++), LinkChangeChance, LinkChangeRatio, LinkChangeResilience, MaxLinkStrength);
		}
	}
}

void FNeuNetFullMutator::MutateF(float& Val, float Chance, float Ratio, int Resilience, float MaxVal)
{
	if (RandomF() >= Chance)
	{
		return;
	}

	float Change = 0;
	for (int Step = 0; Step < Resilience; ++Step)
	{
		Change += RandomF();
	}
	Change = -1.0f + 2.0f * Change / Resilience;
	Change = Change * Ratio * MaxVal;
	Val = ClampF(Val + Change, -MaxVal, MaxVal);
}







