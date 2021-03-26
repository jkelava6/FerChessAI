#pragma once

#include <Array.h>

class FDna
{
public:
	void SetData(const int* InData, int Length);
	int* GetData(int& Length);
	void New(int Size);

	void Peek(int Index);
	int Size();

	int ReadInt();
	float ReadFloat();
	void WriteInt(int Value);
	void WriteFloat(float Value);
	int& AccesInt(int Index);
	float& AccesFloat(int Index);

	void PushInt(int Value);
	void PushFloat(float Value);
	void Pack();

	FDna* Clone();
	void CloneTo(FDna& Target);
private:
	int NextIndex = 0;
	TArray<int> Data;
};



