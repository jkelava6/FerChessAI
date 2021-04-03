// FerChessAI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Executable/MainSwitch.h>
#if MAIN_USE_DEBUG_UI

#include <StdH.h>
#include <ChessBoard.h>
#include <MinMaxAI.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

using namespace std;


int GetNumber(char Base)
{
    char Input = 0;
    do
    {
        int Ignore = scanf_s("%c", &Input);
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
            case EChessPiece::WhitePawn:
                Piece = 'P';
                break;
            case EChessPiece::WhiteKnight:
                Piece = 'N';
                break;
            case EChessPiece::WhiteBishop:
                Piece = 'B';
                break;
            case EChessPiece::WhiteRook:
                Piece = 'R';
                break;
            case EChessPiece::WhiteQueen:
                Piece = 'Q';
                break;
            case EChessPiece::WhiteKing:
                Piece = 'K';
                break;
            case EChessPiece::BlackPawn:
                Piece = 'p';
                break;
            case EChessPiece::BlackKnight:
                Piece = 'n';
                break;
            case EChessPiece::BlackBishop:
                Piece = 'b';
                break;
            case EChessPiece::BlackRook:
                Piece = 'r';
                break;
            case EChessPiece::BlackQueen:
                Piece = 'q';
                break;
            case EChessPiece::BlackKing:
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
    int Ignore = scanf_s("%d", &PlayAsWhite);
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

            TArray<int> PossibleRanks;
            TArray<int> PossibleFiles;
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
                Board.MovePiece(RankFrom, FileFrom, RankTo, FileTo);
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
