
#include <StdH.h>
#include <NeuNet/Network.h>

#include <NeuNet/Dna.h>
#include <NeuNet/Node.h>


void FDna::SetData(const int* InData, int Length)
{
	Data.Prealocate(Length);
	for (int Index = 0; Index < Length; ++Index)
	{
		Data[Index] = InData[Index];
	}
}

int* FDna::GetData(int& Length)
{
	Length = Data.Count();
	int* OutData = new int[Length];
	for (int Index = 0; Index < Length; ++Index)
	{
		OutData[Index] = Data[Index];
	}
	NextIndex = 0;
}

void FDna::New(int Size)
{
	Data.Prealocate(Size);
	NextIndex = 0;
}

void FDna::Peek(int Index)
{
	NextIndex = Index;
}

int FDna::Size()
{
	return Data.Count();
}

int FDna::ReadInt()
{
	return Data[NextIndex++];
}

float FDna::ReadFloat()
{
	return *(float*)&Data[NextIndex++];
}

void FDna::WriteInt(int Value)
{
	Data[NextIndex++] = Value;
}

void FDna::WriteFloat(float Value)
{
	(*(float*)&Data[NextIndex++]) = Value;
}

int& FDna::AccesInt(int Index)
{
	return Data[Index];
}

float& FDna::AccesFloat(int Index)
{
	return *(float*)&Data[Index];
}


void FDna::PushInt(int Value)
{
	Data.Push() = Value;
}

void FDna::PushFloat(float Value)
{
	(*(float*)&Data.Push()) = Value;
}

void FDna::Pack()
{
	Data.Prealocate(Data.Count());
}

FDna* FDna::Clone()
{
	FDna* Cloned = new FDna();
	Cloned->Peek(0);
	Cloned->Data = Data;
	return Cloned;
}

void FDna::CloneTo(FDna& Target)
{
	Target.Peek(0);
	Target.Data = Data;
}





