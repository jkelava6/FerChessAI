

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
			int RowFrom = MessageBuffer[0] - '0';
			const int FileFrom = MessageBuffer[1] - '0';
			int RowTo = MessageBuffer[2] - '0';
			const int FileTo = MessageBuffer[3] - '0';
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
				RowFrom = 7 - RowFrom;
				RowTo = 7 - RowTo;
			}

			Array<int> Rows;
			Array<int> Files;
			Board.CollectMoves(RowFrom, FileFrom, Rows, Files);
			bool bMoveValid = false;
			for (int Move = 0; Move < Rows.Count(); ++Move)
			{
				if (Rows[Move] == RowTo && Files[Move] == FileTo)
				{
					Board.Move(RowFrom, FileFrom, RowTo, FileTo);
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
			int RowFrom = MessageBuffer[0] - '0';
			const int FileFrom = MessageBuffer[1] - '0';
			int RowTo = MessageBuffer[2] - '0';
			const int FileTo = MessageBuffer[3] - '0';
			if (!bWhiteTurn)
			{
				Board.FlipBoard();
				RowFrom = 7 - RowFrom;
				RowTo = 7 - RowTo;
			}

			Array<int> Rows;
			Array<int> Files;
			Board.CollectMoves(RowFrom, FileFrom, Rows, Files);
			char Moves[128];
			for (int Move = 0; Move < Rows.Count(); ++Move)
			{
				Moves[2 * Move] = '0' + (bWhiteTurn ? Rows[Move] : 7 - Rows[Move]);
				Moves[2 * Move + 1] = '0' + Files[Move];
			}
			Moves[Rows.Count() * 2] = '\0';
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
