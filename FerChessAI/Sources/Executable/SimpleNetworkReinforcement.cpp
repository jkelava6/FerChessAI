
#include <Executable/MainSwitch.h>
#if MAIN_USE_SIMPLE_NET_REINFORCE

#include <StdH.h>

#include <NeuNet/Dna.h>
#include <NeuNet/NeuNetAI.h>
#include <NeuNet/Node.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

int main()
{
	const bool bWithRecurrent = true;

	FDna Dna;
	Dna.PushInt(2);
	Dna.PushInt(1);
	Dna.PushInt(bWithRecurrent ? 1 : 0);
	if (bWithRecurrent)
	{
		Dna.PushInt(1);
	}
	Dna.PushInt(1);
	if (bWithRecurrent)
	{
		Dna.PushFloat(0.0f);
	}

	Dna.PushFloat(0.0f);
	Dna.PushInt(2);
	Dna.PushInt(0);
	Dna.PushFloat(0.0f);
	Dna.PushInt(1);
	Dna.PushFloat(0.0f);

	Dna.PushFloat(0.0f);
	Dna.PushInt(bWithRecurrent ? 3 : 2);
	Dna.PushInt(0);
	Dna.PushFloat(0.0f);
	Dna.PushInt(bWithRecurrent ? 3 : 2);
	Dna.PushFloat(0.0f);
	if (bWithRecurrent)
	{
		Dna.PushInt(2);
		Dna.PushFloat(0.0f);
	}

	Dna.PushFloat(0.0f);
	Dna.PushInt(bWithRecurrent ? 2 : 1);
	Dna.PushInt(bWithRecurrent ? 3 : 2);
	Dna.PushFloat(0.0f);
	if (bWithRecurrent)
	{
		Dna.PushInt(2);
		Dna.PushFloat(0.0f);
	}

	if (bWithRecurrent)
	{
		Dna.PushFloat(0.0f);
		Dna.PushInt(1);
		Dna.PushInt(3);
		Dna.PushFloat(0.0f);
	}

	FNetwork Net;
	Net.FromDna(Dna);

	const float TestIn1[] = { 0, 1, 1 };
	const float TestIn2[] = { 0, -1, 1 };
	const float TestOut[] = { 0, 1, 1 };
	const int TestCount = ARRAY_SIZE(TestIn1);
	ARRAY_MATCH(TestIn1, TestIn2);
	ARRAY_MATCH(TestIn1, TestOut);
	float Epsilon = 0.05f;

	while (true)
	{
		int ReinforcementCounts[TestCount] = { 0 };

		for (int Test = 0; Test < TestCount; ++Test)
		{
			Net.SetInput(0, TestIn1[0]);
			Net.SetInput(1, TestIn2[0]);
			if (bWithRecurrent)
			{
				Net.ResetRecurrent(0);
			}
			Net.Update();
			if (bWithRecurrent)
			{
				Net.Update();
			}

			while (AbsF(SigmoidFunction(TestOut[Test]) - Net.GetOutput(0)) > Epsilon)
			{
				Net.ReinforceOutput(0, SigmoidFunction(TestOut[Test]), true, 0.1f, 1.0f, 0.1f, 2.0f,
					bWithRecurrent ? 1 : 0, FNetwork::EReinforcementType::Full);
				++ReinforcementCounts[Test];

				if (bWithRecurrent)
				{
					Net.ResetRecurrent(0);
				}
				Net.Update();
				if (bWithRecurrent)
				{
					Net.Update();
				}
			}
		}

		for (int Test = 0; Test < TestCount; ++Test)
		{
			if (Test > 0)
			{
				printf(" | ");
			}
			printf("%d", ReinforcementCounts[Test]);
		}
		printf("\n");
	}

	return 0;
}

#endif
