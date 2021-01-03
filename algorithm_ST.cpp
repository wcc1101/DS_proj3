#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;
#define N 3
#define enemy_win 200000
#define WIN 100000
#define LOSE -100000
#define ORIGINAL -20000
#define ILLEGAL_MOVE -300000
#define ME 1
#define ENEMY 2
#define MAX 2147483647
#define MIN -2147483647

class CELL;
class BOARD;
int heuristic(BOARD, int, int);
int adjency_critical_connected(bool[][COL], int, int, BOARD);
bool corner(int, int);
bool edge(int, int);
bool inrange(int, int);
bool available(BOARD, int, int);
void print(BOARD);
int result(BOARD, Player);
int minimax(int, int, BOARD, Player, Player, int, int, int, int);

char color_me, color_enemy;

class CELL
{
public:
    int num, capacity, row, col;
    char color;

    CELL(){};
    ~CELL(){};
    CELL(int row, int col, int capacity, int num, char color)
    {
        this->row = row;
        this->col = col;
        this->capacity = capacity;
        this->num = num;
        this->color = color;
    }
    CELL(const CELL &a)
    {
        this->row = a.row;
        this->col = a.col;
        this->capacity = a.capacity;
        this->num = a.num;
        this->color = a.color;
    }
};

class BOARD
{
public:
    CELL ground[ROW][COL];

    BOARD(){};
    BOARD(const BOARD &a)
    {
        for (int i = 0; i < ROW; i++)
            for (int j = 0; j < COL; j++)
                this->ground[i][j] = a.ground[i][j];
    }
    ~BOARD(){};

    void add_orb(int r, int c, char color)
    {
        this->ground[r][c].num++;
        this->ground[r][c].color = color;
    }
};

int adjency_critical_connected(bool counted[][COL], int row, int col, BOARD b)
{
    int num = 0;
    for (int i = row - 1; i <= row + 1; i++)
        for (int j = col - 1; j <= col + 1; j++)
        {
            if (inrange(i, j))
            {
                if (i == row && j == col)
                    continue;
                if (b.ground[i][j].color == color_me)
                    if (b.ground[i][j].num == b.ground[i][j].capacity - 1 && !counted[i][j])
                    {
                        num++;
                        counted[i][j] = true;
                    }
            }
        }
    return num * 2;
}

int adjency_critical(int row, int col, BOARD b)
{
    int score = 0;
    for (int i = row - 1; i <= row + 1; i++)
        for (int j = col - 1; j <= col + 1; j++)
        {
            if (inrange(i, j))
            {
                if (i == row && j == col)
                    continue;
                if (b.ground[i][j].color != color_me && b.ground[i][j].color != 'w')
                    if (b.ground[i][j].num == b.ground[i][j].capacity - 1)
                        score -= 9 - b.ground[i][j].capacity;
            }
        }
    return score;
}

bool corner(int row, int col)
{
    if (row == 0)
        if (col == 0 || col == COL - 1)
            return true;
        else if (row == ROW - 1)
            if (col == 0 || col == COL - 1)
                return true;
    return false;
}

bool edge(int row, int col)
{
    if (row == 0)
        if (col != 0 && col != COL - 1)
            return true;
        else if (row == ROW - 1)
            if (col != 0 && col != COL - 1)
                return true;
    return false;
}

bool inrange(int row, int col)
{
    if (row < 0 || row >= ROW || col < 0 || col >= COL)
        return false;
    return true;
}

