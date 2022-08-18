
#ifndef __STDIO_H__
#define __STDIO_H__
#include <stdio.h>
#endif // __STDIO_H__

#ifndef __WINDOWS_H__
#define __WINDOWS_H__
#include <windows.h>
#endif // __WINDOWS_H__

#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <stdlib.h>
#endif

#ifndef _MATH_H_
#define _MATH_H_
#include <math.h>
#endif

#ifndef _CONIO_H_
#define _CONIO_H_
#include <conio.h>
#endif

#include <time.h>


#define ARROW_KEY 224
#define MAP_WIDTH 35// 맵의 가로 크기
#define MAP_LENGTH 35 // 맵의 세로 크기
// 오류가 날 수 있으니 둘의 크기를 갖게 할 것

// 방향키의 코드값
#define ARROW_UP 72
#define ARROW_LEFT 75
#define ARROW_RIGHT 77
#define ARROW_DOWN 80

#define MAX_MOVING_SPIKE 65

static int over_code; // 게임 오버 원인을 담는 변수
static int stats[5] = { 0 }; // 엔딩 크래딧용 통계
static int setevent[4] = { 0 }; // 이벤트 설정
static int event_type = 0;
/*인덱스     내용
    0        찔린 가시 개수
    1        먹은 하트 개수
    2        폭발한 횟수
    3        얻은 열쇠 개수
    4        이동한 칸 개수
*/
// 색깔을 저장하는 열거형
enum COLOR {
    // https://m.blog.naver.com/PostView.nhn?blogId=wcwtmt&logNo=10172917569&proxyReferer=https:%2F%2Fwww.google.com%2F
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    LIGHT_GRAY = 7,
    GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_YELLOW = 14,
    WHITE = 15
};
// 장애물, 블록들의 번호
enum BLOCK {
    AIR = 0,
    SPIKE = 1,
    GOAL_PORTAL = 2,
    HEART = 3,
    MOVING_SPIKE = 4,
    SUPER_SPIKE = 5,
    KEY = 6,
    BOMB = 7,
    FAKE_PORTAL = 8,
    EXPLOSION = 9
};
// 커서 이동하는 함수
void gotoxy(int x, int y) {
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// https://m.blog.naver.com/PostView.nhn?blogId=upssuyo&logNo=80092210109&proxyReferer=https:%2F%2Fwww.google.com%2F
// 커서를 숨기고 나타내는 함수 0은 숨김, 1은 보임
void CursorShow(char show) {
    HANDLE h;
    CONSOLE_CURSOR_INFO cursor;
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    cursor.bVisible = show;
    cursor.dwSize = 1;
    SetConsoleCursorInfo(h, &cursor);
}
// 색을 지정하는 함수, int에 COLOR 열거형을 넣는다.
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
// 이벤트가 일어날 단계 지정
void event() {
    int l[18] = { 0 };
    int i, r;
    l[0] = 15;
    for (i = 1; i < 18; i++) {
        l[i] = l[i - 1] + 1;
    }
    for (i = 0; i < 4; i++) {
        do {
            r = rand() % 18;
        } while (l[r] == 0);
        setevent[i] = l[r];
        l[r] = 0;
    }
}
// 플레이어의 위치를 다루는 구조체
typedef struct Position {
    int x;
    int y;
}Position;
typedef struct Moving_Spike {
    Position pos;
}Moving_Spike;
typedef struct Bomb {
    Position pos;
    int left_time;
}Bomb;
// 맵을 저장하는 구조체
typedef struct World {
    int map[MAP_WIDTH][MAP_LENGTH];
    int level;
    Moving_Spike spike_list[MAX_MOVING_SPIKE];
    int spike_list_count;
    Bomb bomb_list[MAX_MOVING_SPIKE];
    int bomb_list_count;
    int left_key;
}World;


// 플레이어 구조체
typedef struct Player {
    Position pos;
    int health;
}Player;

void erase_moving_spike(World* w, int x, int y) {
    for (int i = 0; i < w->spike_list_count; i++) {
        if (w->spike_list[i].pos.x == x && w->spike_list[i].pos.y == y) {
            for (int j = i; j < w->spike_list_count - 1; j++) {
                w->spike_list[j] = w->spike_list[j + 1];
            }
        }
    }
    w->spike_list_count -= 1;
}

// 데이터 값에 따라 출력을 달리함
void print_block(int blockdata, World* w, int x, int y) {
    enum COLOR c;
    int i;
    switch (blockdata) {
    case AIR:
        c = BLACK;
        setColor(c);
        printf("■");
        break;
    case SPIKE:
        c = LIGHT_YELLOW;
        setColor(c);
        printf("△");
        break;
    case GOAL_PORTAL:
        if (w->left_key > 0) {
            c = WHITE;
        }
        else {
            c = GREEN;
        }
        setColor(c);
        printf("◆");
        break;
    case HEART:
        c = LIGHT_RED;
        setColor(c);
        printf("♥");
        break;
    case MOVING_SPIKE:
        c = MAGENTA;
        setColor(c);
        printf("◈");
        break;
    case SUPER_SPIKE:
        c = RED;
        setColor(c);
        printf("▲");
        break;
    case KEY:
        c = LIGHT_GREEN;
        setColor(c);
        printf("♧");
        break;
    case BOMB:
        for (i = 0; i < w->bomb_list_count; i++) {
            if (w->bomb_list[i].pos.x == x && w->bomb_list[i].pos.y == y) {
                if (w->bomb_list[i].left_time < 4) {
                    c = RED;
                }
                else if (w->bomb_list[i].left_time < 9) {
                    c = LIGHT_RED;
                }
                else {
                    c = LIGHT_YELLOW;
                }
            }
        }
        setColor(c);
        printf("▩");
        break;
    case EXPLOSION:
        c = LIGHT_RED;
        setColor(c);
        printf("▒ ");
        break;
    }
    c = WHITE;
    setColor(c);
}

// 플레이어의 위치와 체력을 동시 설정
void playerSet(Player* p, int x, int y, int health) {
    p->pos.x = x;
    p->pos.y = y;
    p->health = health;
}
// 단계가 넘어갈 때마다 맵 배치
void setting(World* world, Player* p) {
    world->spike_list_count = 0;
    world->bomb_list_count = 0;
    int i, j, x, y;
    int key_count;
    int moving_spike_ratio, bomb_ratio;
    event_type = 0;
    if (setevent[0] == world->level || setevent[1] == world->level) {
        moving_spike_ratio = 765;
        event_type = 1;
    }
    else {
        moving_spike_ratio = 945 - (world->level / 3) * 2;
    }
    if (setevent[2] == world->level || setevent[3] == world->level) {
        bomb_ratio = 780;
        event_type = 2;
    }
    else {
        bomb_ratio = 975 - (world->level / 3) * 3;
    }

    // 가시의 비율 지정
    int ratio, sratio; // sratio는 슈퍼 가시의 비율
    ratio = (world->level > 35) ? 300 : 20 + 8 * world->level;
    sratio = (world->level > 50) ? 700 : (ratio + world->level) * 2;
    // 가시, 슈퍼 가시, 움직이는 가시와 하트 설치
    for (i = 1; i < MAP_LENGTH - 1; i++) {
        for (j = 1; j < MAP_WIDTH - 1; j++) {
            if (rand() % 1000 < ratio && (abs(p->pos.x - i) > 2 || abs(p->pos.y - j) > 2)) {
                if (world->level > 7 && rand() % 1000 < sratio) {
                    world->map[j][i] = SUPER_SPIKE;
                }
                else if (world->level > 12 && (rand() % 1000) > moving_spike_ratio && world->spike_list_count < MAX_MOVING_SPIKE) {
                    world->map[j][i] = MOVING_SPIKE;
                    Moving_Spike m;
                    m.pos.x = j, m.pos.y = i;
                    world->spike_list[world->spike_list_count] = m;
                    world->spike_list_count++;
                }
                else if (world->level > 15 && (rand() % 1000) > bomb_ratio && world->bomb_list_count < MAX_MOVING_SPIKE/*오타가 아니라 맞음*/) {
                    world->map[j][i] = BOMB;
                    Bomb b;
                    b.pos.x = j, b.pos.y = i;
                    b.left_time = rand() % 20 + 12;
                    world->bomb_list[world->bomb_list_count] = b;
                    world->bomb_list_count++;
                }
                else {
                    world->map[j][i] = SPIKE;
                }
                //난이도= 하드(3497) 보통 (3495) 이지 (3490~3493)
            }
            else if (world->level > 3 && rand() % 3500 > 3496) {
                world->map[j][i] = HEART;
            }
            else {
                world->map[j][i] = AIR;
            }
        }
    }

    // 테두리에 모두 가시로 도배
    for (i = 0; i < MAP_WIDTH; i++) {
        if (world->level > 7) {
            world->map[i][0] = SUPER_SPIKE;
            world->map[i][MAP_LENGTH - 1] = SUPER_SPIKE;
        }
        else {
            world->map[i][0] = SPIKE;
            world->map[i][MAP_LENGTH - 1] = SPIKE;
        }
    }
    for (i = 0; i < MAP_LENGTH; i++) {
        if (world->level > 7) {
            world->map[0][i] = SUPER_SPIKE;
            world->map[MAP_WIDTH - 1][i] = SUPER_SPIKE;
        }
        else {
            world->map[0][i] = SPIKE;
            world->map[MAP_WIDTH - 1][i] = SPIKE;
        }
    }
    // 포탈을 설치하고 주변 8칸은 비워버림
    int portal_x = MAP_WIDTH - 3 - (rand() % 3);
    int portal_y = MAP_LENGTH - 3 - (rand() % 3);
    for (int i = portal_x - 1; i <= portal_x + 1; i++) {
        for (int j = portal_y - 1; j <= portal_y + 1; j++) {
            world->map[i][j] = AIR;
        }
    }
    world->map[portal_x][portal_y] = GOAL_PORTAL;
    // 7의 배수 단계에 열쇠 배치
    if (world->level % 7 == 0) {
        key_count = rand() % 3 + 5;
    }
    else {
        key_count = 0;
    }
    int set_key_count = 0;
    while (set_key_count < key_count) {
        x = rand() % (MAP_WIDTH - 1) + 1;
        y = rand() % (MAP_LENGTH - 1) + 1;
        if (world->map[x][y] == AIR) {
            world->map[x][y] = KEY;
            set_key_count++;
        }
    }
    world->left_key = key_count;

}
// 플레이어가 있는 곳 기준 24 * 24의 맵을 출력
void printworld(World* world, Position pos) {
    system("cls");
    //https://4343282.tistory.com/198
    int i, j;
    int left, up, right, down;
    if (pos.x - 12 < 0)
        left = 0;
    else
        left = pos.x - 12;
    if (pos.x + 12 >= MAP_WIDTH)
        right = MAP_WIDTH - 1;
    else
        right = left + 24;
    if (pos.y - 12 < 0)
        up = 0;
    else
        up = pos.y - 12;
    if (pos.y + 12 >= MAP_LENGTH)
        down = MAP_LENGTH - 1;
    else
        down = up + 24;
    for (i = up; i <= down; i++) {
        for (j = left; j <= right; j++) {
            if (i != pos.y || j != pos.x)
                print_block(world->map[j][i], world, j, i);
            else
                printf("★"); // 주인공을 출력함
        }
        printf("\n");
    }
}

// 방향키를 입력받은 뒤 그에 따라 움직인다.
int keyControl(Player* p) {
    //https://m.blog.naver.com/PostView.nhn?blogId=demonic3540&logNo=221206347973&proxyReferer=https:%2F%2Fwww.google.com%2F
    char c;
    while (!_kbhit());
    c = _getch();
    if (c == -32) {
        c = _getch();
        switch (c) {
        case ARROW_UP:
            if (p->pos.y > 0) {// if문 들은 벽에 있는지 검사해서 움직임 여부를 결정한다.
                p->pos.y -= 1;
                stats[4]++;
            }
            break;
        case ARROW_DOWN:
            if (p->pos.y < MAP_LENGTH - 1) {
                p->pos.y += 1;
                stats[4]++;
            }
            break;
        case ARROW_RIGHT:
            if (p->pos.x < MAP_WIDTH - 1) {
                p->pos.x += 1;
                stats[4]++;
            }
            break;
        case ARROW_LEFT:
            if (p->pos.x > 0) {
                p->pos.x -= 1;
                stats[4]++;
            }
            break;
        }
    }
    return 0;
}
// 움직이는 가시의 위치를 이동
void moving(World* w) {
    int i, direction;
    int x, y; // 가시의 x,y좌표를 담는 변수
    // 주변 상하좌우에 장애물이 없으면 이동함
    // 0 :↑,1 : ↓,2 : →,3 : ←
    for (i = 0; i < w->spike_list_count; i++) {
        x = w->spike_list[i].pos.x;
        y = w->spike_list[i].pos.y;
        direction = rand() % 4;
        switch (direction) {
        case 0:
            if (y > 0)
                if (w->map[x][y - 1] == AIR) {
                    w->spike_list[i].pos.y -= 1;
                    w->map[x][y - 1] = MOVING_SPIKE;
                    w->map[x][y] = AIR;

                }
            break;
        case 1:
            if (y < MAP_LENGTH - 1)
                if (w->map[x][y + 1] == AIR) {
                    w->spike_list[i].pos.y += 1;
                    w->map[x][y + 1] = MOVING_SPIKE;
                    w->map[x][y] = AIR;

                }
            break;
        case 2:
            if (x < MAP_WIDTH - 1)
                if (w->map[x + 1][y] == AIR) {
                    w->spike_list[i].pos.x += 1;
                    w->map[x + 1][y] = MOVING_SPIKE;
                    w->map[x][y] = AIR;
                }
            break;
        case 3:
            if (x > 0)
                if (w->map[x - 1][y] == AIR) {
                    w->spike_list[i].pos.x -= 1;
                    w->map[x - 1][y] = MOVING_SPIKE;
                    w->map[x][y] = AIR;
                }
            break;

        }
    }
}
// 맵에 있는 모든 폭탄의 남은 시간을 계산하고 0이 된 경우 폭파시키는 함수
void count_bomb(World* w) {
    int x, y; // 폭탄의의 x,y좌표를 담는 변수
    int x1, x2, y1, y2; // 폭탄의 폭발 범위
    /*
    x1          x2
    ▒▒▒▒▒▒▒
    ▒▒▒▒▒▒▒y1
    ▒▒▒▒▒▒▒
    ▒▒▒▩▒▒▒
    ▒▒▒▒▒▒▒
    ▒▒▒▒▒▒▒
    ▒▒▒▒▒▒▒y2
    */
    int i, j, k; // 반복용 변수
    for (i = 0; i < w->bomb_list_count; i++) {
        x = w->bomb_list[i].pos.x;
        y = w->bomb_list[i].pos.y;
        w->bomb_list[i].left_time -= 1;
        if (w->bomb_list[i].left_time <= 0) {
            // 벽 인근에서 폭파할 경우 폭발 범위를 조정함
            if (x == 0 || x == 1 || x == 2) {
                x1 = 0;
            }
            else {
                x1 = x - 3;
            }
            if (x == MAP_WIDTH - 1 || x == MAP_WIDTH - 2 || x == MAP_WIDTH - 3) {
                x2 = MAP_WIDTH - 1;
            }
            else {
                x2 = x + 3;
            }
            if (y == 0 || y == 1 || y == 2) {
                y1 = 0;
            }
            else {
                y1 = y - 3;
            }
            if (y == MAP_LENGTH - 1 || y == MAP_LENGTH - 2 || y == MAP_LENGTH - 3) {
                y2 = MAP_LENGTH - 1;
            }
            else {
                y2 = y + 3;
            }
            // 폭탄 위치도 폭발 입자로 덮어야 함 -> 단, 가시나 열쇠는 터지지 않아야 함
            w->map[x][y] = AIR;
            for (j = y1; j <= y2; j++) {
                for (k = x1; k <= x2; k++) {
                    if (w->map[k][j] == AIR || w->map[k][j] == HEART) {
                        w->map[k][j] = EXPLOSION;
                    }
                }
            }
            // 터진 폭탄은 리스트에서 제외시킴
            for (int j = i; j < w->bomb_list_count - 1; j++) {
                w->bomb_list[j] = w->bomb_list[j + 1];
            }
            w->bomb_list_count -= 1;
        }
    }
}

// 폭발 입자를 1/16 확률로 없앰
void remove_explosion(World* w) {
    int i, j;
    for (i = 0; i < MAP_LENGTH - 1; i++) {
        for (j = 0; j < MAP_WIDTH - 1; j++) {
            if (w->map[j][i] == EXPLOSION && rand() % 16 == 0) {
                w->map[j][i] = AIR;
            }
        }
    }
}
// 플레이어가 맵의 물체와 충돌했는지 검사
void checkCollision(Player* p, World* w) {
    // 가시는 체력을 감소 시키고 공기로 바꿈
    if (w->map[p->pos.x][p->pos.y] == SPIKE) {
        p->health -= 1;
        over_code = SPIKE;
        stats[0]++;
        w->map[p->pos.x][p->pos.y] = AIR;
    }
    if (w->map[p->pos.x][p->pos.y] == SUPER_SPIKE) {
        p->health -= 3;
        over_code = SUPER_SPIKE;
        stats[0]++;
        w->map[p->pos.x][p->pos.y] = AIR;
    }
    if (w->map[p->pos.x][p->pos.y] == MOVING_SPIKE) {
        p->health -= 1;
        over_code = MOVING_SPIKE;
        stats[0]++;
        erase_moving_spike(w, p->pos.x, p->pos.y);
        w->map[p->pos.x][p->pos.y] = AIR;
    }
    // 포탈은 다음 단계로 이동시킴 -> 남은 키의 개수가 0개여야 함
    if (w->map[p->pos.x][p->pos.y] == GOAL_PORTAL) {
        if (w->left_key <= 0) {
            playerSet(p, rand() % 5 + 2, rand() % 5 + 2, p->health);
            w->level += 1;
            setting(w, p);
        }
    }
    // 하트는 체력을 증가 시키고 공기로 바꿈
    if (w->map[p->pos.x][p->pos.y] == HEART) {
        if (p->health < 15) {
            p->health += 1;
        }
        stats[1]++;
        w->map[p->pos.x][p->pos.y] = AIR;
    }
    // 열쇠는 남은 개수를 줄이고 공기로 바꿈
    if (w->map[p->pos.x][p->pos.y] == KEY) {
        w->left_key -= 1;
        stats[3]++;
        w->map[p->pos.x][p->pos.y] = AIR;
    }
    // 폭발 잔해는 가시와 동일 처리
    if (w->map[p->pos.x][p->pos.y] == EXPLOSION) {
        p->health -= 3;
        over_code = EXPLOSION;
        stats[2]++;
        w->map[p->pos.x][p->pos.y] = AIR;
    }
}

// 위치, 체력, 레벨 정보를 출력
void printInformation(const Player* p, const World* w, double times) {
    gotoxy(70, 10);
    printf("레벨 : %d", w->level);
    gotoxy(70, 11);
    printf("위치 : (%d, %d)", p->pos.x, p->pos.y);
    gotoxy(70, 12);
    printf("남은 체력 : %d/15", p->health);
    gotoxy(70, 13);
    printf("경과 시간 : %.2f", times);
    if (w->left_key > 0) {
        gotoxy(70, 15);
        printf("남은 키 : %d개", w->left_key);
    }
    if (event_type == 1) {
        gotoxy(70, 6);
        printf("경고 : 움직이는 가시의 개수가 많습니다.");
    }
    else if (event_type == 2) {
        gotoxy(70, 6);
        printf("경고 : 폭탄의 개수가 많습니다.");
    }
    if (w->level == 4) {
        gotoxy(70, 17);
        setColor(LIGHT_RED);
        printf("♥ 하트\n");
        gotoxy(70, 18);
        printf("플레이어의 체력을 1 회복시킵니다.");
    }
    else if (w->level == 7) {
        gotoxy(70, 17);
        setColor(LIGHT_GREEN);
        printf("♧ 열쇠(키)\n");
        gotoxy(70, 18);
        printf("7의 배수 단계에서 등장하며 맵의 모든 키를");
        gotoxy(70, 19);
        printf("다 모아야 포탈이 활성화되어 ");
        gotoxy(70, 20);
        printf("다음 단계로 갈 수 있습니다.");
    }
    else if (w->level == 8) {
        gotoxy(70, 17);
        setColor(RED);
        printf("▲ 슈퍼 가시\n");
        gotoxy(70, 18);
        printf("닿으면 체력을 3 감소시킵니다.");
    }
    else if (w->level == 13) {
        gotoxy(70, 17);
        setColor(MAGENTA);
        printf("◈ 움직이는 가시\n");
        gotoxy(70, 18);
        printf("플레이어가 움직일 때 마다 상하좌우 중");
        gotoxy(70, 19);
        printf("빈 곳으로 1칸 이동하며 닿으면");
        gotoxy(70, 20);
        printf("체력을 1 감소시킵니다.");
    }
    else if (w->level == 16) {
        gotoxy(70, 17);
        setColor(LIGHT_YELLOW);
        printf("▩ 폭탄\n");
        gotoxy(70, 18);
        printf("플레이어가 움직일 때 마다 카운트가 감소하고");
        gotoxy(70, 19);
        printf("카운트가 감소할 수록 색깔이");
        gotoxy(70, 20);
        printf("노란색 -> 빨강색 -> 진한 빨강색으로 변하며");
        gotoxy(70, 21);
        printf("카운트가 다되면 폭발하며 7 x 7 크기의");
        gotoxy(70, 22);
        printf("빈 공간에 폭발 잔해를 남깁니다.");
        gotoxy(70, 23);
        printf("폭발 잔해에 닿으면 체력을 3 감소시킵니다.");
    }
    setColor(WHITE);

}
// 게임 오버시 원인을 출력하는 함수
void show_over_message(int over_code) {
    switch (over_code) {
    case SPIKE:
        printf("가시에 찔려서 ");
        break;
    case SUPER_SPIKE:
        printf("슈퍼 가시에 찔려서 ");
        break;
    case MOVING_SPIKE:
        printf("움직이는 가시에 닿아서 ");
        break;
    case EXPLOSION:
        printf("폭탄의 폭발의 휘말려서 ");
        break;
    }
    int back_text = rand() % 3;
    switch (back_text) {
    case 0:
        printf("더 이상 나아갈 수 없게 되었습니다.\n");
        break;
    case 1:
        printf("체력을 다 소진해 버렸습니다.\n");
        break;
    case 2:
        printf("구조대를 기다릴 수 밖에 없게 되었습니다.\n");
        break;
    }

}
void ending_message(double duration) {
    gotoxy(30, 5);
    printf("걸린 시간 : %.2f초", duration);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("방해한 가시 개수 : %d", stats[0]);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("도와준 하트 개수 : %d", stats[1]);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("폭발에 휩싸인 횟수 : %d", stats[2]);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("얻은 열쇠 개수 : %d", stats[3]);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("이동한 거리 : %dm", stats[4]);
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("만든 사람 : 임재민");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("Special thanks");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("황성연 김승욱 이성민");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("김도윤 2020년도 컴퓨터부 학생들");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("그리고");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("지금 플레이해주신 당신");
    Sleep(2000);
    system("cls");
    gotoxy(30, 5);
    printf("플레이 해주셔서 감사합니다!");
    Sleep(2000);
    system("cls");
}
int main()
{
    // 시간 : https://blog.naver.com/dragun62/220307104305
    char clear = 0;
    event();
    clock_t start, now;
    double duration;
    srand(time(NULL));
    World w;
    World* world = &w;
    w.level = 1;
    Player p;
    Player* player = &p;
    playerSet(player, 5, 5, 3);
    setting(world, player);
    printf("기본 설명\n\n");
    printf("키보드 방향키로 움직입니다.\n");
    printf("맵에 놓여 있는 장애물들을 피해서 초록색 포탈에 닿으면 다음으로 이동합니다.\n");
    printf("목표는 35단계 클리어입니다.\n");
    printf("==========================================\n\n");
    printf("오브젝트 설명\n");
    setColor(WHITE);
    printf("★ 플레이어\n");
    printf("이 게임에서 당신이 움직일 캐릭터 입니다. 체력이 0이 되면 게임오버 입니다.\n\n");
    setColor(LIGHT_YELLOW);
    printf("△ 가시\n");
    printf("맵에 놓여진 장애물로 닿으면 체력이 1 감소합니다.\n\n");
    setColor(GREEN);
    printf("◆ 포탈\n");
    printf("다음 단계로 이동합니다. 맵의 오른쪽 아래에 있습니다.\n\n");
    setColor(WHITE);
    printf("나머지 오브젝트들은 그 오브젝트가 처음 나오는 단계에서 설명해드리겠습니다.\n");
    printf("방향키 4개 중 아무거나 눌러서 시작하세요...");
    start = clock();
    while (1) {
        CursorShow(0);
        now = clock();
        duration = (double)(now - start) / CLOCKS_PER_SEC;
        keyControl(player);
        checkCollision(player, world); // 움직이는 가시에 플레이어가 충돌하는 경우 움직이기 전에 가시를 없애기 위해 2번 실행
        count_bomb(world);
        moving(world);
        checkCollision(player, world);
        printworld(world, p.pos);
        remove_explosion(world);
        printInformation(player, world, duration);
        if (p.health <= 0) break;
        if (world->level > 35) {
            clear = 1;
            break;
        }
    }

    system("cls");
    if (clear) {
        gotoxy(30, 5);
        printf("축하합니다! 게임을 클리어했습니다!\n");
        Sleep(2000);
        system("cls");
        ending_message(duration);
    }
    else {
        printf("게임 오버\n");
        show_over_message(over_code);
        printf("최종 단계 : %d", w.level);
    }
    return 0;
}
