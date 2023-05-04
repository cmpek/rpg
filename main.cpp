#include "game.h"


int main() {
    int numberLevel = 1;
    RenderWindow window(VideoMode(640, 480), "Lesson 33. kychka-pc.ru");
    gameRunning(window, numberLevel);
    return 0;
}