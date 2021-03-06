#pragma once

#include <AiMath.h>
#include <BuildFlags.h>

#ifdef _DEBUG
#include <cassert>
#endif
#include <Threading/ThreadingUtils.h>


#if !ALLOW_NON_THREADED_ARRAY_ALLOC
#define ARRAY_THREAD_GUARD() THREAD_GUARD()
#else
#define ARRAY_THREAD_GUARD() (void)0
#endif // !ALLOW_NON_THREADED_ARRAY_ALLOC


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
			ARRAY_THREAD_GUARD();
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
		ARRAY_THREAD_GUARD();
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

	TArray<Type>& operator= (TArray<Type>&& Moved) noexcept
	{
		if (Data)
		{
			ARRAY_THREAD_GUARD();
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

	TArray(TArray<Type>&& Moved) noexcept
	{
		*this = Move(Moved);
	}

	void Prealocate(int Size)
	{
		if (Size == AllocatedSize)
		{
			return;
		}

		ARRAY_THREAD_GUARD();
		Type* NewData = Size > 0 ? new Type[Size] : nullptr;
		UsedSize = Min(Size, UsedSize);

		for (int Index = 0; Index < UsedSize; ++Index)
		{
			NewData[Index] = Move(Data[Index]);
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

	int Count() const
	{
		return UsedSize;
	}

	Type& operator[] (int Index)
	{
#ifdef _DEBUG
		assert(0 <= Index && Index < UsedSize);
#endif
		return Data[Index];
	}

	const Type& operator[] (int Index) const
	{
#ifdef _DEBUG
		assert(0 <= Index && Index < UsedSize);
#endif
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

	int IndexOf(Type* Pointer)
	{
		const int Index = (int)(Pointer - Data);
#if _DEBUG
		assert(0 <= Index && Index < UsedSize);
#endif
		return Index;
	}

	void LendData(Type* InData, int Size, bool bUsed = false)
	{
		Data = InData;
		AllocatedSize = Size;
		UsedSize = bUsed ? Size : 0;
	}

	void RemoveLend()
	{
		Data = nullptr;
		AllocatedSize = 0;
		UsedSize = 0;
	}

};

