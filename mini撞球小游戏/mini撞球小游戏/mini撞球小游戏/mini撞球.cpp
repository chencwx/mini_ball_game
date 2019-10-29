#include "clock.h"//实现球随机落下
#include <stdio.h>
#include <windows.h>
#include"block.h"
#include<mmsystem.h>//播放音乐

/********************************************************************************
* ##########关于Windows数据类型##########
*
* Windows在C语言的基础上定义了很多Windows特有的类型。都是通过C语言关键字typedef定义的。
* Windows类型都是全大写。
*
* DWORD LPSTR WPARAM LPARAM HWND等
* 其中，以'H'大头的数据类型都是句柄*/

#define CLASS_NAME    TEXT("mini撞球小游戏")
#define WIDTH      600
#define HEIGHT        480
//游戏各状态定义
#define GAME_INIT        0
#define GAME_START_LEVEL  1
#define GAME_RUN          2
#define GAME_OVER    3
#define GAME_EXIT        4
//矩形块定义
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
#define BAN_STARTSTATE_Y          (HEIGHT - 32)
#define BAN_WIDTH            80
#define BAN_HEIGHT         8
#define BAN_COLOR            RGB(255,255, 255)
// 球状态
#define BALL_START_Y            (HEIGHT/2)
#define BALL_SIZE                9
#define BALL_START_X       (WIDTH/2)
// 颜色定义
#define BACKGROUND_COLOR        RGB(0, 0,0)//背景颜色
#define BLOCK_COLOR                RGB(87, 250, 255)//矩形块颜色
#define BALL_COLOR                RGB(255, 196, 156)//球颜色
// 键盘定义（游戏使用键盘控制）上网查询资料得知
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//函数声明
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);
DWORD Start_Clock(void);
DWORD Wait_Clock(DWORD count);
//重命名无符号类型
typedef unsigned short UNSHORT;
typedef unsigned char  UNCHAR;
//全局变量
HWND        main_window_handle = NULL;
HINSTANCE    main_instance = NULL;   // HINSTANCE是用来表示程序运行实例的句柄，某些API
//函数会使用到这个变量。实际上是一个无符号长整型   
int            game_state = GAME_INIT;    // 游戏初始状态
int            ban_x = 0, ban_y = 0;                //板的轨迹
int            ball_x = 0, ball_y = 0;               //球的轨迹
int            ball_dx = 0, ball_dy = 0;                // 球速
int            score = 0;                            // 分数
int            level = 1;                            // 等级
int            blocks_hit = 0;                            // 挡板击打的轨迹
DWORD        clock_scount = 0;                // 用于clock
  //矩形块的数据
