#pragma once

#include <Math.h>


template <class Type> class TArray
{
private:
	Type* Data = nullptr;
	int AllocatedSize = 0;
	int UsedSize = 0;


public:
	TArray(int InitialSize = 0)
	{
		Prealocate(InitialSize);
	}

	~TArray()
	{
		if (Data)
		{
			delete[] Data;
		}
	}

	TArray<Type>& operator= (const TArray<Type>& Copied)
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

	TArray(const TArray<Type>& Copied)
	{
		*this = Copied;
	}

	TArray<Type>& operator= (TArray<Type>&& Moved)
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

	TArray(TArray<Type>&& Moved)
	{
		*this = Moved;
	}

	void Prealocate(int Size)
	{
		if (Size == AllocatedSize)
		{
			return;
		}

		Type* NewData = Size > 0 ? new Type[Size] : nullptr;
		UsedSize = Min(Size, UsedSize);

		for (int Index = 0; Index < UsedSize; ++Index)
		{
			NewData[Index] = Data[Index];
		}
		if (Data)
		{
			delete[] Data;
		}

		Data = NewData;
		AllocatedSize = Size;
	}

	Type& Push()
	{
		if (UsedSize == AllocatedSize)
		{
			Prealocate(AllocatedSize != 0 ? AllocatedSize * 2 : 16);
		}
		return Data[UsedSize++];
	}

	Type&& Pop()
	{
		return Move(Data[UsedSize--]);
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

	void PopAll(bool bSetToDefault = false)
	{
		if (bSetToDefault)
		{
			for (int Index = 0; Index < UsedSize; ++Index)
			{
				Data[Index] = Type();
			}
		}
		UsedSize = 0;
	}

	void Clear()
	{
		if (Data)
		{
			delete[] Data;
		}
		Data = nullptr;
		UsedSize = 0;
		AllocatedSize = 0;
	}

};

