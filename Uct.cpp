#include "Uct.h"

// 全局变量初始化
int *UCT::curTop = nullptr;
int **UCT::curBoard = nullptr;

UCT::UCT() {}

UCT::UCT(int _M, int _N, int _noX, int _noY, int _lastX, int _lastY, const int* _top, int** _board):
    M(_M), N(_N), noX(_noX), noY(_noY) {
    update(_lastX, _lastY, _top, _board);
}

void UCT::update(int _lastX, int _lastY, const int* _top, int** _board) {

    // std::cerr << "[UCT::UCT] init\n";
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
    // std::cerr << "[UCT::UCT] init finished\n";

}

void UCT::boardClear() {
    if (curTop) {
        delete[] curTop;
        curTop = nullptr;
    }
    if (curBoard) {
        for (int i = 0; i < M; i++) delete[] curBoard[i];
        delete[] curBoard;
        curBoard = nullptr;
    }
}

void UCT::boardReset() {
    boardClear();
    // std::cerr << "[UCT::boardReset]\n";
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

void UCT::boardPrint(int** _board) {
    // std::cerr << "[UCT::boardPrint]\n";
    if (!_board) {
        std::cerr << "Wrong parameter\n";
        return;
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            std::cerr << _board[i][j] << " ";
        }
        std::cerr << "\n";
    }
    std::cerr << "\n";
}

void UCT::updateRoot(Node* v, int x, int y) {
    if (!v) return;
    Node* newRoot = nullptr;
    for (int i = 0; i < N; i++) {
        if (i != y) {
            // 若多余子节点存在, 则删去
            if (v->child[i]) {
                v->child[i]->clearChild();
                // delete v->child[i];
                // v->child[i] = nullptr;
            }
        } else {
            // 只保留真正下棋位置对应节点
            newRoot = v->child[i];
        }
    }
    root = newRoot;
}

Point UCT::uctSearch() {
    updateRoot(root, lastX, lastY);
    if (!root) root = new Node(nullptr, M, N, noX, noY, lastX, lastY, true);
    std::cerr << "[ROOT status]" << root->countWin << " " << root->countVisited << "\n";

    while (timer.get() < TIME_LIMIT) {
        boardReset();
        Node* vl = treePolicy(root);
        double delta = defaultPolicy(vl);
        backup(vl, delta);
    }

    Point tar = root->bestChild(false)->getMove();
    updateRoot(root, tar.x, tar.y);
    if (root) std::cerr << "[ROOT status]" << root->countWin << " " << root->countVisited << "\n";
    return tar;
}

Node* UCT::treePolicy(Node* v) {
    while (!v->end()) {
        if (v->canExpend()) {
            return v->expand();
        } else {
            v = v->bestChild(true);
        }
    }
    return v;
}

double UCT::defaultPolicy(Node* v) {
    int x = v->posX;
    int y = v->posY;

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

    bool player = v->player;
    int score = getScore(x, y, tmpTop, tmpBoard, player);

    while (score > 1) {
        // 回合切换
        player = !player;

        // 找出下子点
        srand(timer.get());
        int idx = rand() % N;
        while (!tmpTop[idx]) {
            idx = rand() % N;
        }

        // 下棋
        int x = --tmpTop[idx], y = idx;
        tmpBoard[x][y] = (player? 1: 2);
        // std::cerr << x << " " << y << ": " << tmpBoard[x][y] << "\n";

        // 若下棋位置的再下一位是不可下棋点, 则跳过
        if (x - 1 == noX && y == noY) {
            tmpTop[y]--;
        }

        score = getScore(x, y, tmpTop, tmpBoard, player);
    }

    delete[] tmpTop;
    for (int i = 0; i < M; i++) delete[] tmpBoard[i];
    delete[] tmpBoard;

    return score;
}

int UCT::getScore(int _x, int _y, int* _top, int** _board, bool player) {
    if (_x < 0 || _y < 0) return 5;
    if (player && userWin(_x, _y, M, N, _board)) return -1;
    if (!player && machineWin(_x, _y, M, N, _board)) return 1;
    if (isTie(N, top)) return 0;
    return 10;
}

void UCT::backup(Node* v, double status) {
    // std::cerr << "[UCT::backup]\n";
    while (v) {
        v->countVisited ++;
        v->countWin += status;
        status = 1 - status;
        v = v->parent;
    }
}

UCT::~UCT() {
    // std::cerr << "[UCT::~UCT]\n";
    delete[] top;
    for (int i = 0; i < M; i++) delete board[i];
    delete[] board;
}