#include "pch.h"
#include <iostream>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <thread>   //Bakal buat speed ularnya, sleep()
#include <ctime>   //Menghitung FPS time()

using namespace std;

struct Point {
	int X;
	int Y;
public:Point(int X, int Y) {
	this->X = X;
	this->Y = Y;
};
};
struct Size {
	int Witdh;
	int Height;
	public:Size(int Witdh, int Height) {
		this->Height = Height;
		this->Witdh = Witdh;
	};
};
struct Snake {
	Point* Location;
	int Length;
	int Score;
	int Speed;
	vector<Point*> Tail;
};

char LastKey = 'd';
bool InfinityMap;

Size* MAP = new Size(25, 25);
Point* MAP_LOCATION = new Point(2,2);
Point* Makanan;
Snake ular;

vector<vector<int>> MapParent;
bool gameOver;
HANDLE hWnd = GetStdHandle(STD_OUTPUT_HANDLE);

int GetRandomNumber(int min, int max) {
	int dif = max - min;
	return (rand() % (dif + 1)) + min;
}

void DrawMap(vector<vector<int>> Map, Point* Location) {
	COORD pos;
	DWORD wtn;
	Point h = Point(Map.size()+2, Map[0].size() + 2);
	for (int y = 0; y < h.Y; y++) {
		pos.Y = Location->Y + y;
		pos.X = Location->X;
		if (y == 0 || y == h.Y -1) {
			for (int i = 0; i < h.X;i++){
			WORD wColors[2];
			COORD ss = {pos.X + i * 2, pos.Y };
			wColors[0] = 0xF0;
			wColors[1] = 0xF0;
			bool fSuccess = WriteConsoleOutputAttribute(
				hWnd,          // screen buffer handle 
				wColors,          // pointer to source string 
				2,                // length of string 
				ss,            // first cell to write to 
				&wtn);       // actual number written 
			}
		}
		else {
			for (int x = 0; x < h.X; x++) {
				if (x == 0 || x == h.X -1) {
					WORD wColors[2];
					COORD ss = { pos.X + x*2, pos.Y };
					wColors[0] = 0xF0;
					wColors[1] = 0xF0;
					bool fSuccess = WriteConsoleOutputAttribute(
						hWnd,          // screen buffer handle 
						wColors,          // pointer to source string 
						2,                // length of string 
						ss,            // first cell to write to 
						&wtn);       // actual number written 
				}
				else
				{
					if (Map[x-1][y-1] == 1) {
						WORD wColors[2];
						COORD ss = { pos.X + x * 2, pos.Y };
						wColors[0] = 0xC0;
						wColors[1] = 0xC0;
						bool fSuccess = WriteConsoleOutputAttribute(
							hWnd,          // screen buffer handle 
							wColors,          // pointer to source string 
							2,                // length of string 
							ss,            // first cell to write to 
							&wtn);       // actual number written 
					}
					else if (Map[x - 1][y - 1] == 2) {
						WORD wColors[2];
						COORD ss = { pos.X + x * 2, pos.Y };
						wColors[0] = 0x90;
						wColors[1] = 0x90;
						bool fSuccess = WriteConsoleOutputAttribute(
							hWnd,          // screen buffer handle 
							wColors,          // pointer to source string 
							2,                // length of string 
							ss,            // first cell to write to 
							&wtn);       // actual number written 
					}
					else if (Map[x - 1][y - 1] == 3) {
						WORD wColors[2];
						COORD ss = { pos.X + x * 2, pos.Y };
						wColors[0] = 0x10;
						wColors[1] = 0x10;
						bool fSuccess = WriteConsoleOutputAttribute(
							hWnd,          // screen buffer handle 
							wColors,          // pointer to source string 
							2,                // length of string 
							ss,            // first cell to write to 
							&wtn);       // actual number written 
					}
					else
					{
						WORD wColors[2];
						COORD ss = { pos.X + x * 2, pos.Y };
						wColors[0] = 0x0F;
						wColors[1] = 0x0F;
						bool fSuccess = WriteConsoleOutputAttribute(
							hWnd,          // screen buffer handle 
							wColors,          // pointer to source string 
							2,                // length of string 
							ss,            // first cell to write to 
							&wtn);       // actual number written 
					}
				}
			}
		}
	}
}

