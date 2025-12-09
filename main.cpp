#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cctype>
#include <algorithm>    // thêm để dùng std::max

using namespace std;

#define H 20
#define W 15
#define VIEWPORT_HEIGHT 20     // ✅ cho bằng H để không bị “mất khối” khi rơi xuống dưới
const char BLOCK = char(219);

int speed = 200;
char board[H][W] = {};
int score = 0;

// Mảng các khối Tetris
char blocks[7][4][4][4] = {
    // 0: I (Line)
    {
        {{' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,BLOCK}, {' ',' ',' ',' '}, {' ',' ',' ',' '}},
        {{' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,BLOCK}, {' ',' ',' ',' '}, {' ',' ',' ',' '}}
    },
    // 1: O (Square)
    {
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}}
    },
    // 2: T
    {
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,' ',' '}}
    },
    // 3: S
    {
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {BLOCK,BLOCK,' ',' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {BLOCK,BLOCK,' ',' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}}
    },
    // 4: Z
    {
        {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}}
    },
    // 5: J
    {
        {{' ',' ',' ',' '}, {BLOCK,' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
        {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,' ',' '}}
    },
    // 6: L
    {
        {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
        {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}},
        {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {BLOCK,' ',' ',' '}},
        {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}}
    }
};

int x = 4, y = 0, b = 1;
int rotation = 0;
int nextBlock = -1;

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void initBoard(){
    for (int i = 0 ; i < H ; i++){
        for (int j = 0 ; j < W ; j++){
            if(i == H-1) {
                if(j==0) board[i][j] = char(200);
                else if(j==W-1) board[i][j] = char(188);
                else board[i][j] = char(205);
            }
           else if ((j == 0) || (j == W-1))
                board[i][j] = char(186);
            else
                board[i][j] = ' ';
        }
    }
}

// Xóa khối đang rơi khỏi board để vẽ lại ở vị trí mới
void boardDelBlock(){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (blocks[b][rotation][i][j] != ' '){
                int ty = y + i;
                int tx = x + j;
                if (ty >= 0 && ty < H && tx >= 1 && tx < W-1){
                    // Chỉ xóa ô này – các khối đã cố định ở vị trí khác sẽ không bị ảnh hưởng
                    board[ty][tx] = ' ';
                }
            }
        }
    }
}

void block2Board(){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (blocks[b][rotation][i][j] != ' '){
                int ty = y + i;
                int tx = x + j;
                if (ty >= 0 && ty < H && tx >= 1 && tx < W-1){
                    board[ty][tx] = BLOCK;
                }
            }
        }
    }
}

void draw(){
    // Xóa màn hình
    system("cls");

    // Vẽ phòng chơi (✅ giờ vẽ hết chiều cao H)
    for (int i = 0; i < VIEWPORT_HEIGHT && i < H; i++){
        for (int j = 0; j < W; j++){
            cout << board[i][j] << board[i][j];
        }
        cout << "\n";
    }

    // Hiển thị thông tin
    cout << "\n";
    cout << "Score: " << score << "        Next: ";
    char nextBlockName[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
    if (nextBlock >= 0 && nextBlock < 7) {
        cout << nextBlockName[nextBlock];
    }
    cout << "\n";

    cout << "Controls: A/D=Move  S=Down  W=Rotate  Q=Quit\n";

    cout.flush();
}

bool canMove(int dx, int dy){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (blocks[b][rotation][i][j] != ' '){
                int tx = x + j + dx;
                int ty = y + i + dy;

                // Kiểm tra biên
                if (tx < 1 || tx >= W-1 || ty >= H-1) return false;

                // Kiểm tra va chạm
if (ty >= 0 && (board[ty][tx] == '#' || board[ty][tx] == BLOCK))
                    return false;
            }
        }
    }
    return true;
}

bool canRotate(int newRotation) {
for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (blocks[b][newRotation][i][j] != ' '){
                int tx = x + j;
                int ty = y + i;

                if (tx < 1 || tx >= W-1 || ty >= H-1) return false;
                if (ty >= 0 && (board[ty][tx] == '#' || board[ty][tx] == BLOCK))
                    return false;
            }
        }
    }
    return true;
}

