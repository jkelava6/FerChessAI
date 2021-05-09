
#include <StdH.h>
#include <NeuNet/Network.h>

#include <Math.h>
#include <NeuNet/Dna.h>
#include <NeuNet/Node.h>

extern float NetworkDerivativeEpsilon = 1e-6f;

FNetwork::FNetwork() = default;

FNetwork::~FNetwork() = default;

void FNetwork::FromDna(FDna& Dna)
{
	Dna.Peek(0);

	Inputs = Dna.ReadInt();
	Outputs = Dna.ReadInt();
	RecurrentPerLevel.PopAll();
	const int RecurrentLevels = Dna.ReadInt();
	RecurrentPerLevel.Prealocate(RecurrentLevels);
	TotalRecurrent = 0;
	for (int Level = 0; Level < RecurrentLevels; ++Level)
	{
		RecurrentPerLevel.Push() = Dna.ReadInt();
		TotalRecurrent += RecurrentPerLevel[Level];
	}

	const int MiddleNodes = Dna.ReadInt();
	const int TotalNodes = Inputs + TotalRecurrent + MiddleNodes + Outputs + TotalRecurrent;
	const int FirstMiddleNode = Inputs + TotalRecurrent;
	FirstOutput = Inputs + TotalRecurrent + MiddleNodes;

	Nodes.PopAll();
	Nodes.Prealocate(TotalNodes);
	for (int Node = 0; Node < Inputs; ++Node)
	{
		Nodes.Push();
	}
	for (int Node = Inputs; Node < TotalNodes; ++Node)
	{
		const float Bias = Dna.ReadFloat();
		const int Links = Node < FirstMiddleNode ? 0 : Dna.ReadInt();
		Nodes.Push() = FNode(Links, Bias);

		for (int Link = 0; Link < Links; ++Link)
		{
			const int SourceNode = Dna.ReadInt();
			const float SourceStrength = Dna.ReadFloat();
			Nodes[Node].AddLink(&Nodes[SourceNode], SourceStrength);
		}
	}
}

void FNetwork::ToDna(FDna& Dna)
{
	Dna.New(800);
	Dna.PushInt(Inputs);
	Dna.PushInt(Outputs);
	Dna.PushInt(RecurrentPerLevel.Count());
	for (int Level = 0; Level < RecurrentPerLevel.Count(); ++Level)
	{
		Dna.PushInt(RecurrentPerLevel[Level]);
	}

	const int TotalNodes = Nodes.Count();
	const int MiddleNodes = TotalNodes - (Inputs + TotalRecurrent + Outputs + TotalRecurrent);
	Dna.PushInt(MiddleNodes);
	const int FirstMiddleNode = Inputs + TotalRecurrent;
	FirstOutput = Inputs + TotalRecurrent + MiddleNodes;

	const FNode* FirstNode = &Nodes[0];
	for (int Index = Inputs; Index < TotalNodes; ++Index)
	{
		const FNode& Node = Nodes[Index];
		Dna.PushFloat(Node.Bias);
		if (Index < FirstMiddleNode)
		{
			continue;
		}
		const int LinkCount = Node.Inputs.Count();
		Dna.PushInt(LinkCount);

		for (int Link = 0; Link < LinkCount; ++Link)
		{
			const FNodeInput& Source = Node.Inputs[Link];
			Dna.PushInt((int)(Source.HarvestNode - FirstNode));
			Dna.PushFloat(Source.LinkStrength);
		}
	}
}

void FNetwork::SetInput(int Index, float Value)
{
	Nodes[Index].SetState(Value);
}

float FNetwork::GetOutput(int Index)
{
	return Nodes[FirstOutput + Index].GetState();
}

void FNetwork::Update()
{
	const int TotalNodes = Nodes.Count();
	for (int Index = Inputs + TotalRecurrent; Index < TotalNodes; ++Index)
	{
		Nodes[Index].Update();
	}

	if (bTrackReinforcement)
	{
		TArray<float>& SaveStates = ReinforcementStates.Push();
		for (int NodeIndex = 0; NodeIndex < TotalNodes; ++NodeIndex)
		{
			SaveStates.Push() = Nodes[NodeIndex].GetState();
		}
	}

	const int FirstRecurrent = FirstOutput + Outputs;
	for (int Index = 0; Index < TotalRecurrent; ++Index)
	{
		Nodes[Inputs + Index].SetState(Nodes[FirstRecurrent + Index].GetState());
	}
}

void FNetwork::ResetRecurrent(int Level)
{
	const int LastRecurrent = Inputs + TotalRecurrent;
	for (int Index = Inputs; Index < LastRecurrent; ++Index)
	{
		Nodes[Index].Update();
	}
}

