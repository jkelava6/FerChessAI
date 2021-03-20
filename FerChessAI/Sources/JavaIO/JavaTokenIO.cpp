

#include <StdH.h>
#include <JavaIO/JavaTokenIO.h>

#include <Windows.h>
#include <cassert>
#include <cstdio>
#include <stdlib.h>
#include <cstring>

const int MAX_TOKEN_WAIT = 60000;
const int STEP_TOKEN_WAIT = 10;

extern void ReadJavaToken(const char* Token, char* OutMessage)
{
	const int TokenLength = strlen(Token);
	const char* EndSequence = "</>";
	const int EndSeqLen = strlen(EndSequence);

	int TokenProgress = 0;
	int EndProgress = 0;
	int MessageLen = 0;
	int CurrentWait = 0;

	while (true)
	{
		char ReadChar;
		int ReadCount = scanf("%c", &ReadChar);
		if (ReadCount < 1)
		{
			Sleep(STEP_TOKEN_WAIT);
			CurrentWait += STEP_TOKEN_WAIT;
			if (CurrentWait > MAX_TOKEN_WAIT)
			{
				exit(0);
			}
			continue;
		}
		CurrentWait = 0;

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
				OutMessage[MessageLen] = '\0';
				break;
			}
		}

		OutMessage[MessageLen++] = ReadChar;
	}
}

extern void WriteJavaToken(const char* Token, const char* Message)
{
	printf("<%s>%s</>", Token, Message);
	fflush(stdout);
}

extern void ReadJavaTokenAny(char* OutToken, char* OutMessage)
{
	const char* EndSequence = "</>";
	const int EndSeqLen = strlen(EndSequence);

	int TokenLength = -1;
	bool bTokenEnded = false;
	int EndProgress = 0;
	int MessageLen = 0;
	int CurrentWait = 0;

	while (true)
	{
		char ReadChar;
		int ReadCount = scanf("%c", &ReadChar);
		if (ReadCount < 1)
		{
			Sleep(STEP_TOKEN_WAIT);
			CurrentWait += STEP_TOKEN_WAIT;
			if (CurrentWait > MAX_TOKEN_WAIT)
			{
				exit(0);
			}
			continue;
		}
		CurrentWait = 0;

		if (TokenLength < 0)
		{
			if (ReadChar == '<')
			{
				TokenLength = 0;
			}
			continue;
		}

		if (!bTokenEnded)
		{
			if (ReadChar == '>')
			{
				OutToken[TokenLength] = '\0';
				bTokenEnded = true;
			}
			else
			{
				OutToken[TokenLength++] = ReadChar;
			}
			continue;
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
				OutMessage[MessageLen] = '\0';
				break;
			}
			if (EndProgress > 0)
			{
				continue;
			}
		}

		OutMessage[MessageLen++] = ReadChar;
	}
}