void rotateBlock() {
    int newRotation = (rotation + 1) % 4;
    if (canRotate(newRotation)) {
        rotation = newRotation;
    }
}

void removeLine(){
    for (int i = H - 2; i >= 1; i--){
        bool full = true;
        for (int j = 1; j < W - 1; j++){
            if (board[i][j] != BLOCK) {
                full = false;
                break;
            }
        }

        if (full) {
            // Xóa dòng và đẩy các dòng phía trên xuống
            for (int k = i; k > 0; k--){
                for (int j = 1; j < W - 1; j++){
                    board[k][j] = board[k - 1][j];
                }
            }
            // Dòng trên cùng (k = 0) nên clear
            for (int j = 1; j < W - 1; j++){
                board[0][j] = ' ';
            }

            score += 100;
            i++;  // Kiểm tra lại dòng này vì nó vừa thay đổi
        }
    }
}

int getBlockMaxCol(int blockIndex) {
    int maxCol = -1;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (blocks[blockIndex][0][i][j] != ' ') {
                maxCol = std::max(maxCol, j);
            }
        }
    }
    return maxCol + 1;
}

int getRandomX(int blockIndex) {
    int blockMaxCol = getBlockMaxCol(blockIndex);
    int maxX = W - 1 - blockMaxCol;
    if (maxX < 1) maxX = 1;
    return 1 + rand() % maxX;
}

bool isGameOver() {
    for (int j = 1; j < W - 1; j++) {
        if (board[0][j] == BLOCK) {
            return true;
        }
    }
    return false;
}

int main(){
    // Đặt code page để in được char(219)
    system("chcp 437 >nul");
    SetConsoleOutputCP(437);

    // Ẩn con trỏ
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursorInfo);

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    system("cls");

    srand((unsigned)time(0));

    // Khởi tạo game
    initBoard();
    b = rand() % 7;
    nextBlock = rand() % 7;
    rotation = 0;
    x = getRandomX(b);
    y = 0;

    int fallCounter = 0;
    bool gameOver = false;

    while (!gameOver){
        // Xóa khối hiện tại từ board
        boardDelBlock();

        // Xử lý input không blocking
        if (_kbhit()){
unsigned char ch = _getch();
            char c = tolower(ch);
            if (c == 'a') {
                if (canMove(-1, 0)) x--;
            }
            else if (c == 'd') {
                if (canMove(1, 0)) x++;
            }
            else if (c == 's') {
                if (canMove(0, 1)) y++;
            }
            else if (c == 'w') {
                rotateBlock();
            }
            else if (c == 'q') {
                gameOver = true;
                break;
            }
        }

        // Logic rơi tự động
        fallCounter++;
        if (fallCounter >= speed / 30) {
            if (canMove(0, 1)) {
                y++;
            }
            else {
                // Khối không thể rơi nữa -> cố định khối vào board
                block2Board();

                // Xóa dòng đầy
                removeLine();

                // Kiểm tra game over
                if (isGameOver()) {
                    draw();
                    cout << "\n========== GAME OVER ==========" << endl;
                    cout << "Final Score: " << score << endl;
                    cout << "================================" << endl;
                    cout << "Press any key to exit..." << endl;
                    _getch();
                    gameOver = true;
                    break;
                }

                // Sinh khối mới
                b = nextBlock;
                nextBlock = rand() % 7;
                rotation = 0;
                x = getRandomX(b);
                y = 0;

                // Nếu không đặt được khối mới → Game Over
                if (!canMove(0, 0)) {
                    draw();
                    cout << "\n========== GAME OVER ==========" << endl;
                    cout << "Final Score: " << score << endl;
                    cout << "================================" << endl;
                    cout << "Press any key to exit..." << endl;
                    _getch();
                    gameOver = true;
                    break;
                }

                fallCounter = 0;
                // tiếp tục vòng lặp, khối mới sẽ được vẽ ở cuối
            }
            fallCounter = 0;
        }

        // Vẽ khối hiện tại (tạm thời)
        block2Board();
        draw();

        Sleep(30);  // thời gian chờ nhỏ để game mượt hơn
    }

    return 0;
}
