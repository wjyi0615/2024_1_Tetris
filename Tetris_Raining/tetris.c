#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h> 
#include <stdbool.h>
#include <mmsystem.h>
#include <stdlib.h>
#include "cful.h"
#include "food.h"
#include "block.h"

#pragma comment(lib, "winmm.lib") //����

#define kbhit _kbhit
#define getch _getch
#define boardWidth 30
#define boardHeight 27

clock_t startDropT, endT, startGroundT;
clock_t startSpaceT, endSpaceT;
clock_t startItemT, endItemT;

RECT blockSize;

int Width = 90, Height = 30; // â ����, ���� ����
int x = 8, y = 0;
int blockForm;
int blockRotation = 0;
int key;
int blockNum[7] = { 0 }, blockCnt = 8;
int holdBlockForm;
int colorGauge[7] = { 0 };
int themenum = 0;
int stagenum = 1;
int downspeed = 0;
int Number_Line = 1, Number_Color = 1, Number_Speed = 1, Number_Block = 1;

bool isSpace = false;
bool isHold = false;
bool isHoldAlready = false;
bool isMusic = true;
bool isFirst = true;
bool isThemeClear[3] = { false };
bool isEnter = false;
bool isSlowItem = false;
bool isColor[7][2] = { false };
bool isStageClear = false;
bool isBlock = false;
bool isShowStory = false;

//������ ��� ������ �ܼ�â UI���� block.h�� �س���
int block[7][4][4][4];
int space[1 + 20 + 1][10 + 2];
int UIspace[22][20];
int Themespace[10][10];
int Themespace2[10][10];

