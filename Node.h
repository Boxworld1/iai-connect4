#ifndef NODE_H_
#define NODE_H_

#include <iostream>
#include <vector>
#include <cmath>
#include "Judge.h"
#include "Timer.h"
#include "Point.h"

class Node {
    // 原始信息
    int M, N;
    int noX, noY;

    // UCB1
    int countVisited;
    int countWin;
    int countCanMove;
    std::vector<int> canMove;

    // UCT
    double c = 0.8;
    Node *parent;
    Node **child;

    // 当前回合属性
    int posX, posY;
    bool player; // true = player, false = machine

    Timer timer;

public:
    Node();
    Node(Node* _parent, int _M, int _N, int _noX, int _noY, int _posX, int _posY, bool _player);
    Node* bestChild(bool move);
    bool canExpend();
    bool end();
    bool getPlayer();
    Point getMove();
    Node* expand();
    void clearChild();
    ~Node();

    friend class UCT;
};

#endif