UNCHAR blocks[BLOCK_ROW][BLOCK_COLUMN];//类似二维数组  紫色为宏
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);//typedef定义的类型名
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK MainWndProc(HWND    hwnd,
	UINT    msg,
	WPARAM    wparam,// 消息参数，不同的消息有不同的意义，详见MSDN中每个消息的文档
	LPARAM    lparam)
{// 注意，是switch-case, 每次这个函数被调用，只会落入到一个case中。

	PAINTSTRUCT    ps;
	HDC            hdc;
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//窗口移动，默认即可
}
/* WINMAIN ********************************************************************************/
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASS    wc;//结构体
	// 窗口句柄，hwnd变量是主窗口的句柄，这个程序中只用到了一个窗口。
	HWND        hwnd;
	MSG            msg;
	HDC            hdc;
	PAINTSTRUCT    ps;

	/*******************************************************************************
	* ##########   关于 API 函数   ##########
	*
	* API是 Application Programming Interface的缩写，即：应用程序编程接口。
	* API是Windows操作系统提供给应用程序调用的一系列功能的接口函数。用于应用程序调用操作系统的底层功能
	*
	* API函数的使用方法类似C语言的库函数，如printf，fopen，fread等。
	* 在VS环境中使用C或者C++进行编程是，只需#include <windows.h>即可调用大部分Windows API函数。
	*
	* 在下面一端代码中使用到的 LoadIcon LoadCursor GetStockObject RegisterClass CreateWindow等函数都是API
	*
	* Windows应用程序开发过程中的主要工作就是使用API。
	*
	****************************************************/
	/*******************************************************************************
	* ##########   窗口类   ##########
	*
	* 在创建窗口之前，首先需要注册一个窗口类，窗口类可以理解为同一种类的窗口。
	* 一个窗口类可以创建多个窗口，属于同一窗口类的窗口具有相同的消息处理方法和相似的样式等
	*
	* 窗口类和C++中的类英文都是class，但是它们不是同一概念。
	* C++类使用class关键字定义，而窗口类是使用RegisterClass API函数向Windows系统注册。
	*
	* 下面的 WNDCLASS wc 是注册窗口类的 API函数RegisterClass的参数，是一个结构体类型，
	* 用来存储一个窗口类的各种属性
	*
	************************************/
	wc.style = CS_HREDRAW | CS_VREDRAW;//⑴CS_HREDRAW：一旦移动或尺寸调整使客户区的
									   //宽度发生变化，就重新绘制窗口； ⑵CS_VREDRAW：一旦移动或尺寸调整使客户区的高度发生变化，就重新绘制窗口。
									   // 一个函数指针，这个函数用来处理窗口消息。 详见 MainWndProc函数的注释。

	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);
	// Cursor是鼠标光标，这里是设定了鼠标光标的样式。
	// 直接使用LoadCursor API函数载入了一个系统预定义的光标样式，还有IDC_CROSS,IDC_HAND等样式 
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;//用字符串会出错

	wc.lpszClassName = CLASS_NAME;
	// register the window class
	//PlaySound(TEXT("E:\\CloudMusic\\Richard Clayderman - 梦中的婚礼"),
	//	NULL, SND_FILENAME | SND_ASYNC);(加入背景音失败）
	if (!RegisterClass(&wc))

	{
		// 窗口注册失败，消息框提示，并退出。

		return -1;
	}
	// 注册成功继续运行
	hwnd = CreateWindow(
		CLASS_NAME,            // // 窗口类名，必须是已经注册了的窗口类
		TEXT("mini撞球小游戏"),    //title-bar string 
		WS_POPUP,                    // 窗口的style，无边框形式
		200,                        // initial x
		100,                        // initial y   
		WIDTH,                // initial width
		HEIGHT,                // initial height
		NULL,                        // handle to parent
		NULL,                        // handle to menu
		hinstance,                    // instance
		NULL);                        // creation parms
	if (!hwnd)
		return 0;
	ShowWindow(hwnd, ncmdshow);
	UpdateWindow(hwnd);

	main_window_handle = hwnd;
	main_instance = hinstance;

	Game_Init();
	// enter main event loop
	/*******************************************************************************
	* ##########   MSG类型和消息    ##########
	*
	* Windows窗口应用程序是一种消息驱动的程序框架。
	* Windows窗口应用程序运行以后，其主线程就一直处于
	* "等待消息--获取消息--效应消息" 的循环之中，直到程序退出。
	*
	* 所谓消息是Windows系统特有的一种系统对象和机制，
	* 是操作系统对某些系统状态（如时钟）和用户操作（如鼠标点击）产生的系统中断进行封装以后，
	* 可以发送到应用程序，并由应用程序进行处理的用户态对象。
	*
	* "MSG"就是用来表示"消息"的数据类型。 这里定义了一个MSG类型的变量，变量名为msg
	*
	*****************************************/
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//判断消息
		{

			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			////将虚拟键消息转换为字符消息
			DispatchMessage(&msg);////函数功能：该函数调度一个消息给窗口程序。
		}
		// main game processing goes here
		Game_Main();
		Sleep(30);
	}

	Game_Shutdown();//结束游戏 释放内存

	return (msg.wParam);
}
//绘图
int Draw_Rectangle(int x1, int y1, int x2, int y2, int color)
{
	/*来看GDI最基础的三个概念：
		* 1、DC：DC的全称是Device Context，绘制操作在此进行。可以类比理解为一块画布，对应屏幕的一个区域。
		*      GDI的绘制操作必须在一个DC上进行，因此第一步就是得到DC的句柄，
		BeginPaint或者GetDC函数。
		* 2、GDI对象：包括PEN、BRUSH、FONT等，每一种绘制操作都会使
		用到一个或者多个GDI对象，
		*      所以，在绘制操作之前必须的一个操作是将绘制所用
		的GDI对象放入DC中，使用SelectObject函数完成。
		* 3、绘制操作：即在DC上使用被选择如DC中的GDI对象绘制图形、线条、文字等。
		*      如LineTo画出线条、Ellipse画出(椭)圆
		、Rectangle画出矩形、TextOut输出文字
		*/
	HBRUSH        hbrush;
	HDC            hdc;
	RECT        rect;
	SetRect(&rect, x1, y1, x2, y2);
	hbrush = CreateSolidBrush(color);
	hdc = GetDC(main_window_handle);
	FillRect(hdc, &rect, hbrush);//画背景
	ReleaseDC(main_window_handle, hdc);
	DeleteObject(hbrush);
	return 1;
}
int DrawText_GUI(TCHAR *text, int x, int y, int color)
{
	HDC    hdc;
	hdc = GetDC(main_window_handle);

	SetTextColor(hdc, color);

	SetBkMode(hdc, TRANSPARENT);

	TextOut(hdc, x, y, text, lstrlen(text));

	ReleaseDC(main_window_handle, hdc);//回收和释放资源
	return 1;
}