void Console_Size(); // �ܼ� ������ ����
void CursorView(char show); // Ŀ�� ������ �����. 0�̸� ����, 1�̸� ����
void DesignMainMenu(); // ���� �޴� ������
int MainMenu(); // ���� �޴�
void MenuTwo(); // ���۹� �޴�
void MenuThree(); // ������ �޴�
void MenuOne(); // ���ӽ��� �޴�
void DrawMap(); // ���� ���¿� ���� �� �׸�
void DrawBlock(); // ���� �� �׸��� (4�����迭)
void DropBlock(); // 0.8�ʸ��� ���� ��ĭ�� ������ ����
void BlockToGround(); // 1�ʵ��� ���� ��������� ������ ������ ������ ����, ������ ���� ����� ���� �ø�
void RemoveLine(); // ������ �� ��ĭ�� ����
void InputKey();
void CreateRandomForm(); // ���� �����ö����� �������� �ٲ�. 0~6
bool CheckCrash(int x, int y); // �浹���� ��ġ�°� ������ true�� ��ȯ
void ShowBlockArrivePosition(); // ���� ���� ���� ��ġ ǥ��
void DrawUI(); // Map ���κ� UI �׸���
void ShowNextBlock(); // ���� �� ǥ��
void HoldFunction(); // �� Ȧ�� ���
void CheckEnding(); // ���� ���� üũ
void Gauge(int line); // ����ġ �Լ�
void DeleteArea(int x1, int y1, int x2, int y2); // ���� ���� ����
void RemoveSelectedLine(); // ���ϴ� �� ����� ������
void RemoveSelectedColor(); // ���ϴ� �� ����� ������
void SelectTheme(); // �׸� ���� �޴�
void SlowFallSpeed(); // �ϰ� �ӵ� �Ͻ��� ���� ������
void DrawGauge(); // ������ ���
void CheckClear(); // �������� Ŭ���� üũ
void SelectBlock(); // ���ϴ� �� ���� ������
void FirstSetting(); // �ʱ� ����
void ShowStory(); // ���丮 �����ֱ�

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; //Ŀ������� �Լ��� ���Ǵ� ������ 
void setcursortype(CURSOR_TYPE c) { //Ŀ������� �Լ� 
	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int main() {
	srand(time(NULL));
	setcursortype(NOCURSOR); //Ŀ�� ����
	SetConsoleTitle(TEXT("NEW TETRIS : �丮�� ���"));
	CursorView(0);  // Ŀ�� ������ �����. 0�̸� ����, 1�̸� ����
	Console_Size(); // �ܼ� ������ ����
	DesignMainMenu(); // ���θ޴� ������ ���

	while (1) // ���� �޴� ����
	{
		int return_n = MainMenu();

		switch (return_n)
		{
		case 0:
			ShowStory();
			break;
		case 3:
			MenuTwo();
			break;
		case 6:
			MenuThree();
			break;
		default:
			exit(1);
		}
	}

	return 0;
}

void Console_Size() // �ܼ� ������ ����
{
	char temp[50];
	sprintf(temp, "Mode con cols=%d lines=%d", Width, Height);
	system(temp);
}

void CursorView(char show)  // Ŀ�� ������ �����. 0�̸� ����, 1�̸� ����
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;

	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void DesignMainMenu() // ���� �޴� ������
{
	printf("\n\n");
	gotoxy(9, 3);
	printf(FG_COLOR(255, 0, 0) "�� �� �� �� ��" RESET);
	gotoxy(13, 4);
	printf(FG_COLOR(255, 0, 0) "��" RESET);
	gotoxy(13, 5);
	printf(FG_COLOR(255, 0, 0) "��" RESET);
	gotoxy(13, 6);
	printf(FG_COLOR(255, 0, 0) "��" RESET);
	gotoxy(13, 7);
	printf(FG_COLOR(255, 0, 0) "��" RESET);

	gotoxy(22, 3);
	printf(FG_COLOR(255, 127, 0) "�� �� ��" RESET);
	gotoxy(22, 4);
	printf(FG_COLOR(255, 127, 0) "��" RESET);
	gotoxy(22, 5);
	printf(FG_COLOR(255, 127, 0) "�� �� ��" RESET);
	gotoxy(22, 6);
	printf(FG_COLOR(255, 127, 0) "��" RESET);
	gotoxy(22, 7);
	printf(FG_COLOR(255, 127, 0) "�� �� ��" RESET);

	gotoxy(32, 3);
	printf(FG_COLOR(255, 255, 0) "�� �� �� �� ��" RESET);
	gotoxy(36, 4);
	printf(FG_COLOR(255, 255, 0) "��" RESET);
	gotoxy(36, 5);
	printf(FG_COLOR(255, 255, 0) "��" RESET);
	gotoxy(36, 6);
	printf(FG_COLOR(255, 255, 0) "��" RESET);
	gotoxy(36, 7);
	printf(FG_COLOR(255, 255, 0) "��" RESET);

	gotoxy(45, 3);
	printf(FG_COLOR(0, 255, 0) "�� �� �� ��" RESET);
	gotoxy(45, 4);
	printf(FG_COLOR(0, 255, 0) "��      ��" RESET);
	gotoxy(45, 5);
	printf(FG_COLOR(0, 255, 0) "�� �� �� ��" RESET);
	gotoxy(45, 6);
	printf(FG_COLOR(0, 255, 0) "��     ��" RESET);
	gotoxy(45, 7);
	printf(FG_COLOR(0, 255, 0) "��      ��" RESET);

	gotoxy(58, 3);
	printf(FG_COLOR(0, 255, 255) "����������" RESET);
	gotoxy(62, 4);
	printf(FG_COLOR(0, 255, 255) "��" RESET);
	gotoxy(62, 5);
	printf(FG_COLOR(0, 255, 255) "��" RESET);
	gotoxy(62, 6);
	printf(FG_COLOR(0, 255, 255) "��" RESET);
	gotoxy(58, 7);
	printf(FG_COLOR(0, 255, 255) "����������" RESET);

	gotoxy(72, 3);
	printf(FG_COLOR(148, 0, 211) "��������" RESET);
	gotoxy(71, 4);
	printf(FG_COLOR(148, 0, 211) "��" RESET);
	gotoxy(72, 5);
	printf(FG_COLOR(148, 0, 211) "��������" RESET);
	gotoxy(79, 6);
	printf(FG_COLOR(148, 0, 211) "��" RESET);
	gotoxy(72, 7);
	printf(FG_COLOR(148, 0, 211) "��������" RESET);

	printf("\n\n\n");
}

int MainMenu()
{
	system("cls");
	DesignMainMenu();

	gotoxy(Width / 2 - 5, Height / 2);
	printf("���� ����");
	gotoxy(Width / 2 - 5, Height / 2 + 3);
	printf("���۹�");
	gotoxy(Width / 2 - 5, Height / 2 + 6);
	printf("������");
	gotoxy(Width / 2 - 5, Height / 2 + 9);
	printf("���� ����");
	gotoxy(Width / 2 + 8, Height / 2);
	printf("��");

	int return_n = 0;
	while (1) // Ű���� ������
	{
		int key;
		if (kbhit()) // Ű���� �Է��� ������ ���
		{
			key = getch(); // �� Ű�� �ƽ�Ű�ڵ尪�� �޾� key�� ����
			if (key == 224 || key == 0) // �� Ű�� ����Ű�� ��� �۵�
			{
				key = getch();
				switch (key)
				{
				case 72: // ���� ����Ű
					gotoxy(Width / 2 + 8, Height / 2 + return_n); //���� �ڸ��� �̵�
					printf("  "); //����
					return_n -= 3; //ȭ��ǥ�� ��ǥ�� ���� 3�����Ű��
					if (return_n < 0) return_n = 0; //���������� ������ ���ϰ�
					gotoxy(Width / 2 + 8, Height / 2 + return_n);
					printf("��"); //�ٲ� ��ǥ�� ����Ű ���
					break;
				case 80: //�Ʒ��� ����Ű�� ���� ���
					gotoxy(Width / 2 + 8, Height / 2 + return_n); //���� �ڸ��� �̵�
					printf("  "); //����
					return_n += 3; //ȭ��ǥ�� ��ǥ�� �Ʒ��� 3�����Ű��
					if (return_n > 6) return_n = 9; //���������� ������ ���ϰ�
					gotoxy(Width / 2 + 8, Height / 2 + return_n);
					printf("��"); //�ٲ� ��ǥ�� ����Ű ���
					break;
				default:
					break;
				}
			}
			else
				if (key == 13 || key == 32) //����Ű�� ������ ��
					return return_n; //ȭ��ǥ�� ��ǥ���� ��ȯ
		}
	}
}

void ShowStory() {
	system("cls");
	if (isShowStory == false) {
		PlaySound(TEXT("op.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		gotoxy(31, 5);
		printf(FG_COLOR(255, 255, 0) "\" �丮���� �ǰ� ���� ��� \"" RESET);
		gotoxy(14, 9);
		printf(FG_COLOR(255, 255, 0) "\" ����� �������� �丮�� �������� �˾Ƴ��� ���� ������ ������. \"" RESET);

		gotoxy(20, 16);
		printf("�����е��� ����� �丮���� �� �� �ֵ��� �����ּ���!");
		gotoxy(14, 19);
		printf("3���� �׸��� �丮 �������� ��� �����Ͽ� �丮���� �Ǳ� �ٶ��ϴ�!");
		isShowStory = true;

		Sleep(1000);
		while (1)
		{
			gotoxy(Width / 2 - 20, Height - 3);
			printf("����Ϸ��� ����Ű / �����̽��ٸ� ��������!");
			if (kbhit()) {
				key = getch();
				if (key == 13 || key == 32) break;
			}
		}
	}

	SelectTheme();
}

void SelectTheme() {
	system("cls");
	gotoxy(Width / 2 - 9, Height / 2 - 12);
	printf("�׸��� �����ϼ���");
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (isThemeClear[0] == false) {
				switch (Themespace[i][j]) {
				case 1:
					gotoxy(j * 2 + 6, i + 10);
					printf("��");
					break;
				case 2:
					gotoxy(j * 2 + 6, i + 10);
					printf("��");
					break;
				}
			}
			else {
				gotoxy(14, 20);
				printf("clear");
				switch (Themespace2[i][j]) {
				case 1:
					gotoxy(j * 2 + 6, i + 10);
					printf(FG_COLOR(255, 0, 0) "��" RESET);
					break;
				case 2:
					gotoxy(j * 2 + 6, i + 10);
					printf(FG_COLOR(255, 0, 0) "��" RESET);
					break;
				}
			}
			if (isThemeClear[1] == false) {
				switch (Themespace[i][j]) {
				case 1:
					gotoxy(j * 2 + 35, i + 10);
					printf("��");
					break;
				case 2:
					gotoxy(j * 2 + 35, i + 10);
					printf("��");
					break;
				}
			}
			else {
				gotoxy(43, 20);
				printf("clear");
				switch (Themespace2[i][j]) {
				case 1:
					gotoxy(j * 2 + 35, i + 10);
					printf(FG_COLOR(255, 255, 0) "��" RESET);
					break;
				case 2:
					gotoxy(j * 2 + 35, i + 10);
					printf(FG_COLOR(255, 255, 0) "��" RESET);
					break;
				}
			}
			if (isThemeClear[2] == false) {
				switch (Themespace[i][j]) {
				case 1:
					gotoxy(j * 2 + 64, i + 10);
					printf("��");
					break;
				case 2:
					gotoxy(j * 2 + 64, i + 10);
					printf("��");
					break;
				}
			}
			else {
				gotoxy(72, 20);
				printf("clear");
				switch (Themespace2[i][j]) {
				case 1:
					gotoxy(j * 2 + 64, i + 10);
					printf(FG_COLOR(0, 255, 0) "��" RESET);
					break;
				case 2:
					gotoxy(j * 2 + 64, i + 10);
					printf(FG_COLOR(0, 255, 0) "��" RESET);
					break;
				}
			}
		}
	}
	gotoxy(13, 8);
	printf("Ƣ���");
	gotoxy(43, 8);
	printf("���");
	gotoxy(69, 8);
	printf("����Ʈ��");

	int return_n = 0;
	gotoxy(15, 22);
	printf("��");
	gotoxy(14, 23);
	printf("����");

	while (1) { // Ű���� ������
		int key;
		if (kbhit()) {
			key = getch();
			if (key == 224 || key == 0) {
				key = getch();
				switch (key) {
				case 75: // ����
					gotoxy(15 + return_n, 22);
					printf("  ");
					gotoxy(14 + return_n, 23);
					printf("    ");
					return_n -= 29;
					if (return_n < 0) return_n = 0;
					gotoxy(15 + return_n, 22);
					printf("��");
					gotoxy(14 + return_n, 23);
					printf("����");
					break;
				case 77: // ������
					gotoxy(15 + return_n, 22);
					printf("  ");
					gotoxy(14 + return_n, 23);
					printf("    ");
					return_n += 29;
					if (return_n > 58) return_n = 58;
					gotoxy(15 + return_n, 22);
					printf("��");
					gotoxy(14 + return_n, 23);
					printf("����");
					break;
				default:
					break;
				}
			}
			else {
				if (key == 13 || key == 32) // ����, �����̽�
				{
					if (return_n == 0) // Ƣ���
						themenum = 1;
					else if (return_n == 29)
						themenum = 2;
					else
						themenum = 3;

					break;
				}
			}
		}
	}

	PlaySound(NULL, 0, 0);
	system("cls");
	MenuOne();
}

void MenuOne() // ���ӽ��� �޴�
{
	if (isMusic == true) PlaySound(TEXT("music.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	system("cls");
	startDropT = clock();
	endSpaceT = clock();
	CreateRandomForm();

	while (1)
	{
		DrawMap();
		ShowNextBlock();
		DrawUI();
		DrawGauge();
		ShowBlockArrivePosition();
		DrawBlock();
		DropBlock();
		BlockToGround();
		RemoveLine();
		InputKey();

		endItemT = clock();
		if ((float)(endItemT - startItemT >= 10000)) {
			isSlowItem = false;
		}

		CheckClear();
	}
}

void MenuTwo() // ���۹� �޴�
{
	system("cls");
	DesignMainMenu();
	gotoxy(Width / 2 - 3, Height / 2 - 6);
	printf("����Ű");
	gotoxy(Width / 2 - 30, Height / 2 - 4);
	printf("��, �� : Move Left, Right");
	gotoxy(Width / 2 - 26, Height / 2 - 2);
	printf("�� : Soft Drop");
	gotoxy(Width / 2 - 29, Height / 2);
	printf("Space : Hard Drop");
	gotoxy(Width / 2 - 26, Height / 2 + 2);
	printf("�� : Rotate");
	gotoxy(Width / 2 - 25, Height / 2 + 4);
	printf("C : Hold");
	gotoxy(Width / 2 + 5, Height / 2 - 4);
	printf("D : Select Line to remove");
	gotoxy(Width / 2 + 5, Height / 2 - 2);
	printf("E : Select Color to remove");
	gotoxy(Width / 2 + 5, Height / 2);
	printf("S : Decrease The Fall Speed");
	gotoxy(Width / 2 + 5, Height / 2 + 2);
	printf("W : Select Next Block");


	gotoxy(Width / 2 - 8, Height / 2 + 7);
	printf("���ư��ðڽ��ϱ�?");
	gotoxy(Width / 2 - 6, Height / 2 + 9);
	printf("��");
	gotoxy(Width / 2 + 1, Height / 2 + 9);
	printf("�ƴϿ�");
	gotoxy(Width / 2 - 3, Height / 2 + 9);
	printf("��");

	int return_n = 0;
	while (1) //Ű���� ������
	{
		int key;
		if (kbhit())
		{
			key = getch();
			if (key == 224 || key == 0)
			{
				key = getch();
				switch (key)
				{
				case 75: //�� �⺻
					gotoxy(Width / 2 - 3 + return_n, Height / 2 + 9);
					printf("  ");
					return_n = 0;
					gotoxy(Width / 2 - 3, Height / 2 + 9);
					printf("��");
					break;
				case 77: //�� 11ĭ��
					return_n = 11;
					gotoxy(Width / 2 + 8 - return_n, Height / 2 + 9);
					printf("  ");
					gotoxy(Width / 2 + 8, Height / 2 + 9);
					printf("��");
					break;
				default:
					break;
				}
			}
			else
				if (key == 13 || key == 32) // ����
				{
					if (return_n == 0)
						break;
					else
						continue;
				}
		}
	}
}

void MenuThree() // ������ �޴�
{
	system("cls");
	DesignMainMenu();
	gotoxy(Width / 2 - 2, Height / 2 - 5);
	printf("����");
	gotoxy(Width / 2 - 13, Height / 2 - 2);
	printf("�߾Ӵ��б� ����Ʈ�����к�");
	gotoxy(Width / 2 - 7, Height / 2 + 1);
	printf("������, �̿���, ���ؿ�");
	gotoxy(Width / 2 - 7, Height / 2 + 3);
	printf("�߾Ӵ��б� �濵�к�");
	gotoxy(Width / 2 - 7, Height / 2 + 5);
	printf("������");

	gotoxy(Width / 2 - 8, Height / 2 + 7);
	printf("���ư��ðڽ��ϱ�?");
	gotoxy(Width / 2 - 6, Height / 2 + 9);
	printf("��");
	gotoxy(Width / 2 + 1, Height / 2 + 9);
	printf("�ƴϿ�");
	gotoxy(Width / 2 - 3, Height / 2 + 9);
	printf("��");

	int return_n = 0;
	while (1) //Ű���� ������
	{
		int key;
		if (kbhit())
		{
			key = getch();
			if (key == 224 || key == 0)
			{
				key = getch();
				switch (key)
				{
				case 75: //�� �⺻
					gotoxy(Width / 2 - 3 + return_n, Height / 2 + 9);
					printf("  ");
					return_n = 0;
					gotoxy(Width / 2 - 3, Height / 2 + 9);
					printf("��");
					break;
				case 77: //�� 11ĭ��
					return_n = 11;
					gotoxy(Width / 2 + 8 - return_n, Height / 2 + 9);
					printf("  ");
					gotoxy(Width / 2 + 8, Height / 2 + 9);
					printf("��");
					break;
				default:
					break;
				}
			}
			else
				if (key == 13 || key == 32)//����
				{
					if (return_n == 0)
						break;
					else
						continue;
				}
		}
	}
}

// ���� �� ����, �������� ������� 3���� ������ ���ϰ� ���� (7bag �ý��� ����)
void CreateRandomForm() { // ���� �� ���� (7bag �ý��� ����)
	int check[7] = { 0 };
	int p = 8;
	for (int i = 0; i < 7; i++) {
		if (blockNum[i] >= 7) {
			p = i;
		}
		else {
			check[blockNum[i]]++;
		}
	}
	for (int i = 0; i < 7; i++) {
		if (check[i] == 0 && p != 8)
			blockNum[p] = i;
	}

	isHoldAlready = false;
	if (blockCnt < 7) {
		blockForm = blockNum[blockCnt++];
		return;
	}
	blockCnt = 0; // ���Ⱑ ����Ǹ� blockCnt�� 7�϶� �̹Ƿ� �ʱ�ȭ

	for (int i = 0; i < 7; i++)
		blockNum[i] = i;

	int i, j;
	int n = 25; // ���� Ƚ��
	while (n--) {
		int tmp;
		i = rand() % 7;
		j = rand() % 7;

		tmp = blockNum[i];
		blockNum[i] = blockNum[j];
		blockNum[j] = tmp;
	}
	CreateRandomForm();
}

void DeleteArea(int x1, int y1, int x2, int y2)
{
	for (int i = x1; i <= x2; i += 2) {
		for (int j = y1; j <= y2; j++) {
			gotoxy(i, j);
			printf("  ");
		}
	}
}

void ShowNextBlock() {	// ���� �� ǥ��
	for (int i = 4; i < 8; i++) {
		for (int j = 1; j < 6; j++) {
			UIspace[i][j] = 0;
		}
	}

	if (blockCnt <= 6) {
		switch (blockNum[blockCnt]) {
		case 0: // T��
			UIspace[5][3] = 2;
			UIspace[6][2] = 2;
			UIspace[6][3] = 2;
			UIspace[6][4] = 2;
			break;
		case 1: // �����ʹ�����
			UIspace[5][3] = 3;
			UIspace[5][4] = 3;
			UIspace[6][2] = 3;
			UIspace[6][3] = 3;
			break;
		case 2: // ���ʹ�����
			UIspace[5][2] = 4;
			UIspace[5][3] = 4;
			UIspace[6][3] = 4;
			UIspace[6][4] = 4;
			break;
		case 3: // I�� ��
			UIspace[6][2] = 5;
			UIspace[6][3] = 5;
			UIspace[6][4] = 5;
			UIspace[6][5] = 5;
			break;
		case 4: // L�� �ݴ��
			UIspace[4][3] = 6;
			UIspace[5][3] = 6;
			UIspace[6][2] = 6;
			UIspace[6][3] = 6;
			break;
		case 5: // L�ں�
			UIspace[4][3] = 7;
			UIspace[5][3] = 7;
			UIspace[6][3] = 7;
			UIspace[6][4] = 7;
			break;
		case 6: // �׸��
			UIspace[5][2] = 8;
			UIspace[5][3] = 8;
			UIspace[6][2] = 8;
			UIspace[6][3] = 8;
			break;
		}
	}
	else {
		UIspace[5][2] = 9;
		UIspace[5][3] = 9;
		UIspace[5][4] = 9;
		UIspace[6][2] = 9;
		UIspace[6][3] = 9;
		UIspace[6][4] = 9;
	}
}

void DrawMap()
{
	gotoxy(10, 1);
	printf(FG_COLOR(255, 0, 0) "�� " RESET);
	printf(FG_COLOR(255, 127, 0) "�� " RESET);
	printf(FG_COLOR(255, 255, 0) "�� " RESET);
	printf(FG_COLOR(0, 255, 0) "�� " RESET);
	printf(FG_COLOR(0, 255, 255) "�� " RESET);
	printf(FG_COLOR(148, 0, 211) "��" RESET);
	gotoxy(12, 4);
	printf(FG_COLOR(255, 0, 0) "T " RESET);
	printf(FG_COLOR(255, 127, 0) "E " RESET);
	printf(FG_COLOR(255, 255, 0) "T " RESET);
	printf(FG_COLOR(0, 255, 0) "R " RESET);
	printf(FG_COLOR(0, 255, 255) "I " RESET);
	printf(FG_COLOR(148, 0, 211) "S" RESET);

	gotoxy(0, 0);
	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 12; j++) {
			switch (space[i][j]) {
			case 1:
				gotoxy(j * 2 + 6, i + 6);
				printf("��");
				break;
			case 2: // blockForm = 0
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(255, 0, 255) "��" RESET); // �����, T�ں�
				break;
			case 3: // blockForm = 1
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(0, 255, 51) "��" RESET); // �ʷϻ�, �����ʹ�����
				break;
			case 4: // blockForm = 2
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(255, 0, 0) "��" RESET); // ������, ���ʹ�����
				break;
			case 5: // blockForm = 3
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(255, 255, 255) "��" RESET); // �Ͼ��, I�� ��
				break;
			case 6: // blockForm = 4
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(150, 75, 0) "��" RESET); // ����, L�ڹݴ��
				break;
			case 7: // blockForm = 5
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(255, 127, 0) "��" RESET); // ��Ȳ��, L�ں�
				break;
			case 8: // blockForm = 6
				gotoxy(j * 2 + 6, i + 6);
				printf(FG_COLOR(255, 255, 0) "��" RESET); // �����, ���ں�
				break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
	}
}

