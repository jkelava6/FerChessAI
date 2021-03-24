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

// Taylor series e^x implementation, taken from:
// https://www.geeksforgeeks.org/program-to-efficiently-calculate-ex/
inline float PowerNat(float Exp, int Precision = 20)
{
	float Res = 1.0f;

	for (int i = Precision - 1; i > 0; --i)
		Res = 1 + Exp * Res / i;

	return Res;
}


