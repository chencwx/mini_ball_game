#include "clock.h"//ʵ�����������
#include <stdio.h>
#include <windows.h>
#include"block.h"
#include<mmsystem.h>//��������

/********************************************************************************
* ##########����Windows��������##########
*
* Windows��C���ԵĻ����϶����˺ܶ�Windows���е����͡�����ͨ��C���Թؼ���typedef����ġ�
* Windows���Ͷ���ȫ��д��
*
* DWORD LPSTR WPARAM LPARAM HWND��
* ���У���'H'��ͷ���������Ͷ��Ǿ��*/

#define CLASS_NAME    TEXT("miniײ��С��Ϸ")
#define WIDTH      600
#define HEIGHT        480
//��Ϸ��״̬����
#define GAME_INIT        0
#define GAME_START_LEVEL  1
#define GAME_RUN          2
#define GAME_OVER    3
#define GAME_EXIT        4
//���ο鶨��
#define BLOCK_ROW          6
#define BLOCK_COLUMN      8
#define BLOCK_WIDTH            64
#define BLOCK_HEIGHT            16
#define INIT_X          9
#define INIT_Y          9
#define GAP_X          88
#define GAP_Y            32
// ����״̬
#define BAN_STARTSTATE_X         (WIDTH/2 - 16)
#define BAN_STARTSTATE_Y          (HEIGHT - 32)
#define BAN_WIDTH            80
#define BAN_HEIGHT         8
#define BAN_COLOR            RGB(255,255, 255)
// ��״̬
#define BALL_START_Y            (HEIGHT/2)
#define BALL_SIZE                9
#define BALL_START_X       (WIDTH/2)
// ��ɫ����
#define BACKGROUND_COLOR        RGB(0, 0,0)//������ɫ
#define BLOCK_COLOR                RGB(87, 250, 255)//���ο���ɫ
#define BALL_COLOR                RGB(255, 196, 156)//����ɫ
// ���̶��壨��Ϸʹ�ü��̿��ƣ�������ѯ���ϵ�֪
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
//��������
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);
DWORD Start_Clock(void);
DWORD Wait_Clock(DWORD count);
//�������޷�������
typedef unsigned short UNSHORT;
typedef unsigned char  UNCHAR;
//ȫ�ֱ���
HWND        main_window_handle = NULL;
HINSTANCE    main_instance = NULL;   // HINSTANCE��������ʾ��������ʵ���ľ����ĳЩAPI
//������ʹ�õ����������ʵ������һ���޷��ų�����   
int            game_state = GAME_INIT;    // ��Ϸ��ʼ״̬
int            ban_x = 0, ban_y = 0;                //��Ĺ켣
int            ball_x = 0, ball_y = 0;               //��Ĺ켣
int            ball_dx = 0, ball_dy = 0;                // ����
int            score = 0;                            // ����
int            level = 1;                            // �ȼ�
int            blocks_hit = 0;                            // �������Ĺ켣
DWORD        clock_scount = 0;                // ����clock
  //���ο������
UNCHAR blocks[BLOCK_ROW][BLOCK_COLUMN];//���ƶ�ά����  ��ɫΪ��
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);//typedef�����������
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK MainWndProc(HWND    hwnd,
	UINT    msg,
	WPARAM    wparam,// ��Ϣ��������ͬ����Ϣ�в�ͬ�����壬���MSDN��ÿ����Ϣ���ĵ�
	LPARAM    lparam)
{// ע�⣬��switch-case, ÿ��������������ã�ֻ�����뵽һ��case�С�

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
	return DefWindowProc(hwnd, msg, wparam, lparam);//�����ƶ���Ĭ�ϼ���
}
/* WINMAIN ********************************************************************************/
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASS    wc;//�ṹ��
	// ���ھ����hwnd�����������ڵľ�������������ֻ�õ���һ�����ڡ�
	HWND        hwnd;
	MSG            msg;
	HDC            hdc;
	PAINTSTRUCT    ps;

	/*******************************************************************************
	* ##########   ���� API ����   ##########
	*
	* API�� Application Programming Interface����д������Ӧ�ó����̽ӿڡ�
	* API��Windows����ϵͳ�ṩ��Ӧ�ó�����õ�һϵ�й��ܵĽӿں���������Ӧ�ó�����ò���ϵͳ�ĵײ㹦��
	*
	* API������ʹ�÷�������C���ԵĿ⺯������printf��fopen��fread�ȡ�
	* ��VS������ʹ��C����C++���б���ǣ�ֻ��#include <windows.h>���ɵ��ô󲿷�Windows API������
	*
	* ������һ�˴�����ʹ�õ��� LoadIcon LoadCursor GetStockObject RegisterClass CreateWindow�Ⱥ�������API
	*
	* WindowsӦ�ó��򿪷������е���Ҫ��������ʹ��API��
	*
	****************************************************/
	/*******************************************************************************
	* ##########   ������   ##########
	*
	* �ڴ�������֮ǰ��������Ҫע��һ�������࣬������������Ϊͬһ����Ĵ��ڡ�
	* һ����������Դ���������ڣ�����ͬһ������Ĵ��ھ�����ͬ����Ϣ�����������Ƶ���ʽ��
	*
	* �������C++�е���Ӣ�Ķ���class���������ǲ���ͬһ���
	* C++��ʹ��class�ؼ��ֶ��壬����������ʹ��RegisterClass API������Windowsϵͳע�ᡣ
	*
	* ����� WNDCLASS wc ��ע�ᴰ����� API����RegisterClass�Ĳ�������һ���ṹ�����ͣ�
	* �����洢һ��������ĸ�������
	*
	************************************/
	wc.style = CS_HREDRAW | CS_VREDRAW;//��CS_HREDRAW��һ���ƶ���ߴ����ʹ�ͻ�����
									   //��ȷ����仯�������»��ƴ��ڣ� ��CS_VREDRAW��һ���ƶ���ߴ����ʹ�ͻ����ĸ߶ȷ����仯�������»��ƴ��ڡ�
									   // һ������ָ�룬�������������������Ϣ�� ��� MainWndProc������ע�͡�

	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);
	// Cursor������꣬�������趨����������ʽ��
	// ֱ��ʹ��LoadCursor API����������һ��ϵͳԤ����Ĺ����ʽ������IDC_CROSS,IDC_HAND����ʽ 
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;//���ַ��������

	wc.lpszClassName = CLASS_NAME;
	// register the window class
	//PlaySound(TEXT("E:\\CloudMusic\\Richard Clayderman - ���еĻ���"),
	//	NULL, SND_FILENAME | SND_ASYNC);(���뱳����ʧ�ܣ�
	if (!RegisterClass(&wc))

	{
		// ����ע��ʧ�ܣ���Ϣ����ʾ�����˳���

		return -1;
	}
	// ע��ɹ���������
	hwnd = CreateWindow(
		CLASS_NAME,            // // �����������������Ѿ�ע���˵Ĵ�����
		TEXT("miniײ��С��Ϸ"),    //title-bar string 
		WS_POPUP,                    // ���ڵ�style���ޱ߿���ʽ
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
	* ##########   MSG���ͺ���Ϣ    ##########
	*
	* Windows����Ӧ�ó�����һ����Ϣ�����ĳ����ܡ�
	* Windows����Ӧ�ó��������Ժ������߳̾�һֱ����
	* "�ȴ���Ϣ--��ȡ��Ϣ--ЧӦ��Ϣ" ��ѭ��֮�У�ֱ�������˳���
	*
	* ��ν��Ϣ��Windowsϵͳ���е�һ��ϵͳ����ͻ��ƣ�
	* �ǲ���ϵͳ��ĳЩϵͳ״̬����ʱ�ӣ����û����������������������ϵͳ�жϽ��з�װ�Ժ�
	* ���Է��͵�Ӧ�ó��򣬲���Ӧ�ó�����д�����û�̬����
	*
	* "MSG"����������ʾ"��Ϣ"���������͡� ���ﶨ����һ��MSG���͵ı�����������Ϊmsg
	*
	*****************************************/
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//�ж���Ϣ
		{

			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			////���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);////�������ܣ��ú�������һ����Ϣ�����ڳ���
		}
		// main game processing goes here
		Game_Main();
		Sleep(30);
	}

	Game_Shutdown();//������Ϸ �ͷ��ڴ�

	return (msg.wParam);
}
//��ͼ
int Draw_Rectangle(int x1, int y1, int x2, int y2, int color)
{
	/*����GDI��������������
		* 1��DC��DC��ȫ����Device Context�����Ʋ����ڴ˽��С�����������Ϊһ�黭������Ӧ��Ļ��һ������
		*      GDI�Ļ��Ʋ���������һ��DC�Ͻ��У���˵�һ�����ǵõ�DC�ľ����
		BeginPaint����GetDC������
		* 2��GDI���󣺰���PEN��BRUSH��FONT�ȣ�ÿһ�ֻ��Ʋ�������ʹ
		�õ�һ�����߶��GDI����
		*      ���ԣ��ڻ��Ʋ���֮ǰ�����һ�������ǽ���������
		��GDI�������DC�У�ʹ��SelectObject������ɡ�
		* 3�����Ʋ���������DC��ʹ�ñ�ѡ����DC�е�GDI�������ͼ�Ρ����������ֵȡ�
		*      ��LineTo����������Ellipse����(��)Բ
		��Rectangle�������Ρ�TextOut�������
		*/
	HBRUSH        hbrush;
	HDC            hdc;
	RECT        rect;
	SetRect(&rect, x1, y1, x2, y2);
	hbrush = CreateSolidBrush(color);
	hdc = GetDC(main_window_handle);
	FillRect(hdc, &rect, hbrush);//������
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

	ReleaseDC(main_window_handle, hdc);//���պ��ͷ���Դ
	return 1;
}