void DrawUI() {
	gotoxy(34, 9);
	printf("NEXT BLOCK");
	gotoxy(35, 20);
	printf("H O L D");

	gotoxy(31, 1);
	printf(FG_COLOR(128, 128, 128) "Ű�Է��� �ȵǸ�" RESET);
	gotoxy(30, 2);
	printf(FG_COLOR(128, 128, 128) "�ѿ�Ű�� ��������" RESET);

	if (isSlowItem == false) {
		if (stagenum == 1) downspeed = 800;
		else if (stagenum == 2) downspeed = 650;
		else if (stagenum == 3) downspeed = 500;
		else downspeed = 350;
	}
	else {
		gotoxy(81, 22);
		printf(FG_COLOR(255, 0, 0) "�����" RESET);
	}

	SET_FG_COLOR(0, 255, 255);
	if (themenum == 1) {
		gotoxy(35, 4);
		printf("Ƣ���");
	}
	else if (themenum == 2) {
		gotoxy(36, 4);
		printf("���");
	}
	else {
		gotoxy(34, 4);
		printf("����Ʈ��");
	}

	SET_FG_COLOR(255, 51, 51);
	gotoxy(35, 6);
	printf("STAGE %d" RESET, stagenum);

	if (Number_Line == 1) {
		gotoxy(50, 18);
		printf(FG_COLOR(255, 255, 0) "D " RESET);
		printf("���� �ı� ������: %d ��", Number_Line);
	}
	else {
		gotoxy(50, 18);
		printf(FG_COLOR(82, 69, 16) "D " RESET);
		SET_FG_COLOR(98, 98, 98);
		printf("���� �ı� ������: %d ��", Number_Line);
	}

	if (Number_Color == 1) {
		gotoxy(50, 20);
		printf(FG_COLOR(255, 255, 0) "E " RESET);
		printf("�� �ı� ������: %d ��", Number_Color);
	}
	else {
		gotoxy(50, 20);
		printf(FG_COLOR(82, 69, 16) "E " RESET);
		SET_FG_COLOR(98, 98, 98);
		printf("�� �ı� ������: %d ��", Number_Color);
	}

	if (Number_Speed == 1) {
		gotoxy(50, 22);
		printf(FG_COLOR(255, 255, 0) "S " RESET);
		printf("��� �ӵ� ���� ������: %d ��", Number_Speed);
	}
	else {
		gotoxy(50, 22);
		printf(FG_COLOR(82, 69, 16) "S " RESET);
		SET_FG_COLOR(98, 98, 98);
		printf("��� �ӵ� ���� ������: %d ��", Number_Speed);
	}

	if (Number_Block == 1) {
		gotoxy(50, 24);
		printf(FG_COLOR(255, 255, 0) "W " RESET);
		printf("��� ���� ������: %d ��", Number_Block);
	}
	else {
		gotoxy(50, 24);
		printf(FG_COLOR(82, 69, 16) "W " RESET);
		SET_FG_COLOR(98, 98, 98);
		printf("��� ���� ������: %d ��", Number_Block);
	}

	SET_FG_COLOR(255, 255, 255);
	gotoxy(50, 27);
	printf("���θ޴��� : ");
	printf(FG_COLOR(255, 127, 0) "Q" RESET);
	if (isMusic == false) {
		gotoxy(70, 27);
		printf("���� On : ");
		printf(FG_COLOR(255, 127, 0) "M" RESET);
	}
	else {
		gotoxy(69, 27);
		printf("���� Off : ");
		printf(FG_COLOR(255, 127, 0) "M" RESET);
	}

	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 20; j++) {
			switch (UIspace[i][j]) {
			case 1:
				gotoxy(j * 2 + 32, i + 8);
				printf("��");
				break;
			case 2: // blockForm = 0
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(255, 0, 255) "��" RESET); // �����, T�ں�
				break;
			case 3: // blockForm = 1
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(0, 255, 51) "��" RESET); // �ʷϻ�, �����ʹ�����
				break;
			case 4: // blockForm = 2
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(255, 0, 0) "��" RESET); // ������, ���ʹ�����
				break;
			case 5: // blockForm = 3
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(255, 255, 255) "��" RESET); // �Ͼ��, I�� ��
				break;
			case 6: // blockForm = 4
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(150, 75, 0) "��" RESET); // ����, L�ڹݴ��
				break;
			case 7: // blockForm = 5
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(255, 127, 0) "��" RESET); // ��Ȳ��, L�ں�
				break;
			case 8: // blockForm = 6
				gotoxy(j * 2 + 32, i + 8);
				printf(FG_COLOR(255, 255, 0) "��" RESET); // �����, ���ں�
				break;
			case 9:
				gotoxy(j * 2 + 32, i + 8);
				printf("?");
				break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
	}
}

