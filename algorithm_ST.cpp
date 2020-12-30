#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;

/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions 
 * 1. ~ 4. are listed in next block)
 * 
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 * 
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/
class BOARD;
class CELL;
int heuristic(BOARD, int, int);
bool inrange(int, int);
bool critical(BOARD, int, int);
bool corner(int, int);
bool edge(int, int);

char color_me;

class CELL
{
public:
    int num, capacity, row, col;
    char color;
    bool explode;

    CELL(){};
    ~CELL(){};
    CELL(int row, int col, int capacity, int num, char color)
    {
        this->row = row;
        this->col = col;
        this->capacity = capacity;
        this->num = num;
        this->color = color;
        this->explode = false;
    }
    CELL(const CELL &a)
    {
        this->row = a.row;
        this->col = a.col;
        this->capacity = a.capacity;
        this->num = a.num;
        this->color = a.color;
        this->explode = a.explode;
    }
};

class BOARD
{
public:
    CELL ground[5][6];

    BOARD() {}
    BOARD(const BOARD &a)
    {
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                this->ground[i][j] = a.ground[i][j];
    }
    ~BOARD(){};

    void cell_reset(int r, int c)
    {
        this->ground[r][c].num = 0;
        this->ground[r][c].color = 'w';
        this->ground[r][c].explode = false;
    }

    void add_orb(int r, int c, char color)
    {
        this->ground[r][c].num++;
        this->ground[r][c].color = color;
    }

    void explode(int r, int c)
    {
        char color = this->ground[r][c].color;
        cell_reset(r, c);
        for (int i = r - 1; i <= r + 1; i++)
            for (int j = c - 1; j <= c + 1; j++)
                if (inrange(i, j) && i != r && j != c)
                    add_orb(i, j, color);
    }

    void reaction_mark()
    {
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++)
                if (this->ground[i][j].num >= this->ground[i][j].capacity)
                    this->ground[i][j].explode = true;
    }

    void chain_reaction()
    {
        bool chain_reac = true;
        while (chain_reac)
        {
            chain_reac = false;
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 6; j++)
                    if (this->ground[i][j].explode)
                    {
                        this->explode(i, j);
                        chain_reac = true;
                    }
            if (win())
                return;
            this->reaction_mark();
        }
    }

    bool win()
    {
        int flag_all_me = 1;
        int flag_all_not_me = 1;
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++)
            {
                if (this->ground[i][j].color == color_me)
                    flag_all_not_me = 0;
                if (this->ground[i][j].color != 'w' && this->ground[i][j].color != color_me)
                    flag_all_me = 0;
            }
        if (flag_all_me || flag_all_not_me)
            return true;
    }
};

int heuristic(BOARD b, int row, int col)
{
    int flag_all_me = 1;
    int flag_all_not_me = 1;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 6; j++)
        {
            if (b.ground[i][j].color == color_me)
                flag_all_not_me = 0;
            if (b.ground[i][j].color != 'w' && b.ground[i][j].color != color_me)
                flag_all_me = 0;
        }
    if (flag_all_me) //win
        return 10000;
    if (flag_all_not_me) //lose
        return -10000;
    int value = 0;
    int flag_no_critical = 1;
    for (int r = row - 1; r <= row + 1; r++)
    {
        for (int c = col - 1; c <= col + 1; c++)
        {
            if (r == row && c == col)
                continue;
            if (inrange(r, c))
            {
                if (critical(b, r, c))
                {
                    flag_no_critical = 0;
                    if (critical(b, row, col))
                        value += b.ground[r][c].capacity;
                    else if (b.ground[r][c].color != b.ground[row][col].color)
                        value -= b.ground[r][c].capacity;
                }
            }
        }
    }
    if (flag_no_critical)
    {
        if (corner(row, col))
            value += 5;
        else if (edge(row, col))
            value += 3;
    }
    return value;
}

bool corner(int row, int col)
{
    if (row == 0)
        if (col == 0 || col == 5)
            return true;
        else if (row == 4)
            if (col == 0 || col == 5)
                return true;
    return false;
}

bool edge(int row, int col)
{
    if (row == 0)
        if (col != 0 && col != 5)
            return true;
        else if (row == 4)
            if (col != 0 && col != 5)
                return true;
    return false;
}

bool critical(BOARD b, int row, int col)
{
    if (b.ground[row][col].num == b.ground[row][col].capacity - 1)
        return true;
    return false;
}

bool inrange(int row, int col)
{
    if (row < 0 || row >= 5 || col < 0 || col >= 6)
        return false;
    return true;
}

void algorithm_A(Board board, Player player, int index[])
{
    int row, col;
    color_me = player.get_color();
    BOARD b;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 6; j++)
            b.ground[i][j] = CELL(i, j, board.get_capacity(i, j), board.get_orbs_num(i, j), board.get_cell_color(i, j));

    int max = 0;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 6; j++)
        {
            BOARD p = b;
            p.add_orb(i, j, color_me);
            cout << "h:" << heuristic(p, i, j) << endl;
            if (heuristic(p, i, j) > max)
            {
                max = heuristic(p, i, j);
                row = i;
                col = j;
            }
        }

    index[0] = row;
    index[1] = col;
}