#include <StdH.h>
#include <League/Population.h>

#include <ChessBoard.h>
#include <League/League.h>
#include <NeuNet/ConsumerFunctions.h>
#include <NeuNet/MappingFunctions.h>
#include <NeuNet/Node.h>

#include <assert.h>

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

void FPopulation::Initialize(int Size, int InMaxMiddleNodes, int InMaxRecurrentNodes, int InMaxLinksPerNode, float InLinkCutChance,
	float InNodeAnomalyChance, int InEquilibriumNodeCount, float InNodeDisruptionChance,
	float InRecurrentAnomalyChance, int InEquilibriumRecurrentCount, float InRecurrentDisruptionChance
#if USE_CONSUMER_FUNCTIONS
	, float InConsumerAnomalyChance
#endif
#if USE_MAPPING_FUNCTIONS
	, float InMappingAnomalyChance
#endif
)
{
	MaxMiddleNodes = InMaxMiddleNodes;
	MaxRecurrentNodes = InMaxRecurrentNodes;
	MaxLinksPerNode = InMaxLinksPerNode;
	LinkCutChance = InLinkCutChance;
	NodeAnomalyChance = InNodeAnomalyChance;
	EquilibriumNodeCount = InEquilibriumNodeCount;
	NodeDisruptionChance = InNodeDisruptionChance;
	RecurrentAnomalyChance = InRecurrentAnomalyChance;
	EquilibriumRecurrentCount = InEquilibriumRecurrentCount;
	RecurrentDisruptionChance = InRecurrentDisruptionChance;
#if USE_CONSUMER_FUNCTIONS
	ConsumerAnomalyChance = InConsumerAnomalyChance;
#endif
#if USE_MAPPING_FUNCTIONS
	MappingAnomalyChance = InMappingAnomalyChance;
#endif

	Units.Clear();
	Units.Prealocate(Size);
	for (int Index = 0; Index < Size; ++Index)
	{
		FUnit& Unit = Units.Push();
		GenerateDna(Unit.Dna, (int)(RandomF() * MaxMiddleNodes), (int)(RandomF() * MaxRecurrentNodes), MaxLinksPerNode);
	}
}

static float FitnessMapping(float MappedFitness, float WorstFitnesss, float BestFitness)
{
	assert(MappedFitness > 0.0f);

	if (BestFitness == WorstFitnesss)
	{
		return 1.0f;
	}

	const float MinFitness = 0.1f;
	const float Strictness = 2.0f;

	const float Ratio = (MappedFitness - WorstFitnesss) / (BestFitness - WorstFitnesss);
	return LerpF(MinFitness, 1.0f, pow(Ratio, Strictness));
}