//游戏具体实现
void Init_Blocks(void)
{
	// 矩形块初始化
	for (int row = 0; row < BLOCK_ROW; row++)
		for (int col = 0; col < BLOCK_COLUMN; col++)
			blocks[row][col] = 1;
}
void Draw_Blocks(void)
{
	//画矩形块
	int x1 = INIT_X;
	int y1 = INIT_Y;

	for (int row = 0; row < BLOCK_ROW; row++)
	{
		//重置矩形块位置
		x1 = INIT_X;
		for (int count = 0; count < BLOCK_COLUMN; count++)
		{
			if (blocks[row][count] != 0)
			{
				Draw_Rectangle(x1, y1, x1 + BLOCK_WIDTH, y1 + BLOCK_HEIGHT, BLOCK_COLOR);
			}
			else
			{
				Draw_Rectangle(x1, y1, x1 + BLOCK_WIDTH, y1 + BLOCK_HEIGHT, BACKGROUND_COLOR);
			}
			x1 += GAP_X;        //更新矩形块的位置
		}
		y1 += GAP_Y;            //更新纵坐标
	}
}
int Process_Ball(void)
{
	// 测试球与板的位置 如果碰撞，则弹起
	int x1 = INIT_X;
	int y1 = INIT_Y;
	//计算球的中心
	int ball_cx = ball_x + (BALL_SIZE / 2);
	int ball_cy = ball_y + (BALL_SIZE / 2);
	// 判断球是否在挡板上
	if (ball_y > (HEIGHT / 2) && ball_dy > 0)
	{
		//获得球的边缘位置
		int x = ball_x + (BALL_SIZE / 2);
		int y = ball_y + (BALL_SIZE / 2);

		if ((x >= ban_x && x <= ban_x + BAN_WIDTH) &&
			(y >= ban_y && y <= ban_y + BAN_HEIGHT))
		{
			ball_dy = -ball_dy;        // 反弹球
			ball_y += ball_dy;        // 使球离开挡板
			if (KEY_DOWN(VK_RIGHT))//键盘左右键操作
				ball_dx -= rand() % 3;
			else if (KEY_DOWN(VK_LEFT))
				ball_dx += rand() % 3;
			else
				ball_dx += (-1 + rand() % 3);

			MessageBeep(ERROR_SMB_GUEST_LOGON_BLOCKED);//制造音效
			return 0;
		}
	}

	for (int row = 0; row < BLOCK_ROW; row++)
	{
		x1 = INIT_X;
		//扫描这一行的矩形块
		for (int count = 0; count < BLOCK_COLUMN; count++)
		{
			if (blocks[row][count] != 0)
			{
				//判断球是否越过边界
				if ((ball_cx > x1) && (ball_cx < x1 + BLOCK_WIDTH) &&
					(ball_cy > y1) && (ball_cy < y1 + BLOCK_HEIGHT))
				{
					blocks[row][count] = 0;            //移动矩形块
													 //判断什么时候升级
					blocks_hit++;
					ball_dy = -ball_dy;                // 弹球              
					ball_dx += (-1 + rand() % 3);
					MessageBeep(MB_OK);                // 发出声音
				//判断是否需要还有矩形块即是否需要升级									  
					if (blocks_hit >= (BLOCK_ROW * BLOCK_COLUMN))
					{
						game_state = GAME_START_LEVEL;
						level++;
					}
					//加分
					score += 5 * (level + (abs)(ball_dx));
					return 1;
				}
			}
			x1 += GAP_X;        // 更新位置
		}
		y1 += GAP_Y;
	}
	return 0;
}
int Game_Init(void *parms)
{
	//游戏重新开始即初始化
	return 1;
}
int Game_Shutdown(void *parms)
{
	//游戏结束并释放内存
	return 1;
}
int Game_Main(void *parms)
{
	//类似于主函数
	TCHAR    huan[80];
	BOOL    banMove = FALSE;//版移动
	int       old_ban_x, old_ban_y;
	int        old_ball_x, old_ball_y;
	//判断游戏状态
	if (game_state == GAME_INIT)
	{
		//用clock使每次游戏都不同
		srand((unsigned int)time(0));

		ban_x = BAN_STARTSTATE_X;
		ban_y = BAN_STARTSTATE_Y;
		//设置球的位置及速度
		ball_x = 8 + rand() % (WIDTH - 16);
		ball_y = BALL_START_Y;
		ball_dx = -4 + rand() % (8 + 1);
		ball_dy = 6 + rand() % 2;
		//转为初始状态
		game_state = GAME_START_LEVEL;
	}
	else if (game_state == GAME_START_LEVEL)
	{
		//到一个新的等级重新开始游戏
		Init_Blocks();                // 初始化矩形块
		blocks_hit = 0;
		Draw_Blocks();                // 画矩形块
									  // 画挡板
		Draw_Rectangle(ban_x, ban_y, ban_x + BAN_WIDTH, ban_y + BAN_HEIGHT, BAN_COLOR);
		game_state = GAME_RUN;// 使游戏开始
	}
	else if (game_state == GAME_RUN)
	{
		//移动挡板
		if (KEY_DOWN(VK_RIGHT))
		{
			old_ban_x = ban_x;
			old_ban_y = ban_y;
			ban_x += 8;            // 挡板往右移
									  // 确保挡板在框内
			if (ban_x > (WIDTH - BAN_WIDTH))
				ban_x = WIDTH - BAN_WIDTH;
			banMove = TRUE;
		}
		else if (KEY_DOWN(VK_LEFT))
		{
			old_ban_x = ban_x;
			old_ban_y = ban_y;
			ban_x -= 8;            // 挡板往右移

			if (ban_x < 0)
				ban_x = 0;
			banMove = TRUE;
		}
		old_ball_x = ball_x;
		old_ball_y = ball_y;
		// 移动球
		ball_x += ball_dx;
		ball_y += ball_dy;
		//判断球是否在框内
		if (ball_x > (WIDTH - BALL_SIZE) || ball_x < 0)
		{
			ball_dx = -ball_dx;        //反弹球
			ball_x += ball_dx;       //更新位置（横坐标）
		}

		if (ball_y < 0)
		{
			ball_dy = -ball_dy;
			ball_y += ball_dy;
		}
		else if (ball_y > (HEIGHT - BALL_SIZE))
		{
			MessageBeep(MB_DEFAULT_DESKTOP_ONLY);//游戏结束提示
			MessageBoxA(0, "大吉大利 游戏结束", "Game over!!!", 0);
			//之前用老师代码中的messagebox 结果无法显示文字 加了A后成功

			ExitProcess(0);
			//游戏结束 由于挡板未接到球

		}
		//注意球的速率
		if (ball_dx > 8)
			ball_dx = 8;
		else if (ball_dx < -8)
			ball_dx = -8;
		//判断球撞到挡板或出框
		if (Process_Ball())
		{
			Draw_Blocks();
		}
		if (banMove)
		{
			// 覆盖旧的挡板
			Draw_Rectangle(old_ban_x, old_ban_y, old_ban_x + BAN_WIDTH, old_ban_y + BAN_HEIGHT,
				BACKGROUND_COLOR);

			Draw_Rectangle(ban_x, ban_y, ban_x + BAN_WIDTH, ban_y + BAN_HEIGHT, BAN_COLOR);
		}
		// 覆盖旧的ball
		Draw_Rectangle(old_ball_x, old_ball_y, old_ball_x + BALL_SIZE, old_ball_y + BALL_SIZE, BACKGROUND_COLOR);

		Draw_Rectangle(ball_x, ball_y, ball_x + BALL_SIZE, ball_y + BALL_SIZE, BALL_COLOR);

		wsprintf(huan, TEXT("mini撞球小游戏           Score %d              Level %d"), score, level);
		Draw_Rectangle(8, HEIGHT - 26, WIDTH, HEIGHT, BACKGROUND_COLOR);
		DrawText_GUI(huan, 8, HEIGHT - 26, RGB(255, 255, 128));//下标题
		//判断玩家是否还想玩
		if (KEY_DOWN(VK_ESCAPE))//按ESC键表示停止游戏
		{

			PostMessage(main_window_handle, WM_DESTROY, 0, 0);

			game_state = GAME_OVER;

		}
	}
	else if (game_state == GAME_OVER)
	{

		game_state = GAME_EXIT;
	}
	return 1;
}
//时钟的作用
DWORD Get_Clock(void)
{
	//返回时间
	return GetTickCount();
}
DWORD Start_Clock(void)
{
	// 时钟开始运转
	//  Wait_Clock()中使用
	return (clock_scount = Get_Clock());
}
DWORD Wait_Clock(DWORD count)
{

	while (Get_Clock() - clock_scount < count);
	return Get_Clock();
}