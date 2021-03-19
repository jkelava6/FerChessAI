

#include <Executable/MainSwitch.h>
#if MAIN_USE_JAVA_UI

#include <StdH.h>

#include <ChessBoard.h>
#include <MinMaxAI.h>
#include <JavaIO/JavaTokenIO.h>

#include <Windows.h>

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

int main()
{
	char BoardString[65];
	DoubleBoard Board;
	Board.DefaultBoard();
	MinMaxAI AI;

	WriteBoard(Board, BoardString);

	const int BufferSize = 1000;
	char InputBuffer[BufferSize];
	ReadJavaToken("<start>", InputBuffer, BufferSize);
	WriteJavaToken("board", BoardString);

	return 0;
}

#endif
