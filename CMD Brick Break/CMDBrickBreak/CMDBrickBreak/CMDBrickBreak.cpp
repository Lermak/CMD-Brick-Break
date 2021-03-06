#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <math.h>

using namespace std;

const int MAP_WIDTH = 100;
const int MAP_HEIGHT = 40;
const int NUM_BRICKS = 100;
struct vector {
	int x = 0;
	int y = 0;
	int speed = 0;
};

struct point
{
	int x = 0;
	int y = 0;

	public: point(){}

	public : point(int initX, int initY)
	{
		x = initX;
		y = initY;
	}
};

struct ball {
	vector moveVector;
	char img = '@';
	point position;

public: ball(point startPos)
{
	position = startPos;
}
};

struct paddle {
	int halfWidth;
	char img = '_';
	vector moveVector;
	point position;

	public: paddle(int halfSize, point startPos)
	{
		position = startPos;
		halfWidth = halfSize;
	}

	public: void checkCollisionWithBall(ball& myBallPtr)
	{
		if (myBallPtr.position.x >= this->position.x - this->halfWidth && myBallPtr.position.x <= this->position.x + this->halfWidth)
		{
			if (this->position.y == myBallPtr.position.y)
			{
				myBallPtr.moveVector.x = 0;
				myBallPtr.moveVector.y = 0;
			}
		}
	}
};

struct brick {
	int halfWidth;
	char imgArr[4] = {' ','~','=','#'};
	int myImgIndex;
	point position;

	public : void checkCollisionWithBall(ball& myBallPtr, int& score)
	{
		if (myImgIndex != 0)
		{
			if (myBallPtr.position.x >= this->position.x - this->halfWidth && myBallPtr.position.x <= this->position.x + this->halfWidth)
			{
				if (this->position.y == myBallPtr.position.y)
				{
					myImgIndex--;
					score += 5;
					myBallPtr.moveVector.x *= -1;
				}
				if (myBallPtr.position.y == this->position.y - 1 || myBallPtr.position.y == this->position.y + 1)
				{
					myImgIndex--;
					score += 5;
					myBallPtr.moveVector.y *= -1;
				}
			}
		}
	}

	public: brick(){}

	public: brick(int halfSize, point startPos, int imgIndex)
	{
		position = startPos;
		halfWidth = halfSize;
		myImgIndex = imgIndex;
	}
};

void moveBall(ball& myBallPtr, brick bricks[], int& score) {
	for (int x = 0; x < myBallPtr.moveVector.speed; ++x)
	{
		myBallPtr.position.y += myBallPtr.moveVector.y;

		if (myBallPtr.position.y == 0)
			myBallPtr.moveVector.y *= -1;

		if (myBallPtr.position.y == MAP_HEIGHT - 1)
		{
			myBallPtr.moveVector.x = 0;
			myBallPtr.moveVector.y = 0;
		}

		for (int y = 0; y < myBallPtr.moveVector.speed; ++y)
		{
			myBallPtr.position.x += myBallPtr.moveVector.x;

			for (int i = 0; i < NUM_BRICKS; ++i)
			{
				bricks[i].checkCollisionWithBall(myBallPtr, score);
			}

			if (myBallPtr.position.x == 0 || myBallPtr.position.x == MAP_WIDTH - 1)
				myBallPtr.moveVector.x *= -1;
		}
	}
}

void movePaddle(paddle& player, char cmd) {

	if (cmd == 'a')
	{
		player.position.x -= 3;
	}
	else if (cmd == 'd')
	{
		player.position.x += 3;
	}
}

void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}


void drawMap(brick bricks[], ball myBall, paddle player) {
	setCursorPosition(0, 0);
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			char output = ' ';
			for(int i  = 0; i < NUM_BRICKS; ++i)
			{
				if (x >= bricks[i].position.x - bricks[i].halfWidth && x <= bricks[i].position.x + bricks[i].halfWidth && y == bricks[i].position.y)
				{
					output = bricks[i].imgArr[bricks[i].myImgIndex]; break;
				}
			}
			if (x == myBall.position.x && y == myBall.position.y)
				output = myBall.img;
			if (x >= player.position.x - player.halfWidth && x <= player.position.x + player.halfWidth && y == player.position.y)
				output = player.img;
			cout << output;
		}
		cout << endl;
	}
}

void setConsoleSize(int x, int y)
{
	HWND console = GetConsoleWindow();
	RECT ConsoleRect;
	GetWindowRect(console, &ConsoleRect);
	MoveWindow(console, ConsoleRect.left, ConsoleRect.top, x, y, TRUE);
}

void ballFollowIfStopped(ball& myBall, paddle player, char cmd)
{
	if (myBall.moveVector.x == 0 && myBall.moveVector.y == 0)
	{
		myBall.position.x = player.position.x;
		myBall.position.y = player.position.y - 1;
		if (cmd == ' ')
		{
			myBall.moveVector.y = -1;
			myBall.moveVector.x = -1;
		}
	}
}

void main()
{
	setConsoleSize(800, 800);
	ball myBall = ball(point(MAP_WIDTH/2, MAP_HEIGHT-6));
	paddle player = paddle(4, point(MAP_WIDTH/2, MAP_HEIGHT - 5));
	brick bricks[100];

	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			bricks[(y*10) + x] = brick(1, point(x*3 + 10, y + 2), 2);
		}
	}

	myBall.moveVector.y = -1;
	myBall.moveVector.x = -1;
	myBall.moveVector.speed = 1;

	int lives = 5;
	int score = 0;
	while (lives > 0)
	{
		char c = NULL;
		if (_kbhit())
		{
			c = _getch();
		}
		movePaddle(player, c);
		player.checkCollisionWithBall(myBall);
		ballFollowIfStopped(myBall, player, c);
		moveBall(myBall, bricks, score);

		if (myBall.position.y == MAP_HEIGHT - 1)
			lives--;

		drawMap(bricks, myBall, player);

		cout << endl << "Lives: ";
		for (int i = 0; i < 5; ++i)
		{
			if(i < lives)
				cout << 'V';
			else cout << ' ';
		}
		cout << "\t\t\t\t\t" << "Score: " << score;
	}
}

