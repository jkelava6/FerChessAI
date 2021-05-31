#pragma once

extern float MapSigmoid(float PreMappedValue);
extern float MapSine(float PreMappedValue);
extern float MapStep(float PreMappedValue);

extern int RandomMappingIndex();
extern FunctionPointer(float, MappingFunctionFromIndex(int Index), float);
extern int MappingFunctionToIndex(FunctionPointer(float, Mapper, float));
