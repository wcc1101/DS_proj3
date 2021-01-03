#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"

using namespace std;

#define enemy_win 200000
#define WIN 100000
#define LOSE -100000
#define ORIGINAL -200000
#define ILLEGAL_MOVE -300000
#define ME 1
#define ENEMY 2
#define MAX 2147483647
#define MIN -2147483647

#define N 3 //depth

int heuristic(Board, int, int);
int adjency_critical_connected(bool[][COL], int, int, Board);
int adjency_critical(int, int, Board);
bool corner(int, int);
bool edge(int, int);
bool inrange(int, int);
bool available(Board, int, int, Player);
void print(Board);
int result(Board, Player);
int minimax(int, int, Board, Player, Player, int, int, int, int);

char color_me, color_enemy;

int adjency_critical_connected(bool counted[][COL], int row, int col, Board b)
{
    int num = 0;
    for (int i = row - 1; i <= row + 1; i++)
        for (int j = col - 1; j <= col + 1; j++)
        {
            if (inrange(i, j))
            {
                if (i == row && j == col)
                    continue;
                if (b.get_cell_color(i, j) == color_me)
                    if (b.get_orbs_num(i, j) == b.get_capacity(i, j) - 1 && !counted[i][j])
                    {
                        num++;
                        counted[i][j] = true;
                    }
            }
        }
    return num * 2;
}

int adjency_critical(int row, int col, Board b)
{
    int score = 0;
    for (int i = row - 1; i <= row + 1; i++)
        for (int j = col - 1; j <= col + 1; j++)
        {
            if (inrange(i, j))
            {
                if (i == row && j == col)
                    continue;
                if (b.get_cell_color(i, j) != color_me && b.get_cell_color(i, j) != 'w')
                    if (b.get_orbs_num(i, j) == b.get_capacity(i, j) - 1)
                        score -= 9 - b.get_capacity(i, j);
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

bool available(Board b, int row, int col, Player p)
{
    if (b.get_cell_color(row, col) != p.get_color() && b.get_cell_color(row, col) != 'w')
        return false;
    else
        return true;
}

int result(Board board, Player player)
{
    char player_color = player.get_color();
    int me = 0;
    int enemy = 0;

    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
        {
            if (board.get_cell_color(i, j) == player_color)
                me = 1;
            else if (board.get_cell_color(i, j) != 'w')
                enemy = 1;
        }
    if (enemy == 0 && me != 0)
        return WIN;
    if (enemy != 0 && me == 0)
        return LOSE;
    return 0;
}

int heuristic(Board b, int row, int col, Player me)
{
    if (!available(b, row, col, me))
        return ILLEGAL_MOVE;
    b.place_orb(row, col, &me);
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
            if (color_me == b.get_cell_color(i, j))
            {
                score += b.get_orbs_num(i, j);
                temp = adjency_critical(i, j, b);
                score += temp;
                if (temp == 0)
                {
                    if (corner(i, j))
                        score += 3;
                    else if (edge(i, j))
                        score += 2;

                    if (b.get_orbs_num(i, j) == b.get_capacity(i, j) - 1)
                        score += 2;
                }
                if (b.get_orbs_num(i, j) == b.get_capacity(i, j) - 1)
                    score += adjency_critical_connected(counted, i, j, b);
            }
        }
    return score;
}

int minimax(int row, int col, Board b, Player me, Player enemy, int turn, int round, int alpha, int beta)
{
    int score = ORIGINAL;
    if (round == N)
    {
        if (available(b, row, col, enemy))
            b.place_orb(row, col, &enemy);
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
                b.place_orb(row, col, &enemy);
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
                b.place_orb(row, col, &me);
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

    int ans = ORIGINAL;
    int alpha = MIN;
    int beta = MAX;
    for (int i = 0; i < ROW; i++)
        for (int j = 0; j < COL; j++)
        {
            int q;
            q = minimax(i, j, board, player, enemy, ENEMY, 0, alpha, beta);
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
