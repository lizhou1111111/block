#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	//���캯��
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init();//��ʼ��
	void play();//��ʼ��Ϸ

private:
	void keyEvent();
	void updateWindow();
	//���ؾ�����һ�ε��øú���������˶��ٺ���
	//��һ�ε��øú���������0
	int getDelay();
	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();
	void drawScore();
	void checkOver();
	void displayOver();
	
private:
	int delay;
	//�Ƿ����
	bool update;

	//0: �հף�û�з���
	//5: �ǵ����ֶ���˹���飬��������Ϊ4
	vector<vector<int>> map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;

	Block* curBlock;//��ǰ����
	Block* nextBlock;//Ԥ�淽��
	Block backBlock;//�����½�ʱ�ĺϷ�λ�÷��飬���ڻָ�

	long score; //��ǰ�ķ���

	bool gameOver;//��Ϸ����
};

