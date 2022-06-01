#include "Uct.h"

// 全局变量初始化
int* UCT::curTop = nullptr;
int** UCT::curBoard = nullptr;
Timer UCT::timer = Timer();

UCT::UCT() {}

UCT::UCT(int _M, int _N, int _noX, int _noY, int _lastX, int _lastY, const int* _top, int** _board)
    : M(_M), N(_N), noX(_noX), noY(_noY) {
    update(_lastX, _lastY, _top, _board);
}

void UCT::update(int _lastX, int _lastY, const int* _top, int** _board) {
    // 初始化
    lastX = _lastX;
    lastY = _lastY;
    timer.set();

    // 储存列顶信息
    top = new int[N];
    for (int i = 0; i < N; i++) {
        top[i] = _top[i];
    }

    // 记录当前棋盘信息
    board = new int*[M];
    for (int i = 0; i < M; i++) {
        board[i] = new int[N];
        for (int j = 0; j < N; j++) {
            board[i][j] = _board[i][j];
        }
    }
    boardReset();
}

void UCT::boardClear() {
    if (curTop) {
        delete[] curTop;
        curTop = nullptr;
    }
    if (curBoard) {
        for (int i = 0; i < M; i++)
            delete[] curBoard[i];
        delete[] curBoard;
        curBoard = nullptr;
    }
}

void UCT::boardReset() {
    boardClear();
    curTop = new int[N];
    for (int i = 0; i < N; i++) {
        curTop[i] = top[i];
    }

    // 记录当前棋盘信息
    curBoard = new int*[M];
    for (int i = 0; i < M; i++) {
        curBoard[i] = new int[N];
        for (int j = 0; j < N; j++) {
            curBoard[i][j] = board[i][j];
        }
    }
}

void UCT::updateRoot(Node* v, int x, int y) {
    if (!v)
        return;
    Node* newRoot = nullptr;
    for (int i = 0; i < N; i++) {
        if (v->child[i]) {
            if (i != y) {
                // 若多余子节点存在, 则删去
                v->child[i]->clearChild();
                delete v->child[i];
                v->child[i] = nullptr;
            } else {
                // 只保留真正下棋位置对应节点
                newRoot = v->child[i];
            }
        }
    }
    root = newRoot;
}

Point UCT::uctSearch() {
    updateRoot(root, lastX, lastY);
    if (!root)
        root = new Node(nullptr, M, N, noX, noY, lastX, lastY, true);

    while (timer.get() < TIME_LIMIT) {
        boardReset();
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::uctSearch - treepolicy]\n";
        }
        Node* vl = treePolicy(root);
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::uctSearch - defaultPolicy]\n";
        }
        double delta = defaultPolicy(vl);
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::uctSearch - backup]\n";
        }
        backup(vl, delta);

        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::uctSearch]\n";
        }
    }

    Point tar = root->bestChild(false)->getMove();
    updateRoot(root, tar.x, tar.y);
    return tar;
}

Node* UCT::treePolicy(Node* v) {
    while (!v->end()) {
        if (v->canExpend()) {
            return v->expand();
        } else {
            v = v->bestChild(true);
        }
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::treePolicy]\n";
        }
    }
    return v;
}

double UCT::defaultPolicy(Node* v) {
    int x = v->posX;
    int y = v->posY;

    // 复制棋盘
    int* tmpTop = new int[N];
    for (int i = 0; i < N; i++) {
        tmpTop[i] = curTop[i];
    }

    int** tmpBoard = new int*[M];
    for (int i = 0; i < M; i++) {
        tmpBoard[i] = new int[N];
        for (int j = 0; j < N; j++) {
            tmpBoard[i][j] = curBoard[i][j];
        }
    }

    bool orgPlayer = v->player;
    bool player = orgPlayer;
    int score = getScore(x, y, tmpTop, tmpBoard, orgPlayer, player);

    // 随机下子
    while (score > 1) {
        // 回合切换
        player = !player;

        // 找出下子点
        srand(timer.get());
        int idx = rand() % N;
        while (!tmpTop[idx]) {
            idx = rand() % N;
            if (timer.get() > TLE_TIME) {
                std::cerr << "[UCT::defaultPolicy - rand]\n";
            }
        }

        // 下棋
        int x = --tmpTop[idx], y = idx;
        tmpBoard[x][y] = (player ? 1 : 2);

        // 若下棋位置的再下一位是不可下棋点, 则跳过
        if (x - 1 == noX && y == noY) {
            tmpTop[y]--;
        }

        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::defaultPolicy]\n";
        }

        score = getScore(x, y, tmpTop, tmpBoard, orgPlayer, player);
    }

    delete[] tmpTop;
    for (int i = 0; i < M; i++)
        delete[] tmpBoard[i];
    delete[] tmpBoard;

    return score;
}

double UCT::getScore(int _x, int _y, int* _top, int** _board, bool _orgPlayer, bool _player) {
    if (timer.get() > TLE_TIME) {
        std::cerr << "[UCT::getScore]\n";
    }
    if (_x < 0 || _y < 0)
        return 5;
    if (_player && userWin(_x, _y, M, N, _board)) {
        if (_orgPlayer)
            return 1;
        return -1;
    }
    if (!_player && machineWin(_x, _y, M, N, _board)) {
        if (_orgPlayer)
            return -1;
        return 1;
    }
    if (isTie(N, _top))
        return 0;
    return 10;
}

void UCT::backup(Node* v, double status) {
    while (v) {
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::defaultPolicy - bf]\n";
        }
        v->countVisited++;
        v->countWin += status;
        status = -status;
        v = v->parent;
        if (timer.get() > TLE_TIME) {
            std::cerr << "[UCT::defaultPolicy]\n";
        }
    }
}

UCT::~UCT() {
    delete[] top;
    for (int i = 0; i < M; i++)
        delete board[i];
    delete[] board;
}