// FerChessAI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Executable/MainSwitch.h>
#if MAIN_USE_DEBUG_UI

#include <StdH.h>
#include <ChessBoard.h>
#include <FMinMaxAI.h>

#include <cstdio>

using namespace std;


int GetNumber(char Base)
{
    char Input = 0;
    do
    {
        int Ignore = scanf("%c", &Input);
    } while (Input < Base || Input >= Base + 8);
    return Input - Base;
}

void DebugBoard(FDoubleBoard& Board)
{
    for (int Rank = 7; Rank >= 0; --Rank)
    {
        for (int File = 0; File < 8; ++File)
        {
            char Piece = ' ';
            switch (Board(Rank, File))
            {
            case ChessPiece::WhitePawn:
                Piece = 'P';
                break;
            case ChessPiece::WhiteKnight:
                Piece = 'N';
                break;
            case ChessPiece::WhiteBishop:
                Piece = 'B';
                break;
            case ChessPiece::WhiteRook:
                Piece = 'R';
                break;
            case ChessPiece::WhiteQueen:
                Piece = 'Q';
                break;
            case ChessPiece::WhiteKing:
                Piece = 'K';
                break;
            case ChessPiece::BlackPawn:
                Piece = 'p';
                break;
            case ChessPiece::BlackKnight:
                Piece = 'n';
                break;
            case ChessPiece::BlackBishop:
                Piece = 'b';
                break;
            case ChessPiece::BlackRook:
                Piece = 'r';
                break;
            case ChessPiece::BlackQueen:
                Piece = 'q';
                break;
            case ChessPiece::BlackKing:
                Piece = 'k';
                break;
            default:
                break;
            }
            printf("%c", Piece);
        }
        printf("\n");
    }
    printf("\n\n");
}

int main()
{
    FDoubleBoard Board;
    Board.DefaultBoard();
    FMinMaxAI AI;

    printf("Do you want to play as white? ");
    int PlayAsWhite;
    int Ignore = scanf("%d", &PlayAsWhite);
    bool bPlayersTurn = PlayAsWhite;

    while (true)
    {
        while (bPlayersTurn)
        {
            int RankFrom;
            int FileFrom;
            int RankTo;
            int FileTo;

            DebugBoard(Board);
            printf("Your move: ");
            FileFrom = GetNumber('a');
            RankFrom = GetNumber('1');
            FileTo = GetNumber('a');
            RankTo = GetNumber('1');

            Array<int> PossibleRanks;
            Array<int> PossibleFiles;
            Board.CollectMoves(RankFrom, FileFrom, PossibleRanks, PossibleFiles);
            bool bMoveValid = false;
            const int PossibleMoves = PossibleRanks.Count();
            for (int Move = 0; Move < PossibleMoves; ++Move)
            {
                if (RankTo == PossibleRanks[Move] && FileTo == PossibleFiles[Move])
                {
                    bMoveValid = true;
                    break;
                }
            }

            if (bMoveValid)
            {
                Board.Move(RankFrom, FileFrom, RankTo, FileTo);
                DebugBoard(Board);
                Board.FlipBoard();
                bPlayersTurn = false;
            }
            else
            {
                printf("Impossible move!\n");
            }
        }

        if (!bPlayersTurn)
        {
            printf("AI move ... ");
            AI.PlayMove(Board);

            printf("%c%c - %c%c [%4.1f]\n", 'a' + AI.LastPlayedMove.FileFrom, '8' - AI.LastPlayedMove.RankFrom,
                'a' + AI.LastPlayedMove.FileTo, '8' - AI.LastPlayedMove.RankTo, AI.LastPlayedMove.Evaluation);

            Board.FlipBoard();
            bPlayersTurn = true;
        }
    }
}

#endif

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