void DrawGauge() {
	if (themenum == 1) {
		if (stagenum == 1) {
			isColor[3][0] = true;
			isColor[6][0] = true;
			isColor[5][0] = true;
			if (isColor[3][1] == true && isColor[6][1] == true && isColor[5][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 2) {
			isColor[5][0] = true;
			isColor[3][0] = true;
			isColor[6][0] = true;
			isColor[2][0] = true;
			if (isColor[5][1] == true && isColor[3][1] == true && isColor[6][1] == true && isColor[2][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 3) {
			isColor[5][0] = true;
			isColor[0][0] = true;
			isColor[1][0] = true;
			isColor[6][0] = true;
			isColor[2][0] = true;
			if (isColor[5][1] == true && isColor[0][1] == true && isColor[1][1] == true && isColor[6][1] == true && isColor[2][1] == true) {
				isStageClear = true;
			}
		}
		else {
			isColor[5][0] = true;
			isColor[1][0] = true;
			isColor[0][0] = true;
			isColor[6][0] = true;
			isColor[2][0] = true;
			isColor[3][0] = true;
			if (isColor[5][1] == true && isColor[1][1] == true && isColor[0][1] == true && isColor[6][1] == true && isColor[2][1] == true && isColor[3][1] == true) {
				isStageClear = true;
			}
		}
	}
	else if (themenum == 2) {
		if (stagenum == 1) {
			isColor[3][0] = true;
			isColor[1][0] = true;
			isColor[5][0] = true;
			if (isColor[5][1] == true && isColor[1][1] == true && isColor[3][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 2) {
			isColor[5][0] = true;
			isColor[3][0] = true;
			isColor[6][0] = true;
			isColor[2][0] = true;
			if (isColor[5][1] == true && isColor[3][1] == true && isColor[0][1] == true && isColor[6][1] == true && isColor[2][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 3) {
			isColor[3][0] = true;
			isColor[0][0] = true;
			isColor[5][0] = true;
			isColor[2][0] = true;
			isColor[4][0] = true;
			if (isColor[3][1] == true && isColor[0][1] == true && isColor[5][1] == true && isColor[2][1] == true && isColor[4][1] == true) {
				isStageClear = true;
			}
		}
		else {
			isColor[5][0] = true;
			isColor[1][0] = true;
			isColor[0][0] = true;
			isColor[6][0] = true;
			isColor[4][0] = true;
			isColor[2][0] = true;
			if (isColor[5][1] == true && isColor[1][1] == true && isColor[0][1] == true && isColor[6][1] == true && isColor[2][1] == true && isColor[4][1] == true) {
				isStageClear = true;
			}
		}
	}
	else {
		if (stagenum == 1) {
			isColor[3][0] = true;
			isColor[4][0] = true;
			isColor[2][0] = true;
			if (isColor[3][1] == true && isColor[4][1] == true && isColor[2][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 2) {
			isColor[5][0] = true;
			isColor[3][0] = true;
			isColor[6][0] = true;
			isColor[4][0] = true;
			if (isColor[5][1] == true && isColor[3][1] == true && isColor[6][1] == true && isColor[4][1] == true) {
				isStageClear = true;
			}
		}
		else if (stagenum == 3) {
			isColor[5][0] = true;
			isColor[6][0] = true;
			isColor[2][0] = true;
			isColor[1][0] = true;
			isColor[3][0] = true;
			if (isColor[5][1] == true && isColor[6][1] == true && isColor[2][1] == true && isColor[1][1] == true && isColor[3][1] == true) {
				isStageClear = true;
			}
		}
		else {
			isColor[6][0] = true;
			isColor[3][0] = true;
			isColor[2][0] = true;
			isColor[5][0] = true;
			isColor[1][0] = true;
			isColor[4][0] = true;
			if (isColor[6][1] == true && isColor[3][1] == true && isColor[2][1] == true && isColor[5][1] == true && isColor[1][1] == true && isColor[4][1] == true) {
				isStageClear = true;
			}
		}
	}

	for (int j = 0; j < 7; j++) {
		if (j == 0 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("�����");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(255, 0, 255) "��" RESET);
		}
		else if (j == 1 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("�ʷϻ�");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(0, 255, 51) "��" RESET);
		}
		else if (j == 2 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("������");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(255, 0, 0) "��" RESET);
		}
		else if (j == 3 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("�Ͼ��");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(255, 255, 255) "��" RESET);
		}
		else if (j == 4 && isColor[j][0] == true) {
			gotoxy(52, (j + 1) * 2);
			printf("����");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(150, 75, 0) "��" RESET);
		}
		else if (j == 5 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("��Ȳ��");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(255, 127, 0) "��" RESET);
		}
		else if (j == 6 && isColor[j][0] == true) {
			gotoxy(50, (j + 1) * 2);
			printf("�����");
			for (int i = 0; i < colorGauge[j] / 3; i++)
				printf(FG_COLOR(255, 255, 0) "��" RESET);
		}
	}
}

//��� ���� ����
void DrawBlock()
{
	switch (blockForm) {
	case 0:
		SET_FG_COLOR(255, 0, 255); // �����, T�ں�
		break;
	case 1:
		SET_FG_COLOR(0, 255, 51); // �ʷϻ�, �����ʹ�����
		break;
	case 2:
		SET_FG_COLOR(255, 0, 0); // ������, ���ʹ�����
		break;
	case 3:
		SET_FG_COLOR(255, 255, 255); // �Ͼ��, I�� ��
		break;
	case 4:
		SET_FG_COLOR(150, 75, 0); // ����, L�ڹݴ��
		break;
	case 5:
		SET_FG_COLOR(255, 127, 0); // ��Ȳ��, L�ں�
		break;
	case 6:
		SET_FG_COLOR(255, 255, 0); // �����, ���ں�
		break;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[blockForm][blockRotation][i][j] == 1) {
				gotoxy(x + j * 2 + 6, y + i + 6);
				printf("��");
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

// 0.8�ʸ��� ���� ��ĭ�� ������ ����
void DropBlock() // 0.8�ʸ��� ���� ��ĭ�� ������ ����
{
	endT = clock();
	if ((float)(endT - startDropT) >= downspeed) {
		if (CheckCrash(x, y + 1) == true) return;
		y++;
		startDropT = clock();
		startGroundT = clock();
		system("cls"); // �Ź� ��������
	}
}

// 1�ʵ��� ���� ��������� ������ ������ ������ ����, ������ ���� ����� ���� �ø�
void BlockToGround() { // 1�ʵ��� ���� ��������� ������ ������ ������ ����, ������ ���� ����� ���� �ø�
	if (CheckCrash(x, y + 1) == true) {
		if ((float)(endT - startGroundT) > 1000 || isSpace == true) {
			// ���� ��� ����
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (block[blockForm][blockRotation][i][j] == 1) {
						switch (blockForm) {
						case 0:
							space[i + y][j + x / 2] = 2;
							break;
						case 1:
							space[i + y][j + x / 2] = 3;
							break;
						case 2:
							space[i + y][j + x / 2] = 4;
							break;
						case 3:
							space[i + y][j + x / 2] = 5;
							break;
						case 4:
							space[i + y][j + x / 2] = 6;
							break;
						case 5:
							space[i + y][j + x / 2] = 7;
							break;
						case 6:
							space[i + y][j + x / 2] = 8;
							break;
						}
					}
				}
			}
			isSpace = false;
			x = 8;
			y = 0;
			CreateRandomForm();
			CheckEnding();
		}
	}
}

// ���� �� á���� ���� �����
void RemoveLine() {
	for (int i = 20; i > 0; i--) {
		int cnt = 0;
		for (int j = 1; j < 11; j++) {
			if (space[i][j] == 2 || space[i][j] == 3 || space[i][j] == 4 || space[i][j] == 5 || space[i][j] == 6 || space[i][j] == 7 || space[i][j] == 8) {
				cnt++;
			}
		}

		if (cnt >= 10) { // ������ �� á����
			Gauge(i);
			for (int j = i; j > 1; j--) {
				for (int k = 1; k < 11; k++) {
					space[j][k] = space[j - 1][k];
				}
			}
		}
	}
}

// ������1
void RemoveSelectedLine() {
	DrawMap();
	DrawUI();
	DrawBlock();
	ShowBlockArrivePosition();

	gotoxy(boardWidth, boardHeight - 1); // boardWidth = 30, boardHeight = 27;
	printf("��");

	gotoxy(81, 18);
	printf(FG_COLOR(255, 0, 0) "�����" RESET);

	int return_n = 0;
	while (1) // Ű���� ������
	{
		int key;
		if (kbhit()) // Ű���� �Է��� ������ ���
		{
			key = getch(); // �� Ű�� �ƽ�Ű�ڵ尪�� �޾� key�� ����
			if (key == 224 || key == 0) // �� Ű�� ����Ű�� ��� �۵�
			{
				key = getch();
				switch (key)
				{
				case 72: // ���� ����Ű
					if (isFirst == true) {
						gotoxy(30, 26);
						printf("  ");
						return_n -= 2;
						gotoxy(30, 25);
						printf("��");
						isFirst = false;
					}
					else {
						gotoxy(boardWidth, boardHeight + return_n); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n -= 1; //ȭ��ǥ�� ��ǥ�� ���� 3�����Ű��
						if (return_n < -(boardHeight - 8)) return_n = -(boardHeight - 8); //���������� ������ ���ϰ�
						gotoxy(boardWidth, boardHeight + return_n);;
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
					}
					break;
				case 80: //�Ʒ��� ����Ű�� ���� ���
					gotoxy(boardWidth, boardHeight + return_n);; //���� �ڸ��� �̵�
					printf("  "); //����
					return_n += 1; //ȭ��ǥ�� ��ǥ�� �Ʒ��� 3�����Ű��
					if (return_n > -1) return_n = -1; //���������� ������ ���ϰ�
					gotoxy(boardWidth, boardHeight + return_n);
					printf("��"); //�ٲ� ��ǥ�� ����Ű ���
					break;
				default:
					break;
				}
			}

			else
				if (key == 13 || key == 32) //����Ű�� ������ ��
				{
					Number_Line = 0;
					if (return_n == 0) {
						int i = 20;
						Gauge(i);
						for (int j = i; j > 1; j--) {
							for (int k = 1; k < 11; k++) {
								space[j][k] = space[j - 1][k];
							}
						}
					}
					if (return_n < 0) {
						int i = 21 + return_n; //return_n -1=ù�� -19=����
						Gauge(i);
						for (int j = i; j > 1; j--) {
							for (int k = 1; k < 11; k++) {
								space[j][k] = space[j - 1][k];
							}
						}
					}
					isFirst = true;
					break;
				}
		}
	}
}

// ������2
void RemoveSelectedColor() {
	DrawMap();
	DrawUI();
	DrawBlock();
	ShowBlockArrivePosition();

	gotoxy(81, 20);
	printf(FG_COLOR(255, 0, 0) "�����" RESET);

	gotoxy(boardWidth / 3, (boardHeight / 2) - 3); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");
	gotoxy(boardWidth / 3, (boardHeight / 2) - 2); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) - 1); printf("��    ������� ���� ���� �����ϼ����     ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 0); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 1); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 2); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 3); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 4); printf("��                                        ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 5); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");

	/*�� ����ǥ*/
	gotoxy((boardWidth / 2) + 9, (boardHeight / 2) + 1);
	printf("��");
	gotoxy((boardWidth / 2) + 13, (boardHeight / 2) + 1);
	printf(FG_COLOR(255, 0, 255) "��   " RESET); // �����, T�ں�
	gotoxy((boardWidth / 2) + 17, (boardHeight / 2) + 1);
	printf(FG_COLOR(0, 255, 51) "��   " RESET); // �ʷϻ�, �����ʹ�����
	gotoxy((boardWidth / 2) + 21, (boardHeight / 2) + 1);
	printf(FG_COLOR(255, 0, 0) "��   " RESET); // ������, ���ʹ�����
	gotoxy((boardWidth / 2) + 9, (boardHeight / 2) + 3);
	printf(FG_COLOR(255, 255, 255) "��   " RESET); // �Ͼ��, I�� ��
	gotoxy((boardWidth / 2) + 13, (boardHeight / 2) + 3);
	printf(FG_COLOR(150, 75, 0) "��   " RESET); // ����, L�ڹݴ��
	gotoxy((boardWidth / 2) + 17, (boardHeight / 2) + 3);
	printf(FG_COLOR(255, 127, 0) "��   " RESET); // ��Ȳ��, L�ں�
	gotoxy((boardWidth / 2) + 21, (boardHeight / 2) + 3);
	printf(FG_COLOR(255, 255, 0) "��   " RESET); // �����, ���ں�


	gotoxy((boardWidth / 2) + 9, (boardHeight / 2) + 2);
	printf("��");

	int return_n = 0;
	while (1) // Ű���� ������
	{
		int key;
		if (kbhit()) // Ű���� �Է��� ������ ���
		{
			key = getch(); // �� Ű�� �ƽ�Ű�ڵ尪�� �޾� key�� ����
			if (key == 224 || key == 0) // �� Ű�� ����Ű�� ��� �۵�
			{
				key = getch();
				switch (key)
				{
				case 77: // ������ ����Ű
					if (return_n == 0 || return_n == 4 || return_n == 8 || return_n == 12) {
						gotoxy((boardWidth / 2) + 9 + return_n, (boardHeight / 2) + 2); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n += 4; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n > 12) return_n = 12; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 9 + return_n, (boardHeight / 2) + 2);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					if (return_n == 1 || return_n == 5 || return_n == 9 || return_n == 13) {
						gotoxy((boardWidth / 2) + 8 + return_n, (boardHeight / 2) + 4); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n += 4; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n > 13) return_n = 13; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 8 + return_n, (boardHeight / 2) + 4);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}

				case 75: //���� ����Ű�� ���� ���
					if (return_n == 0 || return_n == 4 || return_n == 8 || return_n == 12) {
						gotoxy((boardWidth / 2) + 9 + return_n, (boardHeight / 2) + 2);
						printf("  "); //����
						return_n -= 4; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n < 0) return_n = 0; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 9 + return_n, (boardHeight / 2) + 2);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					if (return_n == 1 || return_n == 5 || return_n == 9 || return_n == 13) {
						gotoxy((boardWidth / 2) + 8 + return_n, (boardHeight / 2) + 4); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n -= 4; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n < 1) return_n = 1; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 8 + return_n, (boardHeight / 2) + 4);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
				case 80: //�Ʒ� ����Ű�� ���� ���
					if (return_n == 0 || return_n == 4 || return_n == 8 || return_n == 12) {
						gotoxy((boardWidth / 2) + 9 + return_n, (boardHeight / 2) + 2);
						printf("  "); //����
						return_n = 1;
						gotoxy((boardWidth / 2) + 9, (boardHeight / 2) + 4);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
				case 72: //�� ����Ű�� ���� ���
					if (return_n == 1 || return_n == 5 || return_n == 9 || return_n == 13) {
						gotoxy((boardWidth / 2) + 8 + return_n, (boardHeight / 2) + 4); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n = 0;
						gotoxy((boardWidth / 2) + 9, (boardHeight / 2) + 2);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
				default:
					break;
				}
			}
			else {
				if (key == 13 || key == 32) {
					isEnter = true;
				}
			}

			if (isEnter == true) {
				Number_Color = 0;
				switch (return_n) {
				case 4:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 2) {
								colorGauge[0]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}
							}
						}
					}
					break;
				case 8:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 3) {
								colorGauge[1]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				case 12:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 4) {
								colorGauge[2]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				case 1:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 5) {
								colorGauge[3]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				case 5:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 6) {
								colorGauge[4]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				case 9:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 7) {
								colorGauge[5]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				case 13:
					for (int i = 20; i > 0; i--) {
						for (int j = 1; j < 11; j++) {
							while (space[i][j] == 8) {
								colorGauge[6]++;
								for (int k = i; k > 1; k--) {
									space[k][j] = space[k - 1][j];
								}

							}
						}
					}
					break;
				default:
					Number_Color = 1;
				}
				isEnter = false;
				break;
			}
		}
	}
}

