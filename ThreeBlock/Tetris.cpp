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

	//������Ϸ����
	initgraph(938, 896);

	//���ر���ͼƬ
	loadimage(&imgBg, "res/background.png");
	//��ʼ����Ϸ���е�����
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
		//�����û�������
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
			//��Ⱦ��Ϸ����
			updateWindow();
			//������Ϸ����
			clearLine();
		}
		if (gameOver) {
			//TODO �������
			//��Ϸ��������
			displayOver();

			system("pause");
			init(); //�ؿ�
		}
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;//0-225
	bool rotateFlag = false;
	int dx = 0;//�����ƶ�ƫ����

	if (_kbhit()) {
		ch = _getch();
		//wsad�ͷ����
		//���������-224��72��  ��-224��80��  ��-224,75��  ��-224��77
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
		//˳ʱ����ת
		rotate();
		update = true;
	}

	if (dx != 0) {
		//ʵ�������ƶ�
		moveLeftRight(dx);
		update = true;
	}
	
}

void Tetris::updateWindow()
{
	putimage(-830, -150, &imgBg);//���Ʊ���ͼƬ
	//������Ϸ�߿�Χ
	setlinecolor(YELLOW);
	setlinestyle(PS_SOLID, 2);
	line(leftMargin - 1, topMargin, leftMargin - 1, topMargin + blockSize * rows);
	line(leftMargin + blockSize * cols + 1, topMargin, leftMargin + blockSize * cols + 1, topMargin + blockSize * rows);
	line(leftMargin - 1, topMargin + blockSize * rows + 1, leftMargin + blockSize * cols + 1, topMargin + blockSize * rows + 1);
	//���ƴ�188��188  �� 400��400
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

	//���Ʒ���
	drawScore();

	EndBatchDraw();
}

//��һ�ε��÷���0
//���ؾ�����һ�ε��ã�����˶���ms
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
		//�ָ���һ�����鲢�̶�
		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		//�����Ϸ�Ƿ����
		checkOver();
	}

	delay = SPEED_NORMAL;
}

void Tetris::clearLine()
{
	int lines = 0;
	int k = rows - 1; //�洢���ݵ�����
	for (int i = rows - 1; i > 0; i--) {
		//����i���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j]; //һ��ɨ��һ�ߴ洢
		}
		if (count < cols) {//��������
			k--;
		}
		else
		{//����
			lines++;
		}
	}

	if (lines > 0) {
		//����÷֣�������Ч

		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/clean.mp3",0 ,0, 0);
		update = true;
	}

}

void Tetris::moveLeftRight(int offset)
{
	//����ԭ��λ�ã��������߽�ʱ�ָ�
	backBlock = *curBlock;
	curBlock->moveLeftRight(offset);
	if (!curBlock->blockInMap(map)) {
		*curBlock = backBlock;
	}
}

void Tetris::rotate()
{
	//����������η�����������ת
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
	f.lfQuality = ANTIALIASED_QUALITY;//�������
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);
	setbkmode(TRANSPARENT);//����͸��

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
