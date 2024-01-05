#include "Tetris.h"
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include "Block.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

const int SPEED_NORMAL = 500;//ms
const int SPEED_QUICK = 50;

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;

	for (int i = 0; i < rows; i++) {
		vector<int> mapRow;
		for (int j = 0; j < cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
		
	}
}

void Tetris::init()
{
	mciSendString("play res/music.mp3 repeat", 0, 0, 0);

	delay = SPEED_NORMAL;
	srand(time(NULL));

	//创建游戏窗口
	initgraph(938, 896);

	//加载背景图片
	loadimage(&imgBg, "res/background.png");
	//初始化游戏区中的数据
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
		{
			map[i][j] = 0;
		}
	}

	score = 0;

	gameOver = false;
}

void Tetris::play()
{
	init();

	nextBlock = new Block;
	curBlock = nextBlock;
	nextBlock = new Block;

	int timer = 0;
	while (true)
	{
		//接收用户的输入
		keyEvent();
		
		timer += getDelay();
		if (timer > delay) 
		{
			timer = 0;
			drop();
			update = true;
		}

		if (update)
		{
			update = false;
			//渲染游戏画面
			updateWindow();
			//更新游戏数据
			clearLine();
		}
		if (gameOver) {
			//TODO 保存分数
			//游戏结束处理
			displayOver();

			system("pause");
			init(); //重开
		}
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;//0-225
	bool rotateFlag = false;
	int dx = 0;//左右移动偏移量

	if (_kbhit()) {
		ch = _getch();
		//wsad和方向键
		//方向键：上-224，72，  下-224，80，  左-224,75，  右-224，77
		if (ch == 224)
		{
			ch = _getch();
			switch (ch) {
				case 72: 
					rotateFlag = true;
					break;
				case 80:
					delay = SPEED_QUICK;
					break;
				case 75:
					dx += -1;
					break;
				case 77:
					dx += 1;
					break;
				default:
					break;
			}
		}
	}

	if (rotateFlag) {
		//顺时针旋转
		rotate();
		update = true;
	}

	if (dx != 0) {
		//实现左右移动
		moveLeftRight(dx);
		update = true;
	}
	
}

void Tetris::updateWindow()
{
	putimage(-830, -150, &imgBg);//绘制背景图片
	//绘制游戏边框范围
	setlinecolor(YELLOW);
	setlinestyle(PS_SOLID, 2);
	line(leftMargin - 1, topMargin, leftMargin - 1, topMargin + blockSize * rows);
	line(leftMargin + blockSize * cols + 1, topMargin, leftMargin + blockSize * cols + 1, topMargin + blockSize * rows);
	line(leftMargin - 1, topMargin + blockSize * rows + 1, leftMargin + blockSize * cols + 1, topMargin + blockSize * rows + 1);
	//绘制从188，188  到 400，400
	//lineto(400, 400);

	IMAGE** imgs = Block::getImages();
	BeginBatchDraw();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;

			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j]-1]);

		}
	}

	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);

	//绘制分数
	drawScore();

	EndBatchDraw();
}

//第一次调用返回0
//返回距离上一次调用，间隔了多少ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime =  GetTickCount();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	backBlock = *curBlock;
	curBlock->drop();

	if (!curBlock->blockInMap(map)) {
		//恢复上一个方块并固定
		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//检查游戏是否结束
		checkOver();
	}

	delay = SPEED_NORMAL;
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1; //存储数据的行数
	for (int i = rows - 1; i > 0; i--) {
		//检查第i行是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j]; //一边扫描一边存储
		}
		if (count < cols) {//不是满行
			k--;
		}
		else
		{//满行
			lines++;
		}
	}

	if (lines > 0) {
		//计算得分，播放音效

		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/clean.mp3",0 ,0, 0);
		update = true;
	}

}

void Tetris::moveLeftRight(int offset)
{
	//备份原有位置，当超出边界时恢复
	backBlock = *curBlock;
	curBlock->moveLeftRight(offset);
	if (!curBlock->blockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::rotate()
{
	//如果是田字形方块则无需旋转
	if (curBlock->getBlockType() == 6) {
		return;
	}
	backBlock = *curBlock;
	curBlock->rotate();
	if (!curBlock->blockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::drawScore()
{
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 60;
	f.lfWeight = 30;
	f.lfQuality = ANTIALIASED_QUALITY;//消除锯齿
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);
	setbkmode(TRANSPARENT);//背景透明

	outtextxy(670, 727, scoreText);
}

void Tetris::checkOver()
{
	gameOver = !curBlock->blockInMap(map);
}

void Tetris::displayOver()
{
	mciSendString("stop res/music.mp3", 0, 0, 0);
	mciSendString("play res/over.mp3", 0, 0, 0);
}