void FPopulation::NextGeneration(FLeague& League)
{
	TArray<FUnit> NextGen;
	NextGen.Prealocate(Units.Count());

	float TotalFitness = 0.0f;
	float BestFitness = -1.0f;
	float WorstFitness = 1000.0f;
#if USE_BEST_PRESERVATION
	int BestIndex = -1;
#endif

	for (int Index = 0; Index < Units.Count(); ++Index)
	{
#if USE_BEST_PRESERVATION
		if (Units[Index].Fitness > BestFitness)
		{
			BestIndex = Index;
		}
#endif
		BestFitness = Max(BestFitness, Units[Index].Fitness);
		WorstFitness = Min(WorstFitness, Units[Index].Fitness);
	}

	for (int Index = 0; Index < Units.Count(); ++Index)
	{
		Units[Index].Fitness = FitnessMapping(Units[Index].Fitness, WorstFitness, BestFitness);
		TotalFitness += Units[Index].Fitness;
	}

#if USE_BEST_PRESERVATION
	NextGen.Push().Dna = Units[BestIndex].Dna;
#endif

	for (int Iteration = USE_BEST_PRESERVATION; Iteration < Units.Count(); ++Iteration)
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
	const int Outputs = InDna.ReadInt();
	OutDna.PushInt(Outputs);
	OutDna.PushInt(InDna.ReadInt());

	const int RecurrentNodes = InDna.ReadInt();
	OutDna.PushInt(RecurrentNodes);
	const bool bRecurrentAnomaly = RandomF() < RecurrentAnomalyChance;
	const float RecurrentCreationChance = RecurrentAnomalyChance < EquilibriumRecurrentCount
		? LerpF(1.0f, 0.5f, RecurrentNodes / (float)EquilibriumRecurrentCount)
		: LerpF(0.5f, 0.0f, (RecurrentNodes - EquilibriumRecurrentCount) / (float)(MaxRecurrentNodes - EquilibriumRecurrentCount));
	const bool bNewRecurrent = bRecurrentAnomaly && RandomF() < RecurrentCreationChance && RecurrentNodes < MaxRecurrentNodes;
	const bool bDestroyRecurrent = bRecurrentAnomaly && !bNewRecurrent && RecurrentNodes > 0;
	const int DestroyedRecurrent = bDestroyRecurrent ? (int)(RandomF() * RecurrentNodes) : -1;

	const int MiddleNodes = InDna.ReadInt();
	OutDna.PushInt(MiddleNodes);
	const bool bMiddleAnomaly = RandomF() < NodeAnomalyChance;
	const float MiddleCreationChance = NodeAnomalyChance < EquilibriumNodeCount
		? LerpF(1.0f, 0.5f, MiddleNodes / (float)EquilibriumNodeCount)
		: LerpF(0.5f, 0.0f, (MiddleNodes - EquilibriumNodeCount) / (float)(MaxMiddleNodes - EquilibriumNodeCount));
	const bool bNewMiddle = bMiddleAnomaly && RandomF() < MiddleCreationChance && MiddleNodes < MaxMiddleNodes;
	const bool bDestroyMiddle = bMiddleAnomaly && !bNewMiddle && MiddleNodes > 0;
	const int DestroyedMiddle = bDestroyMiddle ? (int)(RandomF() * MiddleNodes) : -1;

	for (int Index = 0; Index < RecurrentNodes; ++Index)
	{
		const float PrevBias = InDna.ReadFloat();
		if (Index != DestroyedRecurrent)
		{
			OutDna.PushFloat(ClampF(PrevBias - 0.01f + RandomF() * 0.02f, -MaxBias, MaxBias));
		}
	}
	for (int Index = 0; Index < MiddleNodes + Outputs + RecurrentNodes; ++Index)
	{
		const float PrevBias = InDna.ReadFloat();
		const bool bPushNode = Index != DestroyedMiddle && (bDestroyRecurrent && Index != MiddleNodes + Outputs + DestroyedRecurrent);
		if (bPushNode)
		{
			OutDna.PushFloat(ClampF(PrevBias - 0.01f + RandomF() * 0.02f, -MaxBias, MaxBias));
		}

		const int LinkCount = InDna.ReadInt();
		const bool bLinkCut = RandomF() < LinkCutChance;
		const bool bNewLink = bLinkCut && RandomF() < 0.5f && LinkCount < MaxLinksPerNode;
		const bool bDestroyLink = bLinkCut && !bNewLink && LinkCount > 0;
		if (bPushNode)
		{
			OutDna.PushInt(LinkCount + bNewLink - bDestroyLink);
		}

#if USE_CONSUMER_FUNCTIONS
		const int ConsumerIndex = InDna.ReadInt();
		if (bPushNode)
		{
			OutDna.PushInt(RandomF() < ConsumerAnomalyChance ? RandomConsumerIndex() : ConsumerIndex);
		}
#endif
#if USE_MAPPING_FUNCTIONS
		const int MapperIndex = InDna.ReadInt();
		if (bPushNode)
		{
			OutDna.PushInt(RandomF() < MappingAnomalyChance ? RandomMappingIndex() : MapperIndex);
		}
#endif

		const int DestroyedLink = bDestroyLink ? (int)(RandomF() * LinkCount) : -1;

		for (int Link = 0; Link < LinkCount; ++Link)
		{
			if (Link == DestroyedLink || !bPushNode)
			{
				InDna.ReadInt();
				InDna.ReadFloat();
				continue;
			}
			OutDna.PushInt(InDna.ReadInt());
			const float PrevLink = InDna.ReadFloat();
			OutDna.PushFloat(ClampF(PrevLink - 0.01f + RandomF() * 0.02f, -MaxLinkStrength, MaxLinkStrength));
		}

		if (bNewLink && bPushNode)
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
#if USE_CONSUMER_FUNCTIONS
		Dna.PushInt(RandomConsumerIndex());
#endif
#if USE_MAPPING_FUNCTIONS
		Dna.PushInt(RandomMappingIndex());
#endif
		for (int Link = 0; Link < LinkCount; ++Link)
		{
			Dna.PushInt((int)(RandomF() * (Inputs + Index)));
			Dna.PushFloat(-MaxLinkStrength + RandomF() * 2.0f * MaxLinkStrength);
		}
	}
}
