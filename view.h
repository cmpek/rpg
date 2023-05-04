#include <SFML/Graphics.hpp>
using namespace sf;

sf::View view;//объявили sfml объект "вид", который и является камерой

void setPlayerCoordinateForView(float x, float y) { //функция для считывания координат игрока
    float tempX = x; float tempY = y;//считываем коорд игрока и проверяем их, чтобы убрать края

    if (x < 320) tempX = 320;//убираем из вида левую сторону
    if (x > 320) tempX = 320;//убираем из вида правую сторону
    if (y < 240) tempY = 240;//верхнюю сторону
    if (y > 240) tempY = 240;//нижнюю сторону

    view.setCenter(tempX, tempY); //следим за игроком, передавая его координаты.
}