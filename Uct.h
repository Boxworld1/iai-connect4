#ifndef UCT_H_
#define UCT_H_

#include "Node.h"
#include "Timer.h"
#include "Point.h"
#include <iostream>

#define TIME_LIMIT 2000000000


class UCT {
    // 原始信息
    int M, N;
    int noX, noY;
    int lastX, lastY;
    int *top;
    int **board;

    // 外部信息
    Timer timer;
    Node* root;

    static int* curTop;
    static int** curBoard;
public:
    UCT();
    UCT(int _M, int _N, int _noX, int _noY, int _lastX, int _lastY, const int* _top, int** _board);
    void boardClear();
    void boardReset();
    Point uctSearch();
    Node* treePolicy(Node* v);
    int defaultPolicy(Node* v);
    void backup(Node* v, int status);
    int getScore(int _x, int _y, int* _top, int** _board, bool player);
    ~UCT();

    friend class Node;
};

#endif