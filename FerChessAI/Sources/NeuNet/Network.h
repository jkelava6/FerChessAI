#pragma once

#include <Array.h>

class FDna;
class FNode;

class FNetwork
{
public:
	void FromDna(FDna& Dna);
	//void ToDna(FDna& Dna);
	void SetInput(int Index, float Value);
	float GetOutput(int Index);

	void Update();
	void ResetRecurrent(int Level);
private:
	int Inputs = 0;
	int Outputs = 0;
	TArray<int> RecurrentPerLevel;
	TArray<FNode> Nodes;

	int FirstOutput = 0;
	int TotalRecurrent = 0;
};





