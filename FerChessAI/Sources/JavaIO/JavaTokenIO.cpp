

#include <StdH.h>
#include <JavaIO/JavaTokenIO.h>

#include <Windows.h>
#include <cassert>
#include <cstdio>
#include <cstring>

extern void ReadJavaToken(const char* Token, char* OutBuffer, int BufferSize)
{
	const int TokenLength = strlen(Token);
	const char* EndSequence = "</>";
	const int EndSeqLen = strlen(EndSequence);

	int TokenProgress = 0;
	int EndProgress = 0;
	int MessageLen = 0;

	while (true)
	{
		char ReadChar;
		int ReadCount = scanf("%c", &ReadChar);
		if (ReadCount < 1)
		{
			Sleep(10);
			continue;
		}

		if (TokenProgress < TokenLength)
		{
			if (ReadChar == Token[TokenProgress])
			{
				++TokenProgress;
			}
			else
			{
				TokenProgress = 0;
			}
		}

		if (EndProgress < EndSeqLen)
		{
			if (ReadChar == EndSequence[EndProgress])
			{
				++EndProgress;
			}
			else
			{
				EndProgress = 0;
			}
			if (EndProgress == EndSeqLen)
			{
				OutBuffer[MessageLen] = '\0';
				break;
			}
		}

		OutBuffer[MessageLen++] = ReadChar;
		if (MessageLen >= BufferSize)
		{
			assert(false);
		}
	}
}

extern void WriteJavaToken(const char* Token, const char* Message)
{
	printf("<%s>%s</>", Token, Message);
}

