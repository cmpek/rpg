#include <SFML/Graphics.hpp>
#include "map.h" //подключили код с картой
#include "view.h"//подключили код с видом камеры
#include <iostream>
#include <sstream>

using namespace sf;

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
private: float x, y ;
public:
    float w, h, dx, dy, speed ;
    int dir, playerScore;//новая переменная, хранящая очки игрока
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F, float X, float Y, float W, float H){
        dir = 0; playerScore = 0;
        dx=0;dy=0;speed=0;
        File = F;
        w = W; h = H;
        image.loadFromFile("../assets/images/" + File);
        image.createMaskFromColor(Color(41, 33, 59));
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        x = X; y = Y;
        sprite.setTextureRect(IntRect(0, 0, w, h));
    }
    void update(float time)
    {
        switch (dir)
        {
            case 0: dx = speed; dy = 0; break;
            case 1: dx = -speed; dy = 0; break;
            case 2: dx = 0; dy = speed; break;
            case 3: dx = 0; dy = -speed; break;

        }

        x += dx*time;
        y += dy*time;

        speed = 0;
        sprite.setPosition(x, y);
        interactionWithMap();
    }

    float getplayercoordinateX(){
        return x;
    }
    float getplayercoordinateY(){
        return y;
    }




    void interactionWithMap()
    {

        for (int i = y / 32; i < (y + h) / 32; i++)
            for (int j = x / 32; j<(x + w) / 32; j++)
            {
                if (TileMap[i][j] == '0')
                {
                    if (dy>0)
                    {
                        y = i * 32 - h;
                    }
                    if (dy<0)
                    {
                        y = i * 32 + 32;
                    }
                    if (dx>0)
                    {
                        x = j * 32 - w;
                    }
                    if (dx < 0)
                    {
                        x = j * 32 + 32;
                    }
                }

                if (TileMap[i][j] == 's') {
                    playerScore++;//если взяли камень, переменная playerScore=playerScore+1;
                    TileMap[i][j] = ' ';
                }
            }
    }

};

int main()
{
    RenderWindow window(VideoMode(640, 480), "Lesson 12. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));

    Font font;//шрифт
    font.loadFromFile("../assets/fonts/CyrilicOld.ttf");//передаем нашему шрифту файл шрифта
    Text text("", font, 20);//создаем объект текст. закидываем в объект текст строку, шрифт, размер шрифта(в пикселях);//сам объект текст (не строка)
    text.setColor(Color::Red);//покрасили текст в красный. если убрать эту строку, то по умолчанию он белый
    text.setStyle(Text::Bold);//жирный текст.

    Image map_image;
    map_image.loadFromFile("../assets/images/map.png");
    Texture map;
    map.loadFromImage(map_image);
    Sprite s_map;
    s_map.setTexture(map);


    Player p("hero.png", 250, 250, 96.0, 96.0);

    float CurrentFrame = 0;
    Clock clock;

    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 800;


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        ///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            p.dir = 1; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 96, 96, 96));
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            p.dir = 0; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 192, 96, 96));
        }

        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            p.dir = 3; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 307, 96, 96));//
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) {
            p.dir = 2; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 0, 96, 96));
        }


        getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
        p.update(time);

        window.setView(view);
        window.clear();

        /////////////////////////////Рисуем карту/////////////////////
        for (int i = 0; i < HEIGHT_MAP; i++)
            for (int j = 0; j < WIDTH_MAP; j++)
            {
                if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
                if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
                if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));

                s_map.setPosition(j * 32, i * 32);

                window.draw(s_map);
            }
        std::ostringstream playerScoreString;    // объявили переменную
        playerScoreString << p.playerScore;		//занесли в нее число очков, то есть формируем строку
        text.setString(L"Собрано камней:" + playerScoreString.str());//задаем строку тексту и вызываем сформированную выше строку методом .str()
        text.setPosition(view.getCenter().x - 165, view.getCenter().y - 200);//задаем позицию текста, отступая от центра камеры
        window.draw(text);//рисую этот текст

        window.draw(p.sprite);
        window.display();
    }

    return 0;
}