Point* LastTail;
vector<vector<int>> UlarUpdate(vector<vector<int>> Map) {
	for (int i = 1; i <= ular.Length; i++) {
		if (ular.Tail.size() < i) {
			if (i == 1) {
				ular.Tail.push_back(ular.Location);
				Map[ular.Tail[i - 1]->X][ular.Tail[i - 1]->Y] = 2;
			}
			else
			{
				if (i - 2 >= 0)
				ular.Tail.push_back(LastTail);
				Map[ular.Tail[i - 1]->X][ular.Tail[i - 1]->Y] = 3;
			}
		}
		else {
			if (i == 1) {
				LastTail = ular.Tail[i - 1];
				ular.Tail[i - 1] = ular.Location;
				Map[ular.Tail[i - 1]->X][ular.Tail[i - 1]->Y] = 2;
			}
			else
			{
				if (i - 2 >= 0){
					Point* lastTail = ular.Tail[i - 1];
					ular.Tail[i - 1] = LastTail;
					LastTail = lastTail;
					Map[ular.Tail[i - 1]->X][ular.Tail[i - 1]->Y] = 3;
				}
			}
		}
	}
	return Map;
}

void MakananUpdate() {
	Makanan = new Point(GetRandomNumber(0, MAP->Witdh - 1), GetRandomNumber(0, MAP->Height - 1));
	for (int i = 0; i < ular.Tail.size(); i++) {
		if (ular.Tail[i]->X == Makanan->X && ular.Tail[i]->Y == Makanan->Y) {
			Makanan = new Point(GetRandomNumber(0, MAP->Witdh - 1), GetRandomNumber(0, MAP->Height - 1));
			MakananUpdate();
			break;
		}
	}
}

void Invalidate() {
	clock_t start = clock();
	vector<vector<int>> MapParent(MAP->Witdh, vector<int>(MAP->Height));
	if (_kbhit()) {
		switch (_getch())
		{
		case 'w':
			if (LastKey != 's')
				LastKey = 'w';
			break;
		case 'a':
			if (LastKey != 'd')
				LastKey = 'a';
			break;
		case 's':
			if (LastKey != 'w')
				LastKey = 's';
			break;
		case 'd':
			if (LastKey != 'a')
				LastKey = 'd';
			break;
		case 'f':
			gameOver = true;
			break;
		default:
			break;
		}
	}
	switch (LastKey)
	{
	case 'w':
		ular.Location = new Point(ular.Location->X, ular.Location->Y - 1);
		break;
	case 'a':
		ular.Location = new Point(ular.Location->X - 1, ular.Location->Y);
		break;
	case 's':
		ular.Location = new Point(ular.Location->X, ular.Location->Y + 1);
		break;
	case 'd':
		ular.Location = new Point(ular.Location->X + 1, ular.Location->Y);
		break;
	default:
		break;
	}
	if (InfinityMap) {
		if (ular.Location->X > MAP->Witdh - 1)
			ular.Location = new Point(0, ular.Location->Y);
		if (ular.Location->Y > MAP->Height - 1)
			ular.Location = new Point(ular.Location->X, 0);
		if (ular.Location->X < 0)
			ular.Location = new Point(MAP->Witdh - 1, ular.Location->Y);
		if (ular.Location->Y < 0)
			ular.Location = new Point(ular.Location->X, MAP->Height - 1);
	}
	else
		if (ular.Location->X > MAP->Witdh - 1 || ular.Location->Y > MAP->Height - 1 || ular.Location->X < 0 || ular.Location->Y < 0) {
			gameOver = true;
			return;
		}
	if (ular.Location->X == Makanan->X && ular.Location->Y == Makanan->Y) {
		ular.Length += 1;
		ular.Score += 5;
		MakananUpdate();
	}
	MapParent[Makanan->X][Makanan->Y] = 1;
	MapParent = UlarUpdate(MapParent);
	for (int i = 1; i < ular.Tail.size(); i++) {
		if (ular.Tail[i]->X == ular.Location->X && ular.Tail[i]->Y == ular.Location->Y) {
			gameOver = true;
			return;
		}
	}
	DrawMap(MapParent, MAP_LOCATION);
	COORD s = { MAP_LOCATION->X,MAP_LOCATION->Y + MAP->Height + 2};
	SetConsoleCursorPosition(hWnd, s);
	cout << "Score : " << ular.Score;	
	s.Y++;
	SetConsoleCursorPosition(hWnd, s);
	double FPS = clock() - start;
	cout << "FPS : " << CLOCKS_PER_SEC/FPS << "           ";
	s.Y++;
	SetConsoleCursorPosition(hWnd,s);
}

