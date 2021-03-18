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

inline int AbsF(float Value)
{
	return Value >= 0.0f ? Value : -Value;
}


