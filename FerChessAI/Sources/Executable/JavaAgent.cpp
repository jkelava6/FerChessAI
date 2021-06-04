

#include <Executable/MainSwitch.h>
#if MAIN_USE_JAVA_UI

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <Assisted/NetEvalMinMax.h>
#include <JavaIO/JavaTokenIO.h>
#include <League/League.h>

#include <Windows.h>
#include <cstring>
#include <cstdlib>
#include <ctime>

void WriteBoard(FDoubleBoard& Board, char* Target)
{
	for (int Rank = 0; Rank < RANKS; ++Rank)
	{
		for (int File = 0; File < FILES; ++File)
		{
			Target[FILES * Rank + File] = 'a' + (char)((int)Board(Rank, File) - (int)EChessPiece::BlackKing);
		}
	}
	Target[RANKS * FILES] = '\0';
}

void SendBoard(FDoubleBoard& Board)
{
	char BoardString[RANKS * FILES + 1];
	WriteBoard(Board, BoardString);
	WriteJavaToken("board", BoardString);
}

int main()
{
	srand(time(0));
	SetRandomSeed(rand());
	const int Rnd = 4 + rand() % 5;
	for (int i = 0; i < Rnd; ++i)
	{
		RandomI();
	}
	FDoubleBoard Board;
	Board.DefaultBoard();
	FLeague League;
	League.Initialize(1, 1, 80, 20, 8, 0.05f, 0.8f, 64, 0.1f, 0.8f, 16, 0.1f, 0.03f, 0.03f);
	League.Iterate();
	FNetEvalMinMax AI;
	FDna Dna = League.GetDna(0, 0);
	AI.AccesNetwork().FromDna(Dna);

	const int BufferSize = 1000;
	char MessageBuffer[BufferSize];
	ReadJavaToken("<start>", MessageBuffer);
	SendBoard(Board);

	char TokenBuffer[BufferSize];
	const char* TokenExeMove = "move";
	const char* TokenAiMove = "ai";
	const char* TokenSearchMoves = "search";
	bool bWhiteTurn = true;
	while (true)
	{
		ReadJavaTokenAny(TokenBuffer, MessageBuffer);

		if (strcmp(TokenBuffer, TokenExeMove) == 0)
		{
			int RankFrom = MessageBuffer[0] - '0';
			const int FileFrom = MessageBuffer[1] - '0';
			int RankTo = MessageBuffer[2] - '0';
			const int FileTo = MessageBuffer[3] - '0';
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
				RankFrom = 7 - RankFrom;
				RankTo = 7 - RankTo;
			}

			TArray<int> Ranks;
			TArray<int> Files;
			Board.CollectMoves(RankFrom, FileFrom, Ranks, Files);
			bool bMoveValid = false;
			for (int Move = 0; Move < Ranks.Count(); ++Move)
			{
				if (Ranks[Move] == RankTo && Files[Move] == FileTo)
				{
					Board.MovePiece(RankFrom, FileFrom, RankTo, FileTo);
					bMoveValid = true;
					break;
				}
			}

			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			if (bMoveValid)
			{
				bWhiteTurn = !bWhiteTurn;
			}
			else
			{
				WriteJavaToken("error", "Impossible move!");
			}
			SendBoard(Board);
			continue;
		}

		if (strcmp(TokenBuffer, TokenAiMove) == 0)
		{
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			AI.PlayMove(Board);
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			bWhiteTurn = !bWhiteTurn;
			SendBoard(Board);
			continue;
		}

		if (strcmp(TokenBuffer, TokenSearchMoves) == 0)
		{
			int RankFrom = MessageBuffer[0] - '0';
			const int FileFrom = MessageBuffer[1] - '0';
			int RankTo = MessageBuffer[2] - '0';
			const int FileTo = MessageBuffer[3] - '0';
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
				RankFrom = 7 - RankFrom;
				RankTo = 7 - RankTo;
			}

			TArray<int> Ranks;
			TArray<int> Files;
			Board.CollectMoves(RankFrom, FileFrom, Ranks, Files);
			char Moves[128];
			for (int Move = 0; Move < Ranks.Count(); ++Move)
			{
				Moves[2 * Move] = '0' + (bWhiteTurn ? Ranks[Move] : 7 - Ranks[Move]);
				Moves[2 * Move + 1] = '0' + Files[Move];
			}
			Moves[Ranks.Count() * 2] = '\0';
			WriteJavaToken("moves", Moves);

			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			continue;
		}
	}

	return 0;
}

#endif
