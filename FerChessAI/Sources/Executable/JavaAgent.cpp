

#include <Executable/MainSwitch.h>
#if MAIN_USE_JAVA_UI

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <JavaIO/JavaTokenIO.h>

#include <Windows.h>
#include <cstring>

void WriteBoard(DoubleBoard& Board, char* Target)
{
	for (int Row = 0; Row < 8; ++Row)
	{
		for (int File = 0; File < 8; ++File)
		{
			Target[8 * Row + File] = 'a' + (char)((int)Board(Row, File) - (int)ChessPiece::BlackKing);
		}
	}
	Target[64] = '\0';
}

void SendBoard(DoubleBoard& Board)
{
	char BoardString[65];
	WriteBoard(Board, BoardString);
	WriteJavaToken("board", BoardString);
}

int main()
{
	DoubleBoard Board;
	Board.DefaultBoard();
	MinMaxAI AI;

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
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			// read the move
			Board.Move(0, 0, 0, 0);
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			bWhiteTurn = !bWhiteTurn;
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
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			// implement move search
			WriteJavaToken("moves", "");
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
			}
			bWhiteTurn = !bWhiteTurn;
			continue;
		}
	}

	return 0;
}

#endif