void Gauge(int line) // ����ġ �Լ� (ĭ �ִ� 16��)
{
	for (int j = 0; j < 10; j++) {
		int color = space[line][j + 1] - 2;
		if (colorGauge[color] >= 36) {
			isColor[color][1] = true;
			continue;
		}
		colorGauge[color]++;
	}
}

void CheckEnding() {
	for (int i = 1; i < 11; i++) {
		if (space[1][i] == 2 || space[1][i] == 3 || space[1][i] == 4 || space[1][i] == 5 || space[1][i] == 6 || space[1][i] == 7 || space[1][i] == 8) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
			gotoxy(Width / 2 - 22, Height / 2 - 7);
			printf("                                         ");
			gotoxy(Width / 2 - 22, Height / 2 - 6);
			printf("   ======================================   \n");
			gotoxy(Width / 2 - 23, Height / 2 - 5);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 - 4);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 - 3);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 - 2);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 - 1);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 + 1);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 23, Height / 2 + 2);
			printf("   ||                                    ||   \n");
			gotoxy(Width / 2 - 22, Height / 2 + 3);
			printf("   ======================================   \n");
			gotoxy(Width / 2 - 22, Height / 2 + 4);
			printf("                                         ");

			gotoxy(Width / 2 - 5, Height / 2 - 4);
			printf("�� �� �� ��");
			gotoxy(Width / 2 - 12, Height / 2);
			printf("�ٽ��ϱ�");
			gotoxy(Width / 2 + 5, Height / 2);
			printf("�����ϱ�");
			gotoxy(Width / 2 - 15, Height / 2);
			printf("��");
			gotoxy(Width / 2 - 3, Height / 2);
			printf("��");

			int return_n = 0;
			while (1) // Ű���� ������
			{
				if (return_n == 0) {
					gotoxy(Width / 2 - 12, Height / 2);
					printf(FG_COLOR(255, 0, 0) "�ٽ��ϱ�" RESET);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					gotoxy(Width / 2 + 5, Height / 2);
					printf("�����ϱ�");
				}
				else {
					gotoxy(Width / 2 + 5, Height / 2);
					printf(FG_COLOR(255, 0, 0) "�����ϱ�" RESET);
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
					gotoxy(Width / 2 - 12, Height / 2);
					printf("�ٽ��ϱ�");
				}

				int key;
				if (kbhit()) // Ű���� �Է��� ������ ���
				{
					key = getch(); // �� Ű�� �ƽ�Ű�ڵ尪�� �޾� key�� ����
					if (key == 224 || key == 0) // �� Ű�� ����Ű�� ��� �۵�
					{
						key = getch();
						switch (key)
						{
						case 75: // ���� ����Ű
							gotoxy(Width / 2 - 15 + return_n, Height / 2);
							printf("  ");
							gotoxy(Width / 2 - 3 + return_n, Height / 2);
							printf("  ");
							return_n -= 17;
							if (return_n < 0) return_n = 0;
							gotoxy(Width / 2 - 15 + return_n, Height / 2);
							printf("��");
							gotoxy(Width / 2 - 3 + return_n, Height / 2);
							printf("��");
							break;
						case 77: // ������ ����Ű
							gotoxy(Width / 2 - 15 + return_n, Height / 2);
							printf("  ");
							gotoxy(Width / 2 - 3 + return_n, Height / 2);
							printf("  ");
							return_n += 17;
							if (return_n > 17) return_n = 17;
							gotoxy(Width / 2 - 15 + return_n, Height / 2);
							printf("��");
							gotoxy(Width / 2 - 3 + return_n, Height / 2);
							printf("��");
							break;
						default:
							break;
						}
					}
					else
						if (key == 13 || key == 32) { // ����Ű�� ������ ��
							if (return_n == 0)
							{
								FirstSetting();
								stagenum = 1;
								PlaySound(NULL, 0, 0);
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
								main();
							}
							else {
								exit(1);
							}
							break;
						}
				}
			}
		}
	}
}