//��Ϸ����ʵ��
void Init_Blocks(void)
{
	// ���ο��ʼ��
	for (int row = 0; row < BLOCK_ROW; row++)
		for (int col = 0; col < BLOCK_COLUMN; col++)
			blocks[row][col] = 1;
}
void Draw_Blocks(void)
{
	//�����ο�
	int x1 = INIT_X;
	int y1 = INIT_Y;

	for (int row = 0; row < BLOCK_ROW; row++)
	{
		//���þ��ο�λ��
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
			x1 += GAP_X;        //���¾��ο��λ��
		}
		y1 += GAP_Y;            //����������
	}
}
int Process_Ball(void)
{
	// ����������λ�� �����ײ������
	int x1 = INIT_X;
	int y1 = INIT_Y;
	//�����������
	int ball_cx = ball_x + (BALL_SIZE / 2);
	int ball_cy = ball_y + (BALL_SIZE / 2);
	// �ж����Ƿ��ڵ�����
	if (ball_y > (HEIGHT / 2) && ball_dy > 0)
	{
		//�����ı�Եλ��
		int x = ball_x + (BALL_SIZE / 2);
		int y = ball_y + (BALL_SIZE / 2);

		if ((x >= ban_x && x <= ban_x + BAN_WIDTH) &&
			(y >= ban_y && y <= ban_y + BAN_HEIGHT))
		{
			ball_dy = -ball_dy;        // ������
			ball_y += ball_dy;        // ʹ���뿪����
			if (KEY_DOWN(VK_RIGHT))//�������Ҽ�����
				ball_dx -= rand() % 3;
			else if (KEY_DOWN(VK_LEFT))
				ball_dx += rand() % 3;
			else
				ball_dx += (-1 + rand() % 3);

			MessageBeep(ERROR_SMB_GUEST_LOGON_BLOCKED);//������Ч
			return 0;
		}
	}

	for (int row = 0; row < BLOCK_ROW; row++)
	{
		x1 = INIT_X;
		//ɨ����һ�еľ��ο�
		for (int count = 0; count < BLOCK_COLUMN; count++)
		{
			if (blocks[row][count] != 0)
			{
				//�ж����Ƿ�Խ���߽�
				if ((ball_cx > x1) && (ball_cx < x1 + BLOCK_WIDTH) &&
					(ball_cy > y1) && (ball_cy < y1 + BLOCK_HEIGHT))
				{
					blocks[row][count] = 0;            //�ƶ����ο�
													 //�ж�ʲôʱ������
					blocks_hit++;
					ball_dy = -ball_dy;                // ����              
					ball_dx += (-1 + rand() % 3);
					MessageBeep(MB_OK);                // ��������
				//�ж��Ƿ���Ҫ���о��ο鼴�Ƿ���Ҫ����									  
					if (blocks_hit >= (BLOCK_ROW * BLOCK_COLUMN))
					{
						game_state = GAME_START_LEVEL;
						level++;
					}
					//�ӷ�
					score += 5 * (level + (abs)(ball_dx));
					return 1;
				}
			}
			x1 += GAP_X;        // ����λ��
		}
		y1 += GAP_Y;
	}
	return 0;
}
int Game_Init(void *parms)
{
	//��Ϸ���¿�ʼ����ʼ��
	return 1;
}
int Game_Shutdown(void *parms)
{
	//��Ϸ�������ͷ��ڴ�
	return 1;
}
int Game_Main(void *parms)
{
	//������������
	TCHAR    huan[80];
	BOOL    banMove = FALSE;//���ƶ�
	int       old_ban_x, old_ban_y;
	int        old_ball_x, old_ball_y;
	//�ж���Ϸ״̬
	if (game_state == GAME_INIT)
	{
		//��clockʹÿ����Ϸ����ͬ
		srand((unsigned int)time(0));

		ban_x = BAN_STARTSTATE_X;
		ban_y = BAN_STARTSTATE_Y;
		//�������λ�ü��ٶ�
		ball_x = 8 + rand() % (WIDTH - 16);
		ball_y = BALL_START_Y;
		ball_dx = -4 + rand() % (8 + 1);
		ball_dy = 6 + rand() % 2;
		//תΪ��ʼ״̬
		game_state = GAME_START_LEVEL;
	}
	else if (game_state == GAME_START_LEVEL)
	{
		//��һ���µĵȼ����¿�ʼ��Ϸ
		Init_Blocks();                // ��ʼ�����ο�
		blocks_hit = 0;
		Draw_Blocks();                // �����ο�
									  // ������
		Draw_Rectangle(ban_x, ban_y, ban_x + BAN_WIDTH, ban_y + BAN_HEIGHT, BAN_COLOR);
		game_state = GAME_RUN;// ʹ��Ϸ��ʼ
	}
	else if (game_state == GAME_RUN)
	{
		//�ƶ�����
		if (KEY_DOWN(VK_RIGHT))
		{
			old_ban_x = ban_x;
			old_ban_y = ban_y;
			ban_x += 8;            // ����������
									  // ȷ�������ڿ���
			if (ban_x > (WIDTH - BAN_WIDTH))
				ban_x = WIDTH - BAN_WIDTH;
			banMove = TRUE;
		}
		else if (KEY_DOWN(VK_LEFT))
		{
			old_ban_x = ban_x;
			old_ban_y = ban_y;
			ban_x -= 8;            // ����������

			if (ban_x < 0)
				ban_x = 0;
			banMove = TRUE;
		}
		old_ball_x = ball_x;
		old_ball_y = ball_y;
		// �ƶ���
		ball_x += ball_dx;
		ball_y += ball_dy;
		//�ж����Ƿ��ڿ���
		if (ball_x > (WIDTH - BALL_SIZE) || ball_x < 0)
		{
			ball_dx = -ball_dx;        //������
			ball_x += ball_dx;       //����λ�ã������꣩
		}

		if (ball_y < 0)
		{
			ball_dy = -ball_dy;
			ball_y += ball_dy;
		}
		else if (ball_y > (HEIGHT - BALL_SIZE))
		{
			MessageBeep(MB_DEFAULT_DESKTOP_ONLY);//��Ϸ������ʾ
			MessageBoxA(0, "�󼪴��� ��Ϸ����", "Game over!!!", 0);
			//֮ǰ����ʦ�����е�messagebox ����޷���ʾ���� ����A��ɹ�

			ExitProcess(0);
			//��Ϸ���� ���ڵ���δ�ӵ���

		}
		//ע���������
		if (ball_dx > 8)
			ball_dx = 8;
		else if (ball_dx < -8)
			ball_dx = -8;
		//�ж���ײ����������
		if (Process_Ball())
		{
			Draw_Blocks();
		}
		if (banMove)
		{
			// ���Ǿɵĵ���
			Draw_Rectangle(old_ban_x, old_ban_y, old_ban_x + BAN_WIDTH, old_ban_y + BAN_HEIGHT,
				BACKGROUND_COLOR);

			Draw_Rectangle(ban_x, ban_y, ban_x + BAN_WIDTH, ban_y + BAN_HEIGHT, BAN_COLOR);
		}
		// ���Ǿɵ�ball
		Draw_Rectangle(old_ball_x, old_ball_y, old_ball_x + BALL_SIZE, old_ball_y + BALL_SIZE, BACKGROUND_COLOR);

		Draw_Rectangle(ball_x, ball_y, ball_x + BALL_SIZE, ball_y + BALL_SIZE, BALL_COLOR);

		wsprintf(huan, TEXT("miniײ��С��Ϸ           Score %d              Level %d"), score, level);
		Draw_Rectangle(8, HEIGHT - 26, WIDTH, HEIGHT, BACKGROUND_COLOR);
		DrawText_GUI(huan, 8, HEIGHT - 26, RGB(255, 255, 128));//�±���
		//�ж�����Ƿ�����
		if (KEY_DOWN(VK_ESCAPE))//��ESC����ʾֹͣ��Ϸ
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
//ʱ�ӵ�����
DWORD Get_Clock(void)
{
	//����ʱ��
	return GetTickCount();
}
DWORD Start_Clock(void)
{
	// ʱ�ӿ�ʼ��ת
	//  Wait_Clock()��ʹ��
	return (clock_scount = Get_Clock());
}
DWORD Wait_Clock(DWORD count)
{

	while (Get_Clock() - clock_scount < count);
	return Get_Clock();
}