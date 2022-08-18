#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>
// 플레이어 구조체
typedef struct Player {
    int coin; // 가지고 있는 돈
    int orange_coin[4]; // 주황 코인 개수
    int pink_coin[5]; // 분홍 코인 개수
    int red_coin[6]; // 빨강 코인 개수
    int purple_coin[7]; // 보라 코인 개수
    int expired_coin[4]; // 만료된 코인의 개수
    int selled_coin[4]; // 종류에 따라 판매한 코인의 총 개수
    int total_gain_coin; // 손실 제외 누적 이익
    char rank; // 랭크
}Player;
// 코인 매니저의 구조체
typedef struct CoinManager {
    // 각각의 코인들의 표준 가격
    int ORANGE_STANDARD;
    int PINK_STANDARD;
    int RED_STANDARD;
    int PURPLE_STANDARD;
    // 코인들의 그날 가격
    int orangevalue;
    int pinkvalue;
    int redvalue;
    int purplevalue;
    // 코인들의 전날 가격
    int before_value[4];
}CoinManager;
// 색깔 열거형
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
// 커서의 위치를 옮기는 함수
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
// 플레이어 생성자
void set_player(Player* p) {
    int i;
    p->coin = 1000;
    p->rank = 1;
    p->total_gain_coin = 0;
    // 각 리스트 0으로 초기화
    for (i = 0; i < 4; i++) {
        p->orange_coin[i] = 0;
    }
    for (i = 0; i < 5; i++) {
        p->pink_coin[i] = 0;
    }
    for (i = 0; i < 6; i++) {
        p->red_coin[i] = 0;
    }
    for (i = 0; i < 7; i++) {
        p->purple_coin[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        p->expired_coin[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        p->selled_coin[i] = 0;
    }
}
// 코인 매니저 생성자
void SetCoinManager(CoinManager* c) {
    // 표준 가격 지정
    // const는 아니지만 변경하지 말 것
    c->ORANGE_STANDARD = 90;
    c->PINK_STANDARD = 810;
    c->RED_STANDARD = 7200;
    c->PURPLE_STANDARD = 63000;
    c->before_value[0] = 90;
    c->before_value[1] = 810;
    c->before_value[2] = 7200;
    c->before_value[3] = 63000;
}
void comparevalue(int before, int current) {
    if (before > current) {
        setColor(LIGHT_BLUE);
        printf("▼ %d", before - current);
    }
    else if (before < current) {
        setColor(LIGHT_RED);
        printf("▲ %d", current - before);
    }
    else {
        setColor(LIGHT_GREEN);
        printf("〓 0");
    }
    setColor(WHITE);
}
// 해당 일의 코인의 가격을 출력
void printinformation(CoinManager* c, Player* p) {
    setColor(LIGHT_YELLOW);
    printf("주황 코인 : %d(x%.2f) ", c->orangevalue, c->orangevalue / (float)c->ORANGE_STANDARD);
    comparevalue(c->before_value[0], c->orangevalue);
    printf("\n");
    setColor(LIGHT_MAGENTA);
    printf("분홍 코인 : %d(x%.2f) ", c->pinkvalue, c->pinkvalue / (float)c->PINK_STANDARD);
    comparevalue(c->before_value[1], c->pinkvalue);
    printf("\n");
    if (p->rank >= 2) {
        setColor(LIGHT_RED);
        printf("빨강 코인 : %d(x%.2f) ", c->redvalue, c->redvalue / (float)c->RED_STANDARD);
        comparevalue(c->before_value[2], c->redvalue);
        printf("\n");
    }if (p->rank >= 3) {
        setColor(MAGENTA);
        printf("보라 코인 : %d(x%.2f) ", c->purplevalue, c->purplevalue / (float)c->PURPLE_STANDARD);
        comparevalue(c->before_value[3], c->purplevalue);
        printf("\n");
    }
    setColor(WHITE);
}
// min ~ max -1 사이의 수를 반환
int Next(int min, int max) {
    return rand() % (max - min) + min;
}
// 각 코인에 맞는 가격을 기준보다 아래는 3등급, 기준 보다 위에는 7등급으로 매긴 뒤 그 구간에서 임의의 자연수를 가격으로 반환
int calculate(int min, int max, int stardard, int i) {
    if (i >= 3)
    {
        return Next((int)(stardard + (i - 3) * (max - stardard) / 7.0), (int)(stardard + (i - 2) * (max - stardard) / 7.0));
    }
    else
    {
        return Next((int)(min + i * (stardard - min) / 3.0), (int)(min + (i + 1) * (stardard - min) / 3.0));
    }
}
// coin_prob에 있는 값들과 coin_v의 값을 비교한 뒤 어떤 구간의 수를 뽑을 지 결정
int set(int min, int max, int standrad_value, int coin_v, int* coin_prob) {
    char done = 0;
    int coinvalue = 0;
    for (int i = 8; i >= 0; i--)
    {
        if (coin_v >= coin_prob[i])
        {
            coinvalue = calculate(min, max, standrad_value, i + 1);
            done = 1;
            break;
        }
    }
    if (!done) coinvalue = calculate(min, max, standrad_value, 0);
    return coinvalue;
}
// 코인들의 가격을 설정 setCoinValue보다 먼저 실행
void before_day_coin(CoinManager* c) {
    c->before_value[0] = c->orangevalue;
    c->before_value[1] = c->pinkvalue;
    c->before_value[2] = c->redvalue;
    c->before_value[3] = c->purplevalue;
}
void setCoinValue(CoinManager* c) {
    int coins_range[4][2] = { { 35,500},{250, 6000},{1800,6500 },{7000,700000 } }; // 코인들의 가격 최소, 최대
    int orange_prob[] = { 160, 400, 700, 850, 920, 950, 972, 985, 994 }; // 각 구간의 가격을 가질 확률
    int pink_prob[] = { 190, 460, 740, 850, 905, 940, 963, 982, 993 };
    int red_prob[] = { 220, 520, 780, 850, 895, 925, 950, 970, 987 };
    int purple_prob[] = { 240, 580, 820, 855, 884, 910, 935, 958, 980 };
    int orange_v = rand() % 1000; // 수치 지정
    int pink_v = rand() % 1000;
    int red_v = rand() % 1000;
    int purple_v = rand() % 1000;
    //========================================================================================
    // 코인 가격 계산
    c->orangevalue = set(coins_range[0][0], coins_range[0][1], c->ORANGE_STANDARD, orange_v, orange_prob);
    c->pinkvalue = set(coins_range[1][0], coins_range[1][1], c->PINK_STANDARD, pink_v, pink_prob);
    c->redvalue = set(coins_range[2][0], coins_range[2][1], c->RED_STANDARD, red_v, red_prob);
    c->purplevalue = set(coins_range[3][0], coins_range[3][1], c->PURPLE_STANDARD, purple_v, purple_prob);
}
// 코인 타입 출력
void print_type(int type) {
    switch (type) {
    case 1:
        printf("주황");
        break;
    case 2:
        printf("분홍");
        break;
    case 3:
        printf("빨강");
        break;
    case 4:
        printf("보라");
        break;
    }
}

void print_coin_count(Player* p) {
    int i, count;
    if (p->rank >= 3) {
        setColor(MAGENTA);
        count = 0;
        printf("현재 가지고 있는 보라 코인 수(앞에서 부터 만료기간 7 ... 1일)\n");
        for (i = 0; i < 7; i++) {
            printf("%d일 : %d, ", 7 - i, p->purple_coin[i]);
            count += p->purple_coin[i];
        }
        printf("총 개수 : %d\n", count);
    }
    if (p->rank >= 2) {
        setColor(RED);
        count = 0;
        printf("현재 가지고 있는 빨강 코인 수(앞에서 부터 만료기간 6 ... 1일)\n");
        for (i = 0; i < 6; i++) {
            printf("%d일 : %d, ", 6 - i, p->red_coin[i]);
            count += p->red_coin[i];
        }
        printf("총 개수 : %d\n", count);
    }

    setColor(LIGHT_MAGENTA);
    count = 0;
    printf("현재 가지고 있는 분홍 코인 수(앞에서 부터 만료기간 5 ... 1일)\n");
    for (i = 0; i < 5; i++) {
        printf("%d일 : %d, ", 5 - i, p->pink_coin[i]);
        count += p->pink_coin[i];
    }
    printf("총 개수 : %d\n", count);

    setColor(LIGHT_YELLOW);
    count = 0;
    printf("현재 가지고 있는 주황 코인 수(앞에서 부터 만료기간 4 ... 1일)\n");
    for (i = 0; i < 4; i++) {
        printf("%d일 : %d, ", 4 - i, p->orange_coin[i]);
        count += p->orange_coin[i];
    }
    printf("총 개수 : %d\n", count);
    setColor(WHITE);
    for (i = 0; i < 4; i++) {
        printf("만료된 ");
        print_type(i + 1);
        printf(" 코인의 개수 : %d\n", p->expired_coin[i]);
    }

}
void print_achieve(int b) {
    if (b) {
        printf("달성");
    }
    else {
        printf("미달성");
    }
}
// 플레이어의 정보 출력(랭크, 누적 코인 판매 개수, 누적 판매액, 랭크를 올리기 위한 조건 출력)
void print_player_information(Player* p) {
    int i;
    switch (p->rank) {
    case 1:
        printf("현재 랭크 : %c\n", 'C');
        break;
    case 2:
        printf("현재 랭크 : %c", 'B');
        break;
    case 3:
        printf("현재 랭크 : %c", 'A');
        break;
    }

    printf("현재까지 판 코인의 개수\n");
    for (i = 0; i < 4; i++) {
        print_type(i + 1);
        printf(" 코인 : %d개\n", p->selled_coin[i]);
    }
    printf("누적으로 얻은 돈(손실 제외) : %d원\n", p->total_gain_coin);
    if (p->rank == 1) {
        printf("B랭크가 되기 위한 조건\n");
        printf("주황 코인 100개 이상 판매 : %d/100 -> ", p->selled_coin[0]);
        print_achieve(p->selled_coin[0] >= 100);
        printf("\n");
        printf("분홍 코인 15개 이상 판매 : %d/15 -> ", p->selled_coin[1]);
        print_achieve(p->selled_coin[1] >= 15);
        printf("\n");
        printf("손실 제외 누적 수익 15000원 이상 : %d/15000 -> ", p->total_gain_coin);
        print_achieve(p->total_gain_coin >= 15000);
        printf("\n");
    }
    if (p->rank == 2) {
        printf("A랭크가 되기 위한 조건\n");
        printf("빨강 코인 30개 이상 판매 : %d/30 -> ", p->selled_coin[2]);
        print_achieve(p->selled_coin[2] >= 30);
        printf("\n");
        printf("손실 제외 누적 수익 500000원 이상 : %d/500000 -> ", p->total_gain_coin);
        print_achieve(p->total_gain_coin >= 500000);
        printf("\n");
    }
}
int purchase(Player* p, int price, int buy_count, int type) {
    if (price * buy_count <= p->coin) {
        p->coin -= price * buy_count;
        print_type(type);
        printf(" 코인을 %d개 구입했습니다.\n", buy_count);
        return buy_count;
    }
    else {
        printf("현재 ");
        print_type(type);
        printf(" 코인 %d개는 %d원 이지만 현재 %d원 밖에 없습니다.\n", buy_count, price * buy_count, p->coin);
        return 0;
    }
}

void buy_coin(Player* p, CoinManager* c, int buyCount, int cointype) {
    if (buyCount >= 0)
    {
        switch (cointype)
        {
        case 1:
            p->orange_coin[0] += purchase(p, c->orangevalue, buyCount, 1);
            break;
        case 2:
            p->pink_coin[0] += purchase(p, c->pinkvalue, buyCount, 2);
            break;
        case 3:
            if (p->rank >= 2)
            {
                p->red_coin[0] += purchase(p, c->redvalue, buyCount, 3);
            }
            else
            {
                printf("B랭크 이상만 빨강 코인을 구매할 수 있습니다.\n");
            }
            break;
        case 4:
            if (p->rank >= 3)
            {
                p->purple_coin[0] += purchase(p, c->purplevalue, buyCount, 4);
            }
            else
            {
                printf("A랭크 이상만 보라 코인을 구매할 수 있습니다.\n");
            }
            break;
        }
    }
    else {
        printf("구입 개수에 음수는 입력할 수 없습니다.");
    }
}
void sell_coin(CoinManager* c, Player* p, int coin_type, int coin_count) {
    int leftcoin = coin_count;
    int count = 0;
    if (coin_count >= 1) {
        switch (coin_type)
        {
        case 1:
            for (int i = 0; i < 4; i++)
            {
                count += p->orange_coin[i];
            }
            if (count < coin_count)
            {
                printf("입력한 개수보다 적은 주황 코인을 갖고 있습니다\n");
                break;
            }
            else
            {
                for (int i = 3; i >= 0; i--)
                {
                    if (leftcoin <= p->orange_coin[i])
                    {
                        p->orange_coin[i] -= leftcoin;
                        leftcoin = 0;
                        break;
                    }
                    else
                    {
                        leftcoin -= p->orange_coin[i];
                        p->orange_coin[i] = 0;
                    }
                }
                p->coin += c->orangevalue * coin_count;
                p->total_gain_coin += c->orangevalue * coin_count;
                p->selled_coin[0] += coin_count;
                printf("주황 코인을 %d개 팔고 %d원을 받았습니다.\n", coin_count, c->orangevalue * coin_count);
            }
            break;
        case 2:
            for (int i = 0; i < 5; i++)
            {
                count += p->pink_coin[i];
            }
            if (count < coin_count)
            {
                printf("입력한 개수보다 적은 분홍 코인을 갖고 있습니다.\n");
                break;
            }
            else
            {
                for (int i = 4; i >= 0; i--)
                {
                    if (leftcoin <= p->pink_coin[i])
                    {
                        p->pink_coin[i] -= leftcoin;
                        leftcoin = 0;
                        break;
                    }
                    else
                    {
                        leftcoin -= p->pink_coin[i];
                        p->pink_coin[i] = 0;
                    }
                }
                p->coin += c->pinkvalue * coin_count;
                p->total_gain_coin += c->pinkvalue * coin_count;
                p->selled_coin[1] += coin_count;
                printf("분홍 코인을 %d개 팔고 %d원을 받았습니다.\n", coin_count, c->pinkvalue * coin_count);
            }
            break;
        case 3:
            for (int i = 0; i < 6; i++)
            {
                count += p->red_coin[i];
            }
            if (count < coin_count)
            {
                printf("입력한 개수보다 적은 빨강 코인을 갖고 있습니다\n");
                break;
            }
            else
            {
                for (int i = 5; i >= 0; i--)
                {
                    if (leftcoin <= p->red_coin[i])
                    {
                        p->red_coin[i] -= leftcoin;
                        leftcoin = 0;
                        break;
                    }
                    else
                    {
                        leftcoin -= p->red_coin[i];
                        p->red_coin[i] = 0;
                    }
                }
                p->coin += c->redvalue * coin_count;
                p->total_gain_coin += c->redvalue * coin_count;
                p->selled_coin[2] += coin_count;
                printf("빨강 코인을 %d개 팔고 %d원을 받았습니다.\n", coin_count, c->redvalue * coin_count);
            }
            break;
        case 4:
            for (int i = 0; i < 7; i++)
            {
                count += p->purple_coin[i];
            }
            if (count < coin_count)
            {
                printf("입력한 개수보다 적은 보라 코인을 갖고 있습니다\n");
                break;
            }
            else
            {
                for (int i = 6; i >= 0; i--)
                {
                    if (leftcoin <= p->purple_coin[i])
                    {
                        p->purple_coin[i] -= leftcoin;
                        leftcoin = 0;
                        break;
                    }
                    else
                    {
                        leftcoin -= p->purple_coin[i];
                        p->purple_coin[i] = 0;
                    }
                }
                p->coin += c->purplevalue * coin_count;
                p->total_gain_coin += c->purplevalue * coin_count;
                p->selled_coin[3] += coin_count;
                printf("보라 코인을 %d개 팔고 %d원을 받았습니다.\n", coin_count, c->purplevalue * coin_count);
            }
            break;
        }
    }
}
void CoinNextDay(Player* p) {
    int i;
    for (i = 3; i >= 0; i--) {
        if (i == 3) {
            if (p->orange_coin[i] > 0) printf("주황 코인 %d개가 만료되었습니다...\n", p->orange_coin[i]);
            p->expired_coin[0] += p->orange_coin[i];
        }
        else {
            p->orange_coin[i + 1] = p->orange_coin[i];
        }
    }
    p->orange_coin[0] = 0;
    for (i = 4; i >= 0; i--) {
        if (i == 4) {
            if (p->pink_coin[i] > 0) printf("분홍 코인 %d개가 만료되었습니다...\n", p->pink_coin[i]);
            p->expired_coin[1] += p->pink_coin[i];
        }
        else {
            p->pink_coin[i + 1] = p->pink_coin[i];
        }
    }
    p->pink_coin[0] = 0;
    for (i = 5; i >= 0; i--) {
        if (i == 5) {
            if (p->red_coin[i] > 0) printf("빨강 코인 %d개가 만료되었습니다...\n", p->red_coin[i]);
            p->expired_coin[2] += p->red_coin[i];
        }
        else {
            p->red_coin[i + 1] = p->red_coin[i];
        }
    }
    p->red_coin[0] = 0;
    for (i = 6; i >= 0; i--) {
        if (i == 6) {
            if (p->purple_coin[i] > 0) printf("보라 코인 %d개가 만료되었습니다...\n", p->purple_coin[i]);
            p->expired_coin[3] += p->purple_coin[i];
        }
        else {
            p->purple_coin[i + 1] = p->purple_coin[i];
        }
    }
    p->purple_coin[0] = 0;
}
// 만료된 코인을 판매함(한 주가 지난 뒤 실행)
void return_expired_coin(Player* p) {
    printf("한 주가 지났으므로 가지고 계신 만료된 코인은 기준 가치의 1/20로 자동 판매됩니다.\n");
    printf("가격 -> 주황 코인 5원, 분홍 코인 41원, 빨강 코인 360원, 보라 코인 3150원\n");
    int i;
    for (i = 0; i < 4; i++) {
        print_type(i + 1);
        printf(" 코인 : %d개\n", p->expired_coin[i]);
    }
    int gain = 5 * p->expired_coin[0] + 41 * p->expired_coin[1] + 360 * p->expired_coin[2] + 3150 * p->expired_coin[3];
    p->coin += gain;
    printf("만료된 코인들을 팔아서 %d원을 얻었습니다.\n", gain);

}
void check_rank_up(Player* p) {
    if (p->rank == 1 && p->selled_coin[0] >= 100 && p->selled_coin[1] >= 15 && p->total_gain_coin >= 15000) {
        p->rank = 2;
        printf("조건을 달성해서 B랭크로 올랐습니다! 축하합니다!\n");
        printf("이제 빨강 코인을 구매할 수 있습니다.\n");
    }
    if (p->rank == 2 && p->selled_coin[2] >= 30 && p->total_gain_coin >= 500000) {
        p->rank = 3;
        printf("조건을 달생해서 A랭크로 올랐습니다! 축하합니다!\n");
        printf("이제 보라 코인을 구매할 수 있습니다.\n");
    }
}


// =========================================================

static int week = 1;
//static char days[][2] = {  };
static int day = 0;
static char running = 1;
static char quit = 0;

static void print_day(int day) {
    switch (day) {
    case 0:
        printf("월");
        break;
    case 1:
        printf("화");
        break;
    case 2:
        printf("수");
        break;
    case 3:
        printf("목");
        break;
    case 4:
        printf("금");
        break;
    case 5:
        printf("토");
        break;
    case 6:
        printf("일");
        break;
    }//, , , , , ,
}
static int action_list[5] = { 0,0,0,0,0 };
static void set_list(int i0, int i1, int i2, int i3, int i4) {
    action_list[0] = i0;
    action_list[1] = i1;
    action_list[2] = i2;
    action_list[3] = i3;
    action_list[4] = i4;
}
static void Nextday(CoinManager* c, Player* p) {
    before_day_coin(c);
    setCoinValue(c);
    CoinNextDay(p);
    if (day >= 6) {
        return_expired_coin(p);
        week++;
        day = 0;
    }
    else {
        day++;
    }
}
static int Player_action(int* a) {
    int b;
    if (a[0]) printf("1 : 코인 구매(월요일, 목요일만 가능)\n");
    if (a[1]) printf("2 : 코인 판매\n");
    if (a[2]) printf("3 : 통계\n");
    if (a[3]) printf("4 : 다음 날로ㅡ,ㅡ\n");
    if (a[4]) printf("5 : 게임 종료\n");
    printf("원하는 기능의 번호를 입력하세요. : ");
    scanf("%d", &b);
    while (1)
    {
        if (a[b - 1]) break;
        else
        {
            printf("숫자가 없거나 잘못되었습니다. 다시 입력하세요. : ");
            scanf("%d", &b);
        }
    }
    return b;
}

static void active(int num, CoinManager* c, Player* p) {
    int coinCount[] = { 0, 0, 0, 0 };
    int sellcount[4], selltype;
    int coincount[4] = { 0,0,0,0 };
    int i;
    switch (num)
    {
    case 1:
        // 코인 구매
        printf("==================================================\n");
        printf("모든 종류의 코인을 한 번에 사는 것이 아니라 낮은 코인 부터 구매합니다.\n");
        printf("한 종류의 코인을 구매할 때마다 돈이 차감됨에 유의하세요.\n");
        printf("구매를 원하지 않는 경우 0을 입력하세요.\n");
        printf("남은 돈 : %d\n", p->coin);
        printf("주황 코인을 몇 개 구입할까요?(%d개 가능) : ", p->coin / c->orangevalue);
        scanf("%d", &coinCount[0]);
        buy_coin(p, c, coinCount[0], 1);
        printf("분홍 코인을 몇 개 구입할까요? : (%d개 가능) : ", p->coin / c->pinkvalue);
        scanf("%d", &coinCount[1]);
        buy_coin(p, c, coinCount[1], 2);
        if (p->rank >= 2)
        {
            printf("빨강 코인을 몇 개 구입할까요? : (%d개 가능) : ", p->coin / c->redvalue);
            scanf("%d", &coinCount[2]);
            buy_coin(p, c, coinCount[2], 3);
        }
        if (p->rank >= 3)
        {
            printf("보라 코인을 몇 개 구입할까요? : (%d개 가능) : ", p->coin / c->purplevalue);
            scanf("%d", &coinCount[3]);
            buy_coin(p, c, coinCount[3], 4);
        }
        break;
    case 2:
        // 코인 판매
        printf("판매를 원하지 않는 경우 0을 입력하세요.\n");
        for (i = 0; i < 4; i++) {
            coincount[0] += p->orange_coin[i];
        }
        for (i = 0; i < 5; i++) {
            coincount[1] += p->pink_coin[i];
        }
        for (i = 0; i < 6; i++) {
            coincount[2] += p->red_coin[i];
        }
        for (i = 0; i < 7; i++) {
            coincount[3] += p->purple_coin[i];
        }
        printf("주황 코인을 몇 개 판매할까요?(현재 : %d개): ", coincount[0]);
        scanf("%d", &sellcount[0]);
        printf("분홍 코인을 몇 개 판매할까요?(현재 : %d개) : ", coincount[1]);
        scanf("%d", &sellcount[1]);
        if (p->rank >= 2) {
            printf("빨강 코인을 몇 개 판매할까요?(현재 : %d개) : ", coincount[2]);
            scanf("%d", &sellcount[2]);
        }
        if (p->rank >= 3) {
            printf("보라 코인을 몇 개 판매할까요?(현재 : %d개) : ", coincount[3]);
            scanf("%d", &sellcount[3]);
        }
        sell_coin(c, p, 1, sellcount[0]);
        sell_coin(c, p, 2, sellcount[1]);
        if (p->rank >= 2) sell_coin(c, p, 3, sellcount[2]);
        if (p->rank >= 3) sell_coin(c, p, 4, sellcount[3]);
        break;
    case 3:
        print_coin_count(p);
        print_player_information(p);
        // 코인 개수 확인
        break;
    case 4:
        Nextday(c, p);
        // 다음 날
        break;
    case 5:
        quit = 1;
        break;
    }
}
int main()
{
    srand(time(NULL));
    // == 구조체 정의==
    int player_select;
    CoinManager c;
    CoinManager* coin = &c;
    Player p;
    Player* player = &p;
    set_player(player);
    setColor(WHITE);
    SetCoinManager(coin);
    setCoinValue(coin);
    while (running)
    {
        system("cls");
        printf("%d주 ", week);
        print_day(day);
        printf("요일\n");
        printinformation(coin, player);
        printf("현재 가진 금액 : %d\n", player->coin);
        if (day == 0 || day == 3) {
            set_list(1, 1, 1, 1, 1);
            player_select = Player_action(action_list);
        }
        else
        {
            set_list(0, 1, 1, 1, 1);
            player_select = Player_action(action_list);
        }
        active(player_select, coin, player);
        check_rank_up(player);
        printf("===============================\n아무 키나 눌러서 다음으로 진행하세요... ");
        while (!_kbhit());
        if (quit) break;
    }
    //printf("Hello world!\n");
    return 0;
}