void InputKey() {
	if (kbhit()) {
		key = getch();
		if (key == 224 || key == 0) {
			key = getch();
			int tmpt;
			switch (key) {
			case 72: // up
				tmpt = blockRotation;
				blockRotation++;
				if (blockRotation >= 4) blockRotation = 0;
				if (CheckCrash(x, y) == true) {
					blockRotation = tmpt;
				}
				startGroundT = clock();
				break;
			case 75: // left
				if (CheckCrash(x - 2, y) == false) {
					x -= 2;
					startGroundT = clock();
				}
				break;
			case 77: // right
				if (CheckCrash(x + 2, y) == false) {
					x += 2;
					startGroundT = clock();
				}
				break;
			case 80: // down
				if (CheckCrash(x, y + 1) == false)
					y++;
				break;
			}
		}
		else {
			switch (key) {
			case 32: // space
				endSpaceT = clock();
				if ((float)(endSpaceT - startSpaceT >= 300)) { // �����Է� ����
					while (1) {
						if (CheckCrash(x, y + 1) == false) y++;
						else break;
					}
					isSpace = true;
				}
				startSpaceT = clock();
				break;
			case 67: // C
			case 99: // c
				HoldFunction();
				break;
			case 68: // D
			case 100: // d
				if (Number_Line == 1)
					RemoveSelectedLine();
				break;
			case 69: // E
			case 101: // e
				if (Number_Color == 1)
					RemoveSelectedColor();
				break;
			case 83: // S
			case 115: // s
				if (Number_Speed == 1)
					SlowFallSpeed();
				break;
			case 87: // W
			case 119: // w
				if (Number_Block == 1)
					SelectBlock();
				break;
			case 77: // M
			case 109: // m
				if (isMusic == false) {
					PlaySound(TEXT("music.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
					isMusic = true;
				}
				else {
					PlaySound(NULL, 0, 0);
					isMusic = false;
				}
				break;
			case 81: // Q
			case 113: // q
				FirstSetting();
				stagenum = 1;
				PlaySound(NULL, 0, 0);
				main();
				break;
			}
		}
		system("cls");
	}
}

// �浹���� ��ġ�°� ������ true�� ��ȯ
bool CheckCrash(int x, int y) { // �浹���� ��ġ�°� ������ true�� ��ȯ
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[blockForm][blockRotation][i][j] == 1) {
				int t = space[i + y][j + x / 2];
				if (t == 1 || t == 2 || t == 3 || t == 4 || t == 5 || t == 6 || t == 7 || t == 8) { // ���� ��, ���� ��
					return true;
				}
			}
		}
	}
	return false;
}

