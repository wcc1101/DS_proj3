#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;
#define N 4

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
class MOVE;
int heuristic(BOARD, int, int);
bool inrange(int, int);
bool critical(BOARD, int, int);
bool corner(int, int);
bool edge(int, int);
bool available_for_me(BOARD, int, int);

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
    ~BOARD() {}

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

class MOVE
{
public:
    int index1, index2;
    int score;
    MOVE() {}
    MOVE(int i, int j, int c)
    {
        this->index1 = i;
        this->index2 = j;
        this->score = c;
    }
};

int heuristic(BOARD b, int row, int col)
{
    b.add_orb(row, col, color_me);
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
                    if (b.ground[row][col].capacity == b.ground[row][col].num) //going to explode
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

bool available_for_me(BOARD b, int row, int col)
{
    if (b.ground[row][col].color == color_me)
        return true;
    else if (b.ground[row][col].color == 'w')
        return true;
    else
        return false;
}

bool available_for_you(BOARD b, int row, int col)
{
    if (b.ground[row][col].color == color_me)
        return false;
    else
        return true;
}

void print(BOARD b)
{
    char symbol;
    int orb_num;
    cout << "=============================================================" << endl;
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {

            symbol = b.ground[i][j].color;
            switch (symbol)
            {
            case 'r':
                symbol = 'O';
                break;
            case 'b':
                symbol = 'X';
                break;
            default:
                symbol = ' ';
                break;
            }

            orb_num = b.ground[i][j].num;
            switch (orb_num)
            {
            case 0:
                cout << "|       | ";
                break;
            case 1:
                cout << "|" << symbol << "      | ";
                break;
            case 2:
                cout << "|" << symbol << symbol << "     | ";
                break;
            case 3:
                cout << "|" << symbol << symbol << symbol << "    | ";
                break;
            case 4:
                cout << "|" << symbol << symbol << symbol << symbol << "   | ";
                break;
            case 5:
                cout << "|" << symbol << symbol << symbol << symbol << symbol << "  | ";
                break;
            case 6:
                cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << " | ";
                break;
            default:
                cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << symbol << "| ";
                break;
            }
        }
        cout << endl;
    }
    cout << "=============================================================" << endl
         << endl;
}

char color_you()
{
    if (color_me == 'r')
        return 'b';
    else
        return 'r';
}

MOVE minimax(BOARD b, int round)
{
    if (round == N)
    {
        MOVE max = MOVE(0, 0, 0);
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 6; j++)
                if (available_for_me(b, i, j))
                {
                    BOARD p = b;
                    int score = heuristic(p, i, j);
                    if (score > max.score)
                        max = MOVE(i, j, score);
                }
        return max;
    }
    else
    {
        if (round % 2 == 0)
        {
            MOVE max = MOVE(0, 0, 0);
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 6; j++)
                    if (available_for_me(b, i, j))
                    {
                        BOARD p = b;
                        p.add_orb(i, j, color_me);
                        if (p.ground[i][j].num == p.ground[i][j].capacity)
                        {
                            p.explode(i, j);
                            p.reaction_mark();
                            p.chain_reaction();
                        }
                        MOVE t = minimax(p, round + 1);
                        if (t.score > max.score)
                            max = t;
                    }
            return max;
        }
        else
        {
            MOVE min = MOVE(0, 0, 100000);
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 6; j++)
                    if (available_for_you(b, i, j))
                    {
                        BOARD p = b;
                        p.add_orb(i, j, color_you());
                        if (p.ground[i][j].num == p.ground[i][j].capacity)
                        {
                            p.explode(i, j);
                            p.reaction_mark();
                            p.chain_reaction();
                        }
                        MOVE t = minimax(p, round + 1);
                        if (t.score < min.score)
                            min = t;
                    }
            return min;
        }
    }
}

void algorithm_A(Board board, Player player, int index[])
{
    int row, col;
    color_me = player.get_color();
    BOARD b;
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 6; j++)
            b.ground[i][j] = CELL(i, j, board.get_capacity(i, j), board.get_orbs_num(i, j), board.get_cell_color(i, j));

    cout << "now board:" << endl;
    print(b);

    MOVE ans;
    ans = minimax(b, 0);

    index[0] = ans.index1;
    index[1] = ans.index2;
}
