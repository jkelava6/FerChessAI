
#include <StdH.h>
#include <NeuNet/Network.h>

#include <NeuNet/Dna.h>
#include <NeuNet/Node.h>


void FNetwork::FromDna(FDna& Dna)
{
	Dna.Peek(0);

	Inputs = Dna.ReadInt();
	Outputs = Dna.ReadInt();
	RecurrentPerLevel.PopAll();
	RecurrentPerLevel.Prealocate(Dna.ReadInt());
	TotalRecurrent = 0;
	for (int Level = 0; Level < RecurrentPerLevel.Count(); ++Level)
	{
		RecurrentPerLevel.Push() = Dna.ReadInt();
		TotalRecurrent = RecurrentPerLevel[Level];
	}

	const int MiddleNodes = Dna.ReadInt();
	const int TotalNodes = Inputs + TotalRecurrent + MiddleNodes + Outputs + TotalRecurrent;
	const int FirstMiddleNode = Inputs + TotalRecurrent;
	FirstOutput = Inputs + TotalRecurrent + MiddleNodes;

	Nodes.PopAll();
	Nodes.Prealocate(TotalNodes);
	for (int Node = Inputs; Inputs < TotalNodes; ++Node)
	{
		const float Bias = Dna.ReadFloat();
		const int Links = Node < TotalRecurrent ? 0 : Dna.ReadInt();
		Nodes.Push() = FNode(Links, Bias);

		for (int Link = 0; Link < Links; ++Link)
		{
			Nodes[Node].AddLink(&Nodes[Dna.ReadInt()], Dna.ReadFloat());
		}
	}
}

/*void FNetwork::ToDna(FDna& Dna)
{

}*/

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