// ���� ���� ���� ��ġ ǥ��
void ShowBlockArrivePosition() { // ���� ���� ���� ��ġ ǥ��
	int k = y;
	while (1) {
		if (CheckCrash(x, k + 1) == false) k++;
		else break;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[blockForm][blockRotation][i][j] == 1) {
				gotoxy(x + j * 2 + 6, k + i + 6);
				switch (blockForm) {
				case 0:
					printf(FG_COLOR(255, 0, 255) "��" RESET); // �����, T�ں�
					break;
				case 1:
					printf(FG_COLOR(0, 255, 51) "��" RESET); // �ʷϻ�, �����ʹ�����
					break;
				case 2:
					printf(FG_COLOR(255, 0, 0) "��" RESET); // ������, ���ʹ�����
					break;
				case 3:
					printf(FG_COLOR(255, 255, 255) "��" RESET); // �Ͼ��, I�� ��
					break;
				case 4:
					printf(FG_COLOR(150, 75, 0) "��" RESET); // ����, L�ڹݴ��
					break;
				case 5:
					printf(FG_COLOR(255, 127, 0) "��" RESET); // ��Ȳ��, L�ں�
					break;
				case 6:
					printf(FG_COLOR(255, 255, 0) "��" RESET); // �����, ���ں�
					break;
				}
			}
		}
	}
}

void HoldFunction() { // �� Ȧ�� ���
	if (isHoldAlready == false) {
		for (int i = 15; i < 19; i++) {
			for (int j = 1; j < 6; j++) {
				UIspace[i][j] = 0;
			}
		}

		switch (blockForm) {
		case 0: // T��
			UIspace[16][3] = 2;
			UIspace[17][2] = 2;
			UIspace[17][3] = 2;
			UIspace[17][4] = 2;
			break;
		case 1: // �����ʹ�����
			UIspace[16][3] = 3;
			UIspace[16][4] = 3;
			UIspace[17][2] = 3;
			UIspace[17][3] = 3;
			break;
		case 2: // ���ʹ�����
			UIspace[16][2] = 4;
			UIspace[16][3] = 4;
			UIspace[17][3] = 4;
			UIspace[17][4] = 4;
			break;
		case 3: // I�� ��
			UIspace[17][2] = 5;
			UIspace[17][3] = 5;
			UIspace[17][4] = 5;
			UIspace[17][5] = 5;
			break;
		case 4: // L�� �ݴ��
			UIspace[15][3] = 6;
			UIspace[16][3] = 6;
			UIspace[17][2] = 6;
			UIspace[17][3] = 6;
			break;
		case 5: // L�ں�
			UIspace[15][3] = 7;
			UIspace[16][3] = 7;
			UIspace[17][3] = 7;
			UIspace[17][4] = 7;
			break;
		case 6: // �׸��
			UIspace[16][2] = 8;
			UIspace[16][3] = 8;
			UIspace[17][2] = 8;
			UIspace[17][3] = 8;
			break;
		}

		if (isHold == true) {
			int tmp = blockForm;
			blockForm = holdBlockForm;
			holdBlockForm = tmp;
			isHoldAlready = true;
		}
		else {
			isHold = true;
			holdBlockForm = blockForm;
			CreateRandomForm();
		}
		system("cls");
		x = 8;
		y = 1;
	}
}

void SlowFallSpeed() {
	Number_Speed = 0;
	isSlowItem = true;
	startItemT = clock();
	downspeed = 1300;
}

void SelectBlock() {
	DrawMap();
	DrawUI();
	DrawBlock();
	ShowBlockArrivePosition();

	gotoxy(81, 24);
	printf(FG_COLOR(255, 0, 0) "�����" RESET);

	gotoxy(boardWidth / 3, (boardHeight / 2) - 3); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");
	gotoxy(boardWidth / 3, (boardHeight / 2) - 2); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) - 1); printf("��       �ڿ��ϴ� ����� �������ּ����       ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 0); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 1); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 2); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 3); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 4); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 5); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 6); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 7); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 8); printf("��                                            ��");
	gotoxy(boardWidth / 3, (boardHeight / 2) + 9); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");


	/*�� ����ǥ*/
	gotoxy((boardWidth / 2) + 4, (boardHeight / 2) + 1);
	printf("��");

	// �����, T�ں�
	gotoxy((boardWidth / 2) + 10, (boardHeight / 2) + 1);
	printf(FG_COLOR(255, 0, 255) "����" RESET);
	gotoxy((boardWidth / 2) + 10, (boardHeight / 2) + 2);
	printf(FG_COLOR(255, 0, 255) "  ��  " RESET);

	// �ʷϻ�, �����ʹ�����
	gotoxy((boardWidth / 2) + 18, (boardHeight / 2) + 1);
	printf(FG_COLOR(0, 255, 51) "  ���" RESET);
	gotoxy((boardWidth / 2) + 18, (boardHeight / 2) + 2);
	printf(FG_COLOR(0, 255, 51) "���" RESET);

	// ������, ���ʹ�����
	gotoxy((boardWidth / 2) + 26, (boardHeight / 2) + 1);
	printf(FG_COLOR(255, 0, 0) "���" RESET);
	gotoxy((boardWidth / 2) + 26, (boardHeight / 2) + 2);
	printf(FG_COLOR(255, 0, 0) "  ���" RESET);

	// �Ͼ��, I�� ��
	gotoxy((boardWidth / 2) + 2, (boardHeight / 2) + 5);
	printf(FG_COLOR(255, 255, 255) "�����" RESET);

	// ����, L�ڹݴ��
	gotoxy((boardWidth / 2) + 12, (boardHeight / 2) + 5);
	printf(FG_COLOR(150, 75, 0) "��" RESET);
	gotoxy((boardWidth / 2) + 12, (boardHeight / 2) + 6);
	printf(FG_COLOR(150, 75, 0) "����" RESET);

	// ��Ȳ��, L�ں�
	gotoxy((boardWidth / 2) + 20, (boardHeight / 2) + 5);
	printf(FG_COLOR(255, 127, 0) "    ��" RESET);
	gotoxy((boardWidth / 2) + 20, (boardHeight / 2) + 6);
	printf(FG_COLOR(255, 127, 0) "����" RESET);

	// �����, ���ں�
	gotoxy((boardWidth / 2) + 28, (boardHeight / 2) + 5);
	printf(FG_COLOR(255, 255, 0) "���" RESET);
	gotoxy((boardWidth / 2) + 28, (boardHeight / 2) + 6);
	printf(FG_COLOR(255, 255, 0) "���" RESET);

	gotoxy((boardWidth / 2) + 4, (boardHeight / 2) + 3);
	printf("��");

	int return_n = 0;
	while (1) // Ű���� ������
	{
		int key;
		if (kbhit()) // Ű���� �Է��� ������ ���
		{
			key = getch(); // �� Ű�� �ƽ�Ű�ڵ尪�� �޾� key�� ����
			if (key == 224 || key == 0) // �� Ű�� ����Ű�� ��� �۵�
			{
				key = getch();
				switch (key)
				{
				case 77: // ������ ����Ű
					if (return_n == 0 || return_n == 8 || return_n == 16 || return_n == 24) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 3); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n += 8; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n > 24) return_n = 24; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 3);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					if (return_n == 1 || return_n == 9 || return_n == 17 || return_n == 25) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 7); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n += 8; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n > 25) return_n = 25; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 7);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}

				case 75: //���� ����Ű�� ���� ���
					if (return_n == 0 || return_n == 8 || return_n == 16 || return_n == 24) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 3); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n -= 8; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n < 0) return_n = 0; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 3);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					if (return_n == 1 || return_n == 9 || return_n == 17 || return_n == 25) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 7); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n -= 8; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						if (return_n < 1) return_n = 1; //���������� ������ ���ϰ�
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 7);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
				case 80: //�Ʒ� ����Ű�� ���� ���
					if (return_n == 0 || return_n == 8 || return_n == 16 || return_n == 24) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 3); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n = 1; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						gotoxy((boardWidth / 2) + 5, (boardHeight / 2) + 7);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					else
						break;

				case 72: //�� ����Ű�� ���� ���
					if (return_n == 1 || return_n == 9 || return_n == 17 || return_n == 25) {
						gotoxy((boardWidth / 2) + 4 + return_n, (boardHeight / 2) + 7); //���� �ڸ��� �̵�
						printf("  "); //����
						return_n = 0; //ȭ��ǥ�� ��ǥ�� ���������� 4�����Ű��
						gotoxy((boardWidth / 2) + 4, (boardHeight / 2) + 3);
						printf("��"); //�ٲ� ��ǥ�� ����Ű ���
						break;
					}
					else
						break;
				default:
					break;
				}
			}
			else {
				if (key == 13 || key == 32) {
					isBlock = true;
				}
			}

			if (isBlock == true) {
				if (return_n == 0) {
					isBlock = false;
					break;
				}
				Number_Block = 0;
				x = 8, y = 0;
				switch (return_n) {
				case 8:
					blockForm = 0;
					break;
				case 16:
					blockForm = 1;
					break;
				case 24:
					blockForm = 2;
					break;
				case 1:
					blockForm = 3;
					break;
				case 9:
					blockForm = 4;
					break;
				case 17:
					blockForm = 5;
					break;
				case 25:
					blockForm = 6;
					break;
				}
				isBlock = false;
				break;
			}
		}
	}
}

