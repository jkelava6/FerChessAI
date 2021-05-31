#pragma once

extern float ConsumeAdd(float Consumer, float Input);
extern float ConsumeMultiply(float Consumer, float Input);
extern float ConsumeMax(float Consumer, float Input);

extern int RandomConsumerIndex();
extern FunctionPointer(float, ConsumerFunctionFromIndex(int Index), float, float);
extern int ConsumerFunctionToIndex(FunctionPointer(float, Consumer, float, float));
