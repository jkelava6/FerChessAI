#include <StdH.h>
#include <Executable/DebugUtils.h>

#include <ChessBoard.h>

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>

extern void DebugBoard(FDoubleBoard& Board)
{
    for (int Rank = RANKS - 1; Rank >= 0; --Rank)
    {
        for (int File = 0; File < FILES; ++File)
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
