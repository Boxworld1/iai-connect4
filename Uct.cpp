#include "Uct.h"

UCT::UCT() {}

UCT::UCT(int _M, int _N, int _noX, int _noY, const int* _top, int** _board):
    M(_M), N(_N), noX(_noX), noY(_noY) {

    // std::cerr << "[UCT::UCT] init\n";
    // 初始化
    root = nullptr;
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

    // std::cerr << "[UCT::UCT] init finished\n";

}

Point UCT::uctSearch() {
    // std::cerr << "[UCT::uctSearch] search started\n";
    root = new Node(nullptr, M, N, noX, noY, -1, -1, top, board, true);
    while (timer.get() < TIME_LIMIT) {
        // std::cerr << "[UCT::uctSearch] check next node\n";
        // std::cerr << "[UCT::uctSearch] check time: " << timer.get() << "\n";
        Node* vl = treePolicy(root);
        int delta = defaultPolicy(vl);
        backup(vl, delta);
    }
    return root->bestChild()->getMove();
}

Node* UCT::treePolicy(Node* v) {
    // std::cerr << "[UCT::treePolicy]\n";
    while (!v->end()) {
        if (v->canExpend()) {
            return v->expand();
        } else {
            v = v->bestChild();
        }
    }
    return v;
}

int UCT::defaultPolicy(Node* v) {
    // std::cerr << "[UCT::defaultPolicy]\n";
    Point pt = v->getMove();

    int x = pt.x;
    int y = pt.y;

    int* _top = v->getTop();
    int** _board = v->getBoard();

    int* tmpTop = new int[N];
    for (int i = 0; i < N; i++) {
        tmpTop[i] = _top[i];
    }

    int** tmpBoard = new int*[M];
    for (int i = 0; i < M; i++) {
        tmpBoard[i] = new int[N];
        for (int j = 0; j < N; j++) {
            tmpBoard[i][j] = _board[i][j];
        }
    }

    // std::cerr << "[UCT::defaultPolicy] copy finished\n";

    bool player = v->getPlayer();
    int score = getScore(x, y, tmpTop, tmpBoard, player);

    // std::cerr << "[UCT::defaultPolicy] simulation\n";
    while (score > 1) {
        // 回合切换
        player = !player;

        // 找出下子点
        srand(timer.get());
        int idx = rand() % N;
        while (!top[idx]) {
            idx = rand() % N;
        }

        // 下棋
        top[idx]--;
        int x = top[idx], y = idx;
        tmpBoard[x][y] = (player? 1: 2);

        // 若下棋位置的再下一位是不可下棋点, 则跳过
        if (x - 1 == noX && y == noY) {
            top[y]--;
        }

        score = getScore(pt.x, pt.y, tmpTop, tmpBoard, player);
    }

    delete[] tmpTop;
    for (int i = 0; i < M; i++) delete[] tmpBoard[i];
    delete[] tmpBoard;

    return score;
}

int UCT::getScore(int _x, int _y, int* _top, int** _board, bool player) {
    // std::cerr << "[UCT::getScore]\n";
    if (_x < 0 || _y < 0) return 5;
    if (player && userWin(_x, _y, M, N, _board)) return -1;
    if (!player && machineWin(_x, _y, M, N, _board)) return 1;
    if (isTie(N, top)) return 0;
    return 10;
}

void UCT::backup(Node* v, int status) {
    // std::cerr << "[UCT::backup]\n";
    while (v) {
        v->countVisited ++;
        v->countWin += status;
        status = 1 - status;
        v = v->parent;
    }
}

UCT::~UCT() {
    delete[] top;
    for (int i = 0; i < M; i++) delete board[i];
    delete[] board;
}