void CheckClear() {
	if (isStageClear == true) {
		PlaySound(NULL, 0, 0);
		PlaySound(TEXT("effect.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		system("cls");
		if (themenum == 1) {
			if (stagenum == 1) {
				Height = 35;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< ������ >  " RESET);
				printf("������ ȹ�� !!");
				doughnut(20, 5);
			}
			else if (stagenum == 2) {
				Height = 37;
				Console_Size();
				gotoxy(Width / 2 - 11, 2);
				printf(FG_COLOR(255, 255, 0) "< ġŲ >  " RESET);
				printf("������ ȹ�� !!");
				chicken(8, 5);
			}
			else if (stagenum == 3) {
				Height = 42;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< �ֵ��� >  " RESET);
				printf("������ ȹ�� !!");
				hot_dog(12, 5);
			}
			else {
				Height = 39;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< �佺Ʈ >  " RESET);
				printf("������ ȹ�� !!");
				toast(16, 5);
			}
		}
		else if (themenum == 2) {
			if (stagenum == 1) {
				Height = 30;
				Console_Size();
				gotoxy(Width / 2 - 13, 2);
				printf(FG_COLOR(255, 255, 0) "< �����ʹ� >  " RESET);
				printf("������ ȹ�� !!");
				shrimp_sushi(14, 5);
			}
			else if (stagenum == 2) {
				Height = 41;
				Console_Size();
				gotoxy(Width / 2 - 13, 2);
				printf(FG_COLOR(255, 255, 0) "< ������ũ >  " RESET);
				printf("������ ȹ�� !!");
				pancake(10, 5);
			}
			else if (stagenum == 3) {
				Height = 30;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< �ָԹ� >  " RESET);
				printf("������ ȹ�� !!");
				rice_ball(14, 7);
			}
			else {
				Height = 30;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< �ܹ��� >  " RESET);
				printf("������ ȹ�� !!");
				hamburger(28, 5);
			}
		}
		else {
			if (stagenum == 1) {
				Height = 50;
				Console_Size();
				gotoxy(Width / 2 - 11, 2);
				printf(FG_COLOR(255, 255, 0) "< ���� >  " RESET);
				printf("������ ȹ�� !!");
				candy(24, 5);
			}
			else if (stagenum == 2) {
				Height = 32;
				Console_Size();
				gotoxy(Width / 2 - 14, 2);
				printf(FG_COLOR(255, 255, 0) "< ���̽�ũ�� >  " RESET);
				printf("������ ȹ�� !!");
				Ice_cream(33, 5);
			}
			else if (stagenum == 3) {
				Height = 32;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< ��ī�� >  " RESET);
				printf("������ ȹ�� !!");
				macaroon(20, 5);
			}
			else {
				Height = 60;
				Width = 110;
				Console_Size();
				gotoxy(Width / 2 - 12, 2);
				printf(FG_COLOR(255, 255, 0) "< ����ũ >  " RESET);
				printf("������ ȹ�� !!");
				cake(3, 4);
			}
		}

		Sleep(3000);
		while (1)
		{
			gotoxy(Width / 2 - 25, Height - 3);
			printf("���� ���������� ������ ����Ű / �����̽��ٸ� ��������!");
			if (kbhit()) {
				key = getch();
				if (key == 13 || key == 32) break;
			}
		}

		if (stagenum == 4) {
			Height = 60;
			Width = 100;
			Console_Size();
			system("cls");
			isThemeClear[themenum - 1] = true;
			gotoxy(Width / 2 - 12, 2);
			if (themenum == 1) printf(FG_COLOR(255, 255, 0) "< Ƣ��� >  " RESET);
			else if (themenum == 2) printf(FG_COLOR(255, 255, 0) "< ��� >  " RESET);
			else printf(FG_COLOR(255, 255, 0) "< ����Ʈ�� >  " RESET);
			printf("�丮 ������ !!");
			dog_cook(5, 5);

			Sleep(3000);
			while (1)
			{
				gotoxy(Width / 2 - 25, Height - 3);
				printf("����Ϸ��� ����Ű / �����̽��ٸ� ��������!");
				if (kbhit()) {
					key = getch();
					if (key == 13 || key == 32) break;
				}
			}
		}

		FirstSetting();

		Width = 90;
		Height = 30;
		Console_Size();

		PlaySound(NULL, 0, 0);
		if (stagenum == 4) {
			stagenum = 1;
			main();
		}

		stagenum++;
		if (isMusic == true) PlaySound(TEXT("music.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
}

void FirstSetting() {
	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 12; j++) {
			if (i == 0 || i == 21) space[i][j] = 1;
			else if (j == 0 || j == 11) space[i][j] = 1;
			else space[i][j] = 0;
		}
	}

	for (int i = 0; i < 22; i++) {
		for (int j = 0; j < 20; j++) {
			if ((i == 3 || i == 8 || i == 14 || i == 19) && j >= 0 && j <= 6) UIspace[i][j] = 1;
			else if ((i >= 4 && i <= 7 && (j == 0 || j == 6)) || (i >= 15 && i <= 18 && (j == 0 || j == 6))) UIspace[i][j] = 1;
			else UIspace[i][j] = 0;
		}
	}

	isStageClear = false;
	Number_Line = 1, Number_Color = 1, Number_Speed = 1, Number_Block = 1;
	blockCnt = 8;
	CreateRandomForm();
	x = 8, y = 0;
	for (int i = 0; i < 7; i++) colorGauge[i] = 0;

	isSpace = false;
	isHold = false;
	isHoldAlready = false;
	isFirst = true;
	isEnter = false;
	isBlock = false;
	isSlowItem = false;
	for (int i = 0; i < 7; i++) {
		isColor[i][0] = false;
		isColor[i][1] = false;
	}
}


