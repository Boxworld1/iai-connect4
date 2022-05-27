#include "Node.h"

Node::Node() {
    timer.set();
}

Node::Node(Node* _parent, int _M, int _N, int _noX, int _noY, int _posX, int _posY, int* _top, int** _board, bool _player): 
    parent(_parent), M(_M), N(_N), noX(_noX), noY(_noY), posX(_posX), posY(_posY), player(_player) {
    
    // std::cerr << "[Node::Node] init\n";
    // 初始化
    canMove.clear();
    countCanMove = 0;
    countVisited = 0;
    countWin = 0;
    timer.set();

    // 记录每一列的可下子位置
    top = new int[N];
    for (int i = 0; i < N; i++) {
        top[i] = _top[i];
        if (top[i]) {
            countCanMove++;
            canMove.push_back(i);
        }
    }

    // 记录当前棋盘信息
    board = new int*[M];
    for (int i = 0; i < M; i++) {
        board[i] = new int[N];
        for (int j = 0; j < N; j++) {
            board[i][j] = _board[i][j];
        }
    }

    // 记录子节点
    child = new Node*[N];
    // std::cerr << "[Node::Node] init finished\n";
}

Node* Node::bestChild() {
    // std::cerr << "[Node::bestChild]\n";
    double tmpScore = -1e20;
    Node* tmpNode = nullptr;
    for (int i = 0; i < N; i++) {
        if (child[i] == nullptr) continue;
        int win = child[i]->countWin;
        int vis = child[i]->countVisited;
        double score = (win * 1.0 / vis + c * sqrt(2.0 * log(countVisited) / vis));
        if (score > tmpScore) {
            tmpScore = score;
            tmpNode = child[i];
        }
    }
    return tmpNode;
}

bool Node::canExpend() {
    return countCanMove > 0;
}

Node* Node::expand() {
    // std::cerr << "[Node::expand]\n";
    // 随机选择要下的列
    srand(timer.get());
    int idx = rand() % countCanMove;
    // std::cerr << "[Node::expand] target idx: " << idx << "\n";
    
    // 复制当前信息
    int* tmpTop = new int[N];
    for (int i = 0; i < N; i++) {
        tmpTop[i] = top[i];
    }

    int** tmpBoard = new int*[M];
    for (int i = 0; i < M; i++) {
        tmpBoard[i] = new int[N];
        for (int j = 0; j < N; j++) {
            tmpBoard[i][j] = board[i][j];
        }
    }
    
    // std::cerr << "[Node::expand] copy info finished\n";

    // 对应位置下棋
    int nxtY = canMove[idx];
    tmpTop[nxtY]--;
    int nxtX = tmpTop[nxtY];
    tmpBoard[nxtX][nxtY] = (player? 1: 2);

    // std::cerr << "[Node::expand] Node move " << nxtX << " " << nxtY << " " << tmpBoard[nxtX][nxtY] << "\n";

    // 若下棋位置的再下一位是不可下棋点, 则跳过
    if (nxtX - 1 == noX && nxtY == noY) {
        tmpTop[nxtY]--;
    }

    // 记录此状态
    Node* tmp = new Node(this, M, N, noX, noY, nxtX, nxtY, tmpTop, tmpBoard, !player);
    // std::cerr << "[Node::expand] canMove size: " << canMove.size() << "\n";
    canMove.erase(canMove.begin() + idx);
    countCanMove--;
    // std::cerr << "[Node::expand] erase idx finished: " << idx << "\n";

    child[nxtY] = tmp;
    // std::cerr << "[Node::expand] Node expend finished\n";
    delete[] tmpTop;
    for (int i = 0; i < M; i++) delete[] tmpBoard[i];
    delete[] tmpBoard;

    return tmp;
}

bool Node::end() {
    // std::cerr << "[Node::end] Node status check\n";
    // 若尚未下棋
    if (posX == -1 && posY == -1) {
        return false;
    }

    // 否则若到达游戏终止条件
    if ((player && userWin(posX, posY, M, N, board)) ||
        (!player && machineWin(posX, posY, M, N, board)) ||
        (isTie(N, top))) {
        return true;
    }
    // std::cerr << "[Node::end] status: game continue\n";
    return false;
}

bool Node::getPlayer() {
    return player;
}

Point Node::getMove() {
    return Point(posX, posY);
}

int* Node::getTop() {
    return top;
}

int** Node::getBoard() {
    return board;
}

void Node::clearArray() {
    delete[] top;
    for (int i = 0; i < M; i++) {
        delete[] board[i];
    }
    delete[] board;
    for (int i = 0; i < N; i++) {
        if (child[i]) {
            child[i]->clearArray();
        }
    }
    delete[] child;
    canMove.clear();
}

Node::~Node() {
    clearArray();
}