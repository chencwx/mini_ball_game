void Init_Blocks(void);
void Draw_Blocks(void);
#define CLASS_NAME    TEXT("mini撞球小游戏")
#define WIDTH      600
#define HEIGHT        480
//游戏各状态定义
#define GAME_INIT        0
#define GAME_START_LEVEL  1
#define GAME_RUN          2
#define GAME_OVER    3
#define GAME_EXIT        4
//砖块定义
#define BLOCK_ROW          6
#define BLOCK_COLUMN      8
#define BLOCK_WIDTH            64
#define BLOCK_HEIGHT            16
#define INIT_X          9
#define INIT_Y          9
#define GAP_X          88
#define GAP_Y            32
// 挡板状态
#define BAN_STARTSTATE_X         (WIDTH/2 - 16)
#define BAN_STARTSTATE_Y          (HEIGHT - 32);
#define BAN_WIDTH            80
#define BAN_HEIGHT         8
#define BAN_COLOR            RGB(255,255, 255)
// 球状态
#define BALL_START_Y            (HEIGHT/2)
#define BALL_SIZE                8.5
#define BALL_START_X       (WIDTH/2)
// 颜色定义
#define BACKGROUND_COLOR        RGB(0, 0,0)//背景颜色
#define BLOCK_COLOR                RGB(0, 255, 186)//砖块颜色
#define BALL_COLOR                RGB(255, 196, 156)//球颜色
// 键盘定义（游戏使用键盘控制）上网查询资料得知
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
