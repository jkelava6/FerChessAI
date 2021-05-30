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

void FPopulation::Initialize(int Size, int InMaxMiddleNodes, int InMaxRecurrentNodes, int InMaxLinksPerNode, float InLinkCutChance/* = 0.01f*/)
{
	MaxMiddleNodes = InMaxMiddleNodes;
	MaxRecurrentNodes = InMaxRecurrentNodes;
	MaxLinksPerNode = InMaxLinksPerNode;
	LinkCutChance = InLinkCutChance;

	Units.Clear();
	Units.Prealocate(Size);
	for (int Index = 0; Index < Size; ++Index)
	{
		FUnit& Unit = Units.Push();
		GenerateDna(Unit.Dna, (int)(RandomF() * MaxMiddleNodes), (int)(RandomF() * MaxRecurrentNodes), MaxLinksPerNode);
	}
}

void FPopulation::NextGeneration(FLeague& League)
{
	TArray<FUnit> NextGen;
	NextGen.Prealocate(Units.Count());

	float TotalFitness = 0.0f;

	for (int Iteration = 0; Iteration < Units.Count(); ++Iteration)
	{
		TotalFitness += Units[Iteration].Fitness;
	}

	for (int Iteration = 0; Iteration < Units.Count(); ++Iteration)
	{
		float FitnessToGo = RandomF() * TotalFitness;

		for (int Index = 0; Index < Units.Count(); ++Index)
		{
			FitnessToGo -= Units[Index].Fitness;
			if (FitnessToGo >= 0)
			{
				continue;
			}

			FUnit& Mutated = NextGen.Push();
			MutateDna(Units[Index].Dna, Mutated.Dna);
			break;
		}
	}
}

void FPopulation::GradeMatch(int UnitId, float Score, int Moves)
{
	FUnit& Unit = Units[UnitId];

	if (Score == 1.0f)
	{
		Unit.Fitness += 1.0f - 0.01f * Moves;
	}
	else if (Score == 0.0f)
	{
		Unit.Fitness += 0.01f * Moves;
	}
	else
	{
		Unit.Fitness += Score;
	}
}

const FDna& FPopulation::GetDna(int UnitIndex)
{
	return Units[UnitIndex].Dna;
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
		const bool bLinkCut = RandomF() < LinkCutChance;
		const bool bNewLink = bLinkCut && RandomF() < 0.5f && LinkCount < MaxLinksPerNode;
		const bool bDestroyLink = bLinkCut && !bNewLink && LinkCount > 0;
		OutDna.PushInt(LinkCount + bNewLink - bDestroyLink);

		const int DestroyedLink = bDestroyLink ? (int)(RandomF() * LinkCount) : -1;

		for (int Link = 0; Link < LinkCount; ++Link)
		{
			if (Link == DestroyedLink)
			{
				InDna.ReadInt();
				InDna.ReadFloat();
				continue;
			}
			OutDna.PushInt(InDna.ReadInt());
			const float PrevLink = InDna.ReadFloat();
			OutDna.PushFloat(ClampF(PrevLink - 0.01f + RandomF() * 0.02f, -MaxLinkStrength, MaxLinkStrength));
		}

		if (bNewLink)
		{
			OutDna.PushInt((int)(RandomF() * (Inputs + Index)));
			OutDna.PushFloat(-MaxLinkStrength + RandomF() * 2.0f * MaxLinkStrength);
		}
	}
}

extern void GenerateDna(FDna& Dna, int MiddleNodes, int RecurrentNodes, int MaxLinksPerNode/* = 5*/)
{
	const float MaxBias = 1.0f;
	const int MinLinks = 1;
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
		const int LinkCount = MinLinks + (int)(RandomF() * (MaxLinksPerNode - MinLinks + 1));
		Dna.PushInt(LinkCount);
		for (int Link = 0; Link < LinkCount; ++Link)
		{
			Dna.PushInt((int)(RandomF() * (Inputs + Index)));
			Dna.PushFloat(-MaxLinkStrength + RandomF() * 2.0f * MaxLinkStrength);
		}
	}
}