void FNetwork::InitiateReinforcement()
{
	bTrackReinforcement = true;
	ReinforcementStates.Clear();

	BiasReinforcements.Clear();
	const int NodeCount = Nodes.Count();
	BiasReinforcements.Prealocate(NodeCount);
	for (int NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		BiasReinforcements.Push() = 0.0f;
	}

	LinkReinforcements.Clear();
	LinkReinforcements.Prealocate(NodeCount);
	for (int NodeIndex = 0; NodeIndex < NodeCount; ++NodeIndex)
	{
		LinkReinforcements.Push();
		const int LinkCount = Nodes[NodeIndex].Inputs.Count();
		LinkReinforcements[NodeIndex].Prealocate(LinkCount);
		for (int LinkIndex = 0; LinkIndex < LinkCount; ++LinkIndex)
		{
			LinkReinforcements[NodeIndex].Push() = 0.0f;
		}
	}
}

void FNetwork::SeedReinforcement(int Output, float Target, float FeedbackAmount, EReinforcementType Type, float TypeValue/* = 0.0f*/)
{
	const int TargetIndex = FirstOutput + Output;
	FNode& TargetNode = Nodes[TargetIndex];

	const int NodeCount = Nodes.Count();
	TArray<float> Feedback(NodeCount);
	for (int Node = 0; Node < NodeCount; ++Node)
	{
		Feedback.Push() = 0.0f;
	}

	for (int Iteration = ReinforcementStates.Count() - 1; Iteration >= 0; --Iteration)
	{
		TArray<float>& StoredStates = ReinforcementStates[Iteration];

		if (Iteration == 0)
		{
			Feedback[TargetIndex] = TargetNode.GetState() < Target ? FeedbackAmount : -FeedbackAmount;
		}
		else
		{
			const int FirstOutRecurrent = FirstOutput + Outputs;
			for (int Node = 0; Node < TotalRecurrent; ++Node)
			{
				Feedback[FirstOutRecurrent + Node] = Feedback[Inputs + Node];
			}
			for (int Node = Inputs; Node < FirstOutRecurrent; ++Node)
			{
				Feedback[Node] = 0.0f;
			}
		}

		FNode* FirstNode = &Nodes[0];
		for (int Index = NodeCount - 1; Index >= Inputs; --Index)
		{
			if (AbsF(Feedback[Index]) < 1e-3f)
			{
				continue;
			}

			FNode& Node = Nodes[Index];
			const float BackProp = SigmaDerivative(Node) * Feedback[Index];
			for (int Link = 0; Link < Node.Inputs.Count(); ++Link)
			{
				const FNodeInput& Input = Node.Inputs[Link];
				Feedback[(int)(Input.HarvestNode - FirstNode)] += Input.LinkStrength * BackProp;
			}

			if (Iteration > 0 && Index > Inputs + TotalRecurrent)
			{
				continue;
			}

			if (Type == EReinforcementType::Full || (Type == EReinforcementType::RandomChance && RandomF() < TypeValue))
			{
				BiasReinforcements[Index] += Feedback[Index];
				for (int LinkIndex = 0; LinkIndex < Node.Inputs.Count(); ++LinkIndex)
				{
					LinkReinforcements[Index][LinkIndex] += SignF(Node.Inputs[LinkIndex].HarvestNode->GetState()) * Feedback[Index];
				}
			}
		}
	}
}

void FNetwork::ClearReinforcementStates()
{
	ReinforcementStates.PopAll();
}

void FNetwork::ExecuteReinforcement(float MinBiasStep, float MaxBiasStep, float MaxBiasValue,
	float MinLinkStep, float MaxLinkStep, float MaxLinkValue)
{
	const int TotalNodes = Nodes.Count();
	for (int NodeIndex = 0; NodeIndex < TotalNodes; ++NodeIndex)
	{
		FNode& Node = Nodes[NodeIndex];
		const int LinkCount = Node.Inputs.Count();
		for (int LinkIndex = 0; LinkIndex < LinkCount; ++LinkIndex)
		{
			const float Feedback = LinkReinforcements[NodeIndex][LinkIndex];
			if (AbsF(Feedback) < 1e-12f)
			{
				continue;
			}

			float& Strength = Node.Inputs[LinkIndex].LinkStrength;
			const float Change = SignF(Feedback) * (MinLinkStep + RandomF() * (MaxLinkStep - MinLinkStep));
			Strength = ClampF(Strength + Change, -MaxLinkValue, MaxLinkValue);
		}

		const float Feedback = BiasReinforcements[NodeIndex];
		if (AbsF(Feedback) < 1e-3f)
		{
			continue;
		}

		const float Change = SignF(Feedback) * (MinLinkStep + RandomF() * (MaxLinkStep - MinLinkStep));
		Node.Bias = ClampF(Node.Bias + Change, -MaxLinkValue, MaxLinkValue);
	}

	bTrackReinforcement = false;
}

// Based on Wolfram Alpha:
// https://www.wolframalpha.com/input/?i=derivate+%28%28e%5Ex+-+e%5E-x%29+%2F+%28e%5Ex+%2B+e%5E-x%29%29
// Query: derivate ((e^x - e^-x) / (e^x + e^-x))
// Gives: 4e^2x / (e^2x + 1)^2
float FNetwork::SigmaDerivative(const FNode& Node)
{
	const float X = Node.GetState();
	const float ETo2X = PowerNat(2 * X);
	return (4 * ETo2X) / SquareF(ETo2X + 1);
}


