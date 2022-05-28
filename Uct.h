#ifndef UCT_H_
#define UCT_H_

#include "Node.h"
#include "Timer.h"
#include "Point.h"
#include <iostream>

#define TIME_LIMIT 2500000000


class UCT {
    // 原始信息
    int M, N;
    int noX, noY;
    int lastX, lastY;
    int *top;
    int **board;

    // 外部信息
    Timer timer;
    Node* root = nullptr;

    static int* curTop;
    static int** curBoard;
public:
    UCT();
    UCT(int _M, int _N, int _noX, int _noY, int _lastX, int _lastY, const int* _top, int** _board);
    void update(int _lastX, int _lastY, const int* _top, int** _board);
    void boardClear();
    void boardReset();
    void boardPrint(int** _board);
    void updateRoot(Node* v, int x, int y);
    Point uctSearch();
    Node* treePolicy(Node* v);
    double defaultPolicy(Node* v);
    void backup(Node* v, double status);
    int getScore(int _x, int _y, int* _top, int** _board, bool player);
    ~UCT();

    friend class Node;
};

#endif