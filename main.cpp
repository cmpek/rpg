#include <SFML/Graphics.hpp>
#include "map.h" //подключили код с картой
#include "view.h"//подключили код с видом камеры
#include <iostream>
#include <sstream>

using namespace sf;

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
private: float x, y;
public:
    float w, h, speed,dx,dy;
    int dir, playerScore, health;//новая переменная, хранящая жизни игрока
    bool life;//переменная жизнь, логическая
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F, float X, float Y, float W, float H){
        dx=0;dy=0;dir = 0; speed = 0; playerScore = 0; health = 100; // инициализировали переменную жизни в конструкторе
        life = true;//инициализировали логическую переменную жизни
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
        if (health <= 0){ life = false; }//если жизней меньше либо равно 0, то умираем
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

                if (TileMap[i][j] == 'f') {
                    health -= 40;//если взяли ядовитейший в мире цветок,то переменная health=health-40;
                    TileMap[i][j] = ' ';//убрали цветок
                }

                if (TileMap[i][j] == 'h') {
                    health += 20;//если взяли сердечко,то переменная health=health+20;
                    TileMap[i][j] = ' ';//убрали сердечко
                }

            }
    }

};

int main()
{
    RenderWindow window(VideoMode(640, 480), "Lesson 13. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));

    Font font;
    font.loadFromFile("../assets/fonts/CyrilicOld.ttf");
    Text text("", font, 20);
    text.setColor(Color::Red);
    text.setStyle(Text::Bold);

    Image map_image;
    map_image.loadFromFile("../assets/images/map.png");
    Texture map;
    map.loadFromImage(map_image);
    Sprite s_map;
    s_map.setTexture(map);


    Player p("hero.png", 250, 250, 96.0, 96.0);


    float currentFrame = 0;
    Clock clock;
    Clock gameTimeClock;//переменная игрового времени, будем здесь хранить время игры
    int gameTime = 0;//объявили игровое время, инициализировали.
    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();

        if (p.life) gameTime = gameTimeClock.getElapsedTime().asSeconds(); else  { view.move(0.8, 0); }//игровое время в секундах идёт вперед, пока жив игрок, перезагружать как time его не надо. оно не обновляет логику игры
        clock.restart();
        time = time / 800;


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        ///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
        if (p.life) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                p.dir = 1; p.speed = 0.1;
                currentFrame += 0.005*time;
                if (currentFrame > 3) currentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 96, 96, 96));
            }

            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                p.dir = 0; p.speed = 0.1;
                currentFrame += 0.005*time;
                if (currentFrame > 3) currentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 192, 96, 96));
            }

            if (Keyboard::isKeyPressed(Keyboard::Up)) {
                p.dir = 3; p.speed = 0.1;
                currentFrame += 0.005*time;
                if (currentFrame > 3) currentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 307, 96, 96));
            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                p.dir = 2; p.speed = 0.1;
                currentFrame += 0.005*time;
                if (currentFrame > 3) currentFrame -= 3;
                p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 0, 96, 96));
            }
            getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
        }
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
                if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));//добавили цветок
                if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));//и сердечко
                s_map.setPosition(j * 32, i * 32);

                window.draw(s_map);
            }
        std::ostringstream playerHealthString, gameTimeString;    // объявили переменную здоровья и времени
        playerHealthString << p.health; gameTimeString << gameTime;		//формируем строку
        text.setString(L"Здоровье: " + playerHealthString.str() + L"\nВремя игры: " + gameTimeString.str());//задаем строку тексту и вызываем сформированную выше строку методом .str()

        text.setPosition(view.getCenter().x - 165, view.getCenter().y - 200);//задаем позицию текста, отступая от центра камеры
        window.draw(text);//рисую этот текст

        window.draw(p.sprite);
        window.display();
    }

    return 0;
}