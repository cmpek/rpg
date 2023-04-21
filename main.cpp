#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include "mission.h"
#include <math.h>

using namespace sf;
////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
    float dx, dy, x, y, speed,moveTimer;//добавили переменную таймер для будущих целей
    int w,h,health;
    bool life, isMove, onGround;
    Texture texture;
    Sprite sprite;
    String name;//враги могут быть разные, мы не будем делать другой класс для различающегося врага.всего лишь различим врагов по имени и дадим каждому свое действие в update в зависимости от имени
    Entity(Image &image, float X, float Y,int W,int H,String Name){
        x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
        speed = 0; health = 100; dx = 0; dy = 0;
        life = true; onGround = false; isMove = false;
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        sprite.setOrigin(w / 2, h / 2);
    }
};
////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player :public Entity {
public:
    enum { left, right, up, down, jump, stay } state;//добавляем тип перечисления - состояние объекта
    int playerScore;//эта переменная может быть только у игрока

    Player(Image &image, float X, float Y,int W,int H,String Name):Entity(image,X,Y,W,H,Name){
        playerScore = 0; state = stay;
        if (name == "Player1"){
            sprite.setTextureRect(IntRect(4, 19, w, h));
        }
    }

    void control(){
        if (Keyboard::isKeyPressed){//если нажата клавиша
            if (Keyboard::isKeyPressed(Keyboard::Left)) {//а именно левая
                state = left; speed = 0.1;
            }
            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                state = right; speed = 0.1;
            }

            if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {//если нажата клавиша вверх и мы на земле, то можем прыгать
                state = jump; dy = -0.6; onGround = false;//увеличил высоту прыжка
            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                state = down;
            }
        }
    }

    void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
    {
        for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
            for (int j = x / 32; j<(x + w) / 32; j++)
            {
                if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
                {
                    if (Dy>0){ y = i * 32 - h;  dy = 0; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
                    if (Dy<0){ y = i * 32 + 32;  dy = 0; }//столкновение с верхними краями карты(может и не пригодиться)
                    if (Dx>0){ x = j * 32 - w; }//с правым краем карты
                    if (Dx<0){ x = j * 32 + 32; }// с левым краем карты
                }
                //else { onGround = false; }//надо убрать т.к мы можем находиться и на другой поверхности или платформе которую разрушит враг
            }
    }

    void update(float time)
    {
        control();//функция управления персонажем
        switch (state)//тут делаются различные действия в зависимости от состояния
        {
            case right: dx = speed;break;//состояние идти вправо
            case left: dx = -speed;break;//состояние идти влево
            case up: break;//будет состояние поднятия наверх (например по лестнице)
            case down: dx = 0; break;//будет состояние во время спуска персонажа (например по лестнице)
            case stay: break;//и здесь тоже
        }
        x += dx*time;
        checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
        y += dy*time;
        checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
        sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
        if (health <= 0){ life = false; }
        if (!isMove){ speed = 0; }
        //if (!onGround) { dy = dy + 0.0015*time; }//убираем и будем всегда притягивать к земле
        if (life) { setPlayerCoordinateForView(x, y); }
        dy = dy + 0.0015*time;//постоянно притягиваемся к земле
    }
};


class Enemy :public Entity{
public:
    Enemy(Image &image, float X, float Y,int W,int H,String Name):Entity(image,X,Y,W,H,Name){
        if (name == "EasyEnemy"){
            sprite.setTextureRect(IntRect(0, 0, w, h));
            dx = 0.1;//даем скорость.этот объект всегда двигается
        }
    }

    void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
    {
        for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
            for (int j = x / 32; j<(x + w) / 32; j++)
            {
                if (TileMap[i][j] == '0')//если элемент наш тайлик земли, то
                {
                    if (Dy>0){ y = i * 32 - h; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
                    if (Dy<0){ y = i * 32 + 32; }//столкновение с верхними краями карты(может и не пригодиться)
                    if (Dx>0){ x = j * 32 - w; dx = -0.1; sprite.scale(-1, 1); }//с правым краем карты
                    if (Dx<0){ x = j * 32 + 32; dx = 0.1; sprite.scale(-1, 1); }// с левым краем карты
                }
            }
    }

    void update(float time)
    {
        if (name == "EasyEnemy"){//для персонажа с таким именем логика будет такой

            //moveTimer += time;if (moveTimer>3000){ dx *= -1; moveTimer = 0; }//меняет направление примерно каждые 3 сек
            checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
            x += dx*time;
            sprite.setPosition(x + w / 2, y + h / 2); //задаем позицию спрайта в место его центра
            if (health <= 0){ life = false; }
        }
    }
};

int main()
{
    RenderWindow window(VideoMode(640, 480), "Lesson 21. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));


    Image map_image;
    map_image.loadFromFile("../assets/images/map.png");
    Texture map;
    map.loadFromImage(map_image);
    Sprite s_map;
    s_map.setTexture(map);

    Image heroImage;
    heroImage.loadFromFile("../assets/images/MilesTailsPrower.gif");

    Image easyEnemyImage;
    easyEnemyImage.loadFromFile("../assets/images/shamaich.png");
    easyEnemyImage.createMaskFromColor(Color(255, 0, 0));//сделали маску по цвету.но лучше изначально иметь прозрачную картинку


    Player p(heroImage, 750, 500,40,30,"Player1");//объект класса игрока
    Enemy easyEnemy(easyEnemyImage, 850, 671,200,97,"EasyEnemy");//простой враг, объект класса врага

    Clock clock;
    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();

        clock.restart();
        time = time / 800;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        p.update(time);// Player update function
        easyEnemy.update(time);//easyEnemy update function
        window.setView(view);
        window.clear();


        for (int i = 0; i < HEIGHT_MAP; i++)
            for (int j = 0; j < WIDTH_MAP; j++)
            {
                if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32));
                if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));
                if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));
                if ((TileMap[i][j] == 'f')) s_map.setTextureRect(IntRect(96, 0, 32, 32));
                if ((TileMap[i][j] == 'h')) s_map.setTextureRect(IntRect(128, 0, 32, 32));
                s_map.setPosition(j * 32, i * 32);

                window.draw(s_map);
            }
        window.draw(easyEnemy.sprite);
        window.draw(p.sprite);
        window.display();
    }
    return 0;
}