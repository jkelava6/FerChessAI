

#include <Executable/MainSwitch.h>
#if MAIN_USE_JAVA_UI

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <JavaIO/JavaTokenIO.h>

#include <Windows.h>
#include <cstring>

void WriteBoard(FDoubleBoard& Board, char* Target)
{
	for (int Rank = 0; Rank < 8; ++Rank)
	{
		for (int File = 0; File < 8; ++File)
		{
			Target[8 * Rank + File] = 'a' + (char)((int)Board(Rank, File) - (int)EChessPiece::BlackKing);
		}
	}
	Target[64] = '\0';
}

void SendBoard(FDoubleBoard& Board)
{
	char BoardString[65];
	WriteBoard(Board, BoardString);
	WriteJavaToken("board", BoardString);
}

int main()
{
	FDoubleBoard Board;
	Board.DefaultBoard();
	FMinMaxAI AI;

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
