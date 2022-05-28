#include "Node.h"
#include "Uct.h"

Node::Node() {
    timer.set();
}

Node::Node(Node* _parent, int _M, int _N, int _noX, int _noY, int _posX, int _posY, bool _player): 
    parent(_parent), M(_M), N(_N), noX(_noX), noY(_noY), posX(_posX), posY(_posY), player(_player) {
    
    // std::cerr << "[Node::Node] init\n";
    // 初始化
    canMove.clear();
    countCanMove = 0;
    countVisited = 0;
    countWin = 0;
    timer.set();

    // 记录可下子的列编号
    for (int i = 0; i < N; i++) {
        if (UCT::curTop[i]) {
            countCanMove++;
            canMove.push_back(i);
        }
    }

    // 记录子节点
    child = new Node*[N];
    for (int i = 0; i < N; i++) {
        child[i] = nullptr;
    }
    // std::cerr << "[Node::Node] init finished\n";
}

Node* Node::bestChild(bool move) {
    // std::cerr << "[Node::bestChild]\n";
    double tmpScore = -1e20;
    Node* tmpNode = nullptr;
    int y = -1;
    // 遍历所有列
    for (int i = 0; i < N; i++) {
        if (child[i] == nullptr) continue;
        int win = child[i]->countWin;
        int vis = child[i]->countVisited;
        double score = (win * 1.0 / vis + c * sqrt(2.0 * log(countVisited) / vis));
        if (score > tmpScore) {
            tmpScore = score;
            tmpNode = child[i];
            y = i;
        }
    }
    // 选好位置后下棋
    if (move) {
        int x = --UCT::curTop[y];
        UCT::curBoard[x][y] = ((!player)? 1: 2);
        // 若下棋位置的再下一位是不可下棋点, 则跳过
        if (x - 1 == noX && y == noY) {
            UCT::curTop[y]--;
        }
    }
    return tmpNode;
}

bool Node::canExpend() {
    return countCanMove > 0;
}

Node* Node::expand() {
    // std::cerr << "[Node::expand]\n";

    if (!checkStat) {
        checkStat = true;
        // std::cerr << "[Node::expand] CheckStat \n";
        // 优先选择必胜节点 (进攻)
        for (int i = 0; i < countCanMove; i++) {
            int idx = canMove[i];
            if (checkWin(idx, player)) {
                countCanMove = 0;
                // std::cerr << "[Node::expand] Win Pos: " << idx << "\n";
                return saveStatus(idx);
            }
        }
        
        // 否则选择必败节点 (防守)
        for (int i = 0; i < countCanMove; i++) {
            int idx = canMove[i];
            if (checkWin(idx, !player)) {
                countCanMove = 0;
                // std::cerr << "[Node::expand] Lose Pos: " << idx << "\n";
                return saveStatus(idx);
            }
        }
    }

    // 最后随机选择要下的列
    srand(timer.get());
    int idx = rand() % countCanMove;
    return saveStatus(idx);
}

bool Node::checkWin(int idx, bool _player) {
    int tmpY = canMove[idx];
    int tmpX = UCT::curTop[tmpY] - 1;
    UCT::curBoard[tmpX][tmpY] = (_player? 1: 2);
    // 检测 AI 是否胜利
    if (!_player && machineWin(tmpX, tmpY, M, N, UCT::curBoard)) {
        return true;
    }
    // 或者检测用户是否胜利
    if (_player && userWin(tmpX, tmpY, M, N, UCT::curBoard)) {
        return true;
    }
    UCT::curBoard[tmpX][tmpY] = 0;
    return false;
}

Node* Node::saveStatus(int idx) {
    // 对应位置下棋
    int nxtY = canMove[idx];
    int nxtX = --UCT::curTop[nxtY];
    UCT::curBoard[nxtX][nxtY] = ((!player)? 1: 2);

    // 若下棋位置的再下一位是不可下棋点, 则跳过
    if (nxtX - 1 == noX && nxtY == noY) {
        UCT::curTop[nxtY]--;
    }

    // 记录此状态
    Node* tmp = new Node(this, M, N, noX, noY, nxtX, nxtY, !player);
    canMove.erase(canMove.begin() + idx);
    countCanMove--;
    child[nxtY] = tmp;

    // std::cerr << "[Node::expand] Node expend finished\n";
    return tmp;
}

bool Node::end() {
    // std::cerr << "[Node::end] Node status check\n";
    // 若尚未下棋
    if (posX == -1 && posY == -1) {
        return false;
    }

    // 否则若到达游戏终止条件
    if ((player && userWin(posX, posY, M, N, UCT::curBoard)) ||
        (!player && machineWin(posX, posY, M, N, UCT::curBoard)) ||
        (isTie(N, UCT::curTop))) {
        return true;
    }
    // std::cerr << "[Node::end] status: game continue\n";
    return false;
}

Point Node::getMove() {
    return Point(posX, posY);
}

void Node::clearChild() {
    // std::cerr << "[Node::clearChild]\n";
    if (!child) return; 
    // 清除子节点
    for (int i = 0; i < N; i++) {
        if (child[i]) {
            child[i]->clearChild();
        }
    }
    delete[] child;
    canMove.clear();
}

Node::~Node() {}