/* 开发日志
* 1. 创建项目
* 2. 导入素材
* 3. C++开发
* 4. 设计C++项目的模块
     就是设计类
     Block方块 - Tetris游戏
* 5. 设计各个模块的主要接口
* 6. 启动游戏
* 
*/

#include "Tetris.h"

int main(void) {
    Tetris game(20, 10, 263, 143, 35);
    game.play();
    return 0;
}