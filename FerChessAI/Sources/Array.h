#pragma once


template <class Type> class Array
{
private:
	Type* Data = nullptr;
	int AllocatedSize = 0;
	int UsedSize = 0;


public:
	Array(int InitialSize = 16)
	{
		Prealocate(InitialSize);
	}

	void Prealocate(int Size)
	{
		Type* NewData = new Type[Size];

		for (int Index = 0; Index < UsedSize; ++Index)
		{
			NewData[Index] = Data[Index];
		}
		delete[] Data;

		Data = NewData;
		AllocatedSize = Size;
	}

	Type& Push()
	{
		if (UsedSize == AllocatedSize)
		{
			Prealocate(AllocatedSize * 2);
		}
		return Data[UsedSize++];
	}

	void Pop()
	{
		--UsedSize;
	}

	int Count()
	{
		return UsedSize;
	}

	Type& operator[] (int Index)
	{
		return Data[Index];
	}

	Type& Top()
	{
		return Data[UsedSize - 1];
	}

};

