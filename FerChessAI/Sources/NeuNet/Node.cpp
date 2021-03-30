
#include <StdH.h>
#include <NeuNet/Node.h>



FNodeInput::FNodeInput(FNode* InHarvestNode/* = nullptr*/, float InLinkStrength/* = 0.0f*/) :
	HarvestNode(InHarvestNode),
	LinkStrength(InLinkStrength)
{
}

FNode::FNode(int InputCount/* = 0*/, float InBias/* = 0.0f*/) :
	Bias(InBias),
	State(InBias),
	Inputs(InputCount)
{
}

void FNode::SetState(float InState)
{
	State = InState;
}

float FNode::GetState() const
{
	return State;
}

void FNode::AddLink(FNode* Node, float Strength)
{
	Inputs.Push() = FNodeInput(Node, Strength);
}

void FNode::Update()
{
	float Sum = Bias;
	const int NumOfInputs = Inputs.Count();
	for (int Index = 0; Index < NumOfInputs; ++Index)
	{
		FNodeInput& Link = Inputs[Index];
		Sum += Link.LinkStrength * Link.HarvestNode->GetState();
	}

	SetState(SigmoidFunction(Sum));
}

extern float SigmoidFunction(float Input)
{
	float ExpPlus = PowerNat(Input);
	float ExpMinus = 1.0f / ExpPlus;
	return (ExpPlus - ExpMinus) / (ExpPlus + ExpMinus);
}

