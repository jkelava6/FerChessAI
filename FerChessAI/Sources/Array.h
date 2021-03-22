#pragma once


#include <Math.h>


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

	~Array()
	{
		if (Data)
		{
			delete[] Data;
		}
	}

	Array<Type>& operator= (const Array<Type>& Copied)
	{
		if (Data)
		{
			delete[] Data;
		}

		AllocatedSize = Copied.AllocatedSize;
		UsedSize = Copied.UsedSize;
		Data = new Type[AllocatedSize];
		for (int Index = 0; Index < UsedSize; ++Index)
		{
			Data[Index] = Copied.Data[Index];
		}

		return *this;
	}

	Array(const Array<Type>& Copied)
	{
		*this = Copied;
	}

	Array<Type>& operator= (Array<Type>&& Moved)
	{
		if (Data)
		{
			delete[] Data;
		}

		AllocatedSize = Moved.AllocatedSize;
		UsedSize = Moved.UsedSize;
		Data = Moved.Data;

		Moved.AllocatedSize = 0;
		Moved.UsedSize = 0;
		Moved.Data = nullptr;

		return *this;
	}

	Array(Array<Type>&& Moved)
	{
		*this = Moved;
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
			Prealocate(Max(AllocatedSize * 2, 1));
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

