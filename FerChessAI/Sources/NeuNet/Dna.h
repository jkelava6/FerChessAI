#pragma once

#include <Array.h>

class FDna
{
public:
	void SetData(const int* InData, int Length);
	int* GetData(int& Length);
	void New(int Size);

	void Peek(int Index);

	int ReadInt();
	float ReadFloat();
	void WriteInt(int Value);
	void WriteFloat(float Value);
private:
	int NextIndex = 0;
	Array<int> Data;
};