int main() {	
	SetConsoleTitle(L"Snake Game by R. Vincent");
	ular.Speed = 10;
ulang:
	system("cls");
	cout << "Snake Game by R. Vincent. To move direction press W,A,S,D. To surrender press 'F'.";
	gameOver = false;
	vector<vector<int>> MapParent(MAP->Witdh, vector<int>(MAP->Height));
	LastKey = 'd';
	ular.Tail.clear();
	ular.Length = 1;
	ular.Score = 0;

	ular.Location = new Point(0, 0);
	ular.Tail.push_back(ular.Location);
	MapParent[ular.Location->X][ular.Location->Y] = 2;
	Makanan = new Point(GetRandomNumber(0, MAP->Witdh - 1), GetRandomNumber(0, MAP->Height - 1));
	while (Makanan->X == 0 && Makanan->Y == 0) 
		Makanan = new Point(GetRandomNumber(0, MAP->Witdh - 1), GetRandomNumber(0, MAP->Height - 1));
	MapParent[Makanan->X][Makanan->Y] = 1;
	DrawMap(MapParent, MAP_LOCATION);
	while (!gameOver) {
		Sleep(ular.Speed * 10);
		Invalidate();
	}
	COORD s = { MAP_LOCATION->X,MAP_LOCATION->Y + MAP->Height + 2 };
	SetConsoleCursorPosition(hWnd, s);
	cout << "GAME OVER               ";
	s.Y += 1;
	SetConsoleCursorPosition(hWnd, s);
	cout << "Score : " << ular.Score << "     ";	
	s.Y += 1;
	SetConsoleCursorPosition(hWnd, s);
	cout << "Press 'T' for settings";
	s.Y += 1;
	SetConsoleCursorPosition(hWnd, s);
	cout << "Press 'R' to change map mode";
	char press = _getch();
	if (press == 't') {
		system("cls");
		int x, y,z;
		try1:
		cout << "Set Window Height (min 10): "; cin >> y;
		if (y < 10) {
			cout << "Input Height salah\n";
			goto try1;
		}
		try2:
		cout << "Set Window Witdh (min 10): "; cin >> x;
		if (x < 10) {
			cout << "Invalid Input Height\n";
			goto try2;
		}
		try3:
		cout << "Set snake speed (1-60) : ";
		cin >> z;
		if (z < 0 || z >60) {
			cout << "Invalid Input Speed\n";
			goto try3;
		}
		ular.Speed = 60-z;
		MAP = new Size(x, y);
	}
	else if (press== 'r')
	{
		system("cls");
		if (InfinityMap) {
			cout << "Change mode to infinity map\n";
			InfinityMap = false;
		}
		else
		{
			cout << "Change mode to limited map\n";
			InfinityMap = true;
		}


		cout << "[Press any key to continue]";
		_getch();
	}
	goto ulang;
}