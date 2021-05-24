#include <StdH.h>
#include <League/Population.h>

#include <ChessBoard.h>
#include <League/League.h>
#include <NeuNet/Node.h>

FUnit::FUnit() = default;
FUnit::~FUnit() = default;

FUnit& FUnit::operator= (const FUnit& Copied)
{
	Dna = Copied.Dna;
	Fitness = Copied.Fitness;
	return *this;
}

FUnit::FUnit(const FUnit& Copied)
{
	*this = Copied;
}

FUnit& FUnit::operator= (FUnit&& Moved) noexcept
{
	Dna = Move(Moved.Dna);
	Fitness = Moved.Fitness;
	Moved.Fitness = 0.0f;
	return *this;
}

FUnit::FUnit(FUnit&& Moved) noexcept
{
	*this = Move(Moved);
}

void FPopulation::Initialize(int Size, int InMaxMiddleNodes, int InMaxRecurrentNodes)
{
	MaxMiddleNodes = InMaxMiddleNodes;
	MaxRecurrentNodes = InMaxRecurrentNodes;
	RepAI.SetDepths(NormalDepth, VolatileDepth);

	Units.Clear();
	Units.Prealocate(Size);
	for (int Index = 0; Index < Size; ++Index)
	{
		FUnit& Unit = Units.Push();
		GenerateDna(Unit.Dna, (int)(RandomF() * MaxMiddleNodes), (int)(RandomF() * MaxRecurrentNodes));
	}

	RepAI.AccesNetwork().FromDna(Units[0].Dna);
}

void FPopulation::PlayInLeague(FLeague& League)
{
	FNetEvalMinMax AI;
	AI.SetDepths(NormalDepth, VolatileDepth);
	for (int Index = 0; Index < Units.Count(); ++Index)
	{
		AI.AccesNetwork().FromDna(Units[Index].Dna);
		League.PlayAI(AI, this, Index, Index == 0);
	}
}

void FPopulation::NextGeneration(FLeague& League)
{
	TArray<FUnit> NextGen;
	NextGen.Prealocate(Units.Count());

	float TotalFitness = 0.0f;

	for (int Iteration = 0; Iteration < Units.Count(); ++Iteration)
	{
		float FitnessToGo = RandomF() * TotalFitness;

		for (int Index = 0; Index < Units.Count(); ++Index)
		{
			FitnessToGo -= Units[Index].Fitness;
			if (FitnessToGo >= 0)
			{
				return;
			}

			FUnit& Mutated = NextGen.Push();
			MutateDna(Units[Index].Dna, Mutated.Dna);
			break;
		}
	}
}

void FPopulation::GradeMatch(int UnitId, EGameState WhiteResult, int WhiteMoves, EGameState BlackResult, int BlackMoves)
{
	FUnit& Unit = Units[UnitId];

	if (WhiteResult == EGameState::OverWhite)
	{
		Unit.Fitness += 1.0f - 0.01f * WhiteMoves;
	}
	else if (WhiteResult == EGameState::OverBlack)
	{
		Unit.Fitness += 0.01f * WhiteMoves;
	}

	if (BlackResult == EGameState::OverBlack)
	{
		Unit.Fitness += 1.0f - 0.01f * BlackMoves;
	}
	else if (BlackResult == EGameState::OverWhite)
	{
		Unit.Fitness += 0.01f * BlackMoves;
	}
}

IChessAI& FPopulation::Representative()
{
	return RepAI;
}

void FPopulation::SetDepths(int InNormalDepth, int InVolatileDepth)
{
	NormalDepth = InNormalDepth;
	VolatileDepth = InVolatileDepth;
	RepAI.SetDepths(NormalDepth, VolatileDepth);
}

void FPopulation::MutateDna(FDna& InDna, FDna& OutDna)
{
	const float MaxBias = 1.0f;
	const int MinLinks = 1;
	const int MaxLinks = 5;
	const float MaxLinkStrength = 2.0f;

	const int Inputs = RANKS * FILES;

	InDna.Peek(0);
	OutDna.PushInt(InDna.ReadInt());
	OutDna.PushInt(InDna.ReadInt());
	OutDna.PushInt(InDna.ReadInt());
	const int RecurrentNodes = InDna.ReadInt();
	OutDna.PushInt(RecurrentNodes);
	const int MiddleNodes = InDna.ReadInt();
	OutDna.PushInt(MiddleNodes);
	for (int Index = 0; Index < RecurrentNodes; ++Index)
	{
		const float PrevBias = InDna.ReadFloat();
		OutDna.PushFloat(ClampF(PrevBias - 0.01f + RandomF() * 0.02f, -MaxBias, MaxBias));
	}
	for (int Index = 0; Index < MiddleNodes + 1 + RecurrentNodes; ++Index)
	{
		const float PrevBias = InDna.ReadFloat();
		OutDna.PushFloat(ClampF(PrevBias - 0.01f + RandomF() * 0.02f, -MaxBias, MaxBias));

		const int LinkCount = InDna.ReadInt();
		OutDna.PushInt(LinkCount);
		for (int Link = 0; Link < LinkCount; ++Link)
		{
			OutDna.PushInt(InDna.ReadInt());
			const float PrevLink = InDna.ReadFloat();
			OutDna.PushFloat(ClampF(PrevLink - 0.01f + RandomF() * 0.02f, -MaxLinkStrength, MaxLinkStrength));
		}
	}
}

extern void GenerateDna(FDna& Dna, int MiddleNodes, int RecurrentNodes)
{
	const float MaxBias = 1.0f;
	const int MinLinks = 1;
	const int MaxLinks = 5;
	const float MaxLinkStrength = 2.0f;

	const int Inputs = RANKS * FILES;

	Dna.PushInt(Inputs);
	Dna.PushInt(1);
	Dna.PushInt(1);
	Dna.PushInt(RecurrentNodes);
	Dna.PushInt(MiddleNodes);
	for (int Index = 0; Index < RecurrentNodes; ++Index)
	{
		Dna.PushFloat(-MaxBias + 2.0f * MaxBias * RandomF());
	}
	for (int Index = 0; Index < MiddleNodes + 1 + RecurrentNodes; ++Index)
	{
		Dna.PushFloat(-MaxBias + 2.0f * MaxBias * RandomF());
		const int LinkCount = MinLinks + (int)(RandomF() * (MaxLinks - MinLinks + 1));
		Dna.PushInt(LinkCount);
		for (int Link = 0; Link < LinkCount; ++Link)
		{
			Dna.PushInt((int)(RandomF() * (Inputs + Index)));
			Dna.PushFloat(-MaxLinkStrength + RandomF() * 2.0f * MaxLinkStrength);
		}
	}
}
