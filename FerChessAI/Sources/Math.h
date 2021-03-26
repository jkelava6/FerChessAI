#pragma once




template <class Type> inline Type Max(Type One, Type Other)
{
	return One > Other ? One : Other;
}

template <class Type> inline Type Min(Type One, Type Other)
{
	return One < Other ? One : Other;
}

inline int AbsI(int Value)
{
	return Value >= 0 ? Value : -Value;
}

inline float AbsF(float Value)
{
	return Value >= 0.0f ? Value : -Value;
}

inline float ClampF(float Value, float MinVal, float MaxVal)
{
	return Max(Min(Value, MaxVal), MinVal);
}

// Taylor series e^x implementation, taken from:
// https://www.geeksforgeeks.org/program-to-efficiently-calculate-ex/
inline float PowerNat(float Exp, int Precision = 20)
{
	float Res = 1.0f;

	for (int i = Precision - 1; i > 0; --i)
		Res = 1 + Exp * Res / i;

	return Res;
}

// Wang has random number generation, taken from:
// https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
static uint RandomSeed;
static inline uint GenerateWangHash(uint Seed)
{
	Seed = (Seed ^ 61) ^ (Seed >> 16);
	Seed *= 9;
	Seed = Seed ^ (Seed >> 4);
	Seed *= 0x27d4eb2d;
	Seed = Seed ^ (Seed >> 15);
	return Seed;
}

inline void SetRandomSeed(uint NewSeed)
{
	RandomSeed = NewSeed;
}

inline uint RandomI()
{
	return RandomSeed = GenerateWangHash(RandomSeed);
}

inline float RandomF()
{
	// RandomI() returns an integer value in range [0, 2^32)
	// Multipltying with 2^-32 maps this to [0, 1)
	return (float)(RandomI() * 2.3283064e-10);
}