bool available(BOARD b, int row, int col, Player p)
{
    if (b.ground[row][col].color != p.get_color() && b.ground[row][col].color != 'w')
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

int result(BOARD board, Player player)
{
    char player_color = player.get_color();
    int me = 0;
    int enemy = 0;

    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
        {
            if (board.ground[i][j].color == player_color)
                me = 1;
            else if (board.ground[i][j].color != 'w')
                enemy = 1;
        }
    if (enemy == 0 && me != 0)
        return WIN;
    if (enemy != 0 && me == 0)
        return LOSE;
    return 0;
}

int heuristic(BOARD b, int row, int col, Player me)
{
    if (!available(b, row, col, me))
        return ILLEGAL_MOVE;
    b.add_orb(row, col, color_me);
    if (result(b, me) == WIN)
        return WIN;
    else if (result(b, me) == LOSE)
        return LOSE;

    int score = 0;
    bool counted[ROW][COL] = {false};
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
        {
            int temp;
            if (color_me == b.ground[i][j].color)
            {
                score += b.ground[i][j].num;
                temp = adjency_critical(i, j, b);
                score += temp;
                if (temp == 0)
                {
                    if (corner(i, j))
                        score += 3;
                    else if (edge(i, j))
                        score += 2;

                    if (b.ground[i][j].num == b.ground[i][j].capacity - 1)
                        score += 2;
                }
                if (b.ground[i][j].num == b.ground[i][j].capacity - 1)
                    score += adjency_critical_connected(counted, i, j, b);
            }
        }
    return score;
}

int minimax(int row, int col, BOARD b, Player me, Player enemy, int turn, int round, int alpha, int beta)
{
    int score = ORIGINAL;
    if (round == N)
    {
        if (available(b, row, col, enemy))
            b.add_orb(row, col, color_enemy);
        else
            return ILLEGAL_MOVE;
        if (result(b, me) == WIN)
            return WIN;
        else if (result(b, me) == LOSE)
            return LOSE;

        for (int i = 0; i < ROW; i++)
            for (int j = 0; j < COL; j++)
            {
                int q = heuristic(b, i, j, me);
                if (score < q)
                    score = q;
            }
        return score;
    }
    else
    {
        if (turn == ME)
        {
            if (available(b, row, col, enemy))
                b.add_orb(row, col, color_enemy);
            else
                return ILLEGAL_MOVE;
            if (result(b, me) == WIN)
                return WIN;
            else if (result(b, me) == LOSE)
                return LOSE;

            for (int i = 0; i < ROW; i++)
                for (int j = 0; j < COL; j++)
                {
                    int q = minimax(i, j, b, me, enemy, ENEMY, round + 1, alpha, beta);
                    if (score < q && q != ILLEGAL_MOVE)
                        score = q;
                    if (alpha < q)
                        alpha = q;
                    if (beta <= alpha)
                        return score;
                }
            return score;
        }
        else if (turn == ENEMY)
        {
            if (available(b, row, col, me))
                b.add_orb(row, col, color_me);
            else
                return ILLEGAL_MOVE;
            if (result(b, me) == WIN)
                return WIN;
            else if (result(b, me) == LOSE)
                return LOSE;

            score = enemy_win;
            for (int i = 0; i < ROW; i++)
                for (int j = 0; j < COL; j++)
                {
                    int q = minimax(i, j, b, me, enemy, ME, round + 1, alpha, beta);
                    if (score > q && q != ILLEGAL_MOVE)
                        score = q;
                    if (beta > q && q > ILLEGAL_MOVE)
                        beta = q;
                    if (beta <= alpha)
                        return score;
                }
            return score;
        }
    }
    return score;
}

void algorithm_A(Board board, Player player, int index[])
{
    color_me = player.get_color();
    color_enemy = (color_me == 'r') ? 'b' : 'r';
    Player enemy(color_enemy);

    BOARD b;
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
            b.ground[i][j] = CELL(i, j, board.get_capacity(i, j), board.get_orbs_num(i, j), board.get_cell_color(i, j));

    int ans = ORIGINAL;
    int alpha = MIN;
    int beta = MAX;
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
        {
            int q;
            q = minimax(i, j, b, player, enemy, ENEMY, 0, alpha, beta);
            if (ans < q && q != ILLEGAL_MOVE)
            {
                index[0] = i;
                index[1] = j;
                ans = q;
            }
            if (alpha < q)
                alpha = q;
            if (beta <= alpha)
                return;
        }
}
