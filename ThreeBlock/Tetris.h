#pragma once
#include <vector>
#include <graphics.h>
#include "Block.h"
using namespace std;
class Tetris
{
public:
	//构造函数
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init();//初始化
	void play();//开始游戏

private:
	void keyEvent();
	void updateWindow();
	//返回距离上一次调用该函数，间隔了多少毫秒
	//第一次调用该函数，返回0
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
	//是否更新
	bool update;

	//0: 空白，没有方块
	//5: 是第五种俄罗斯方块，方块类型为4
	vector<vector<int>> map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBg;

	Block* curBlock;//当前方块
	Block* nextBlock;//预告方块
	Block backBlock;//备份下降时的合法位置方块，用于恢复

	long score; //当前的分数

	bool gameOver;//游戏结束
};

