#include <SFML/Graphics.hpp>
#include "map.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include "mission.h"
#include <math.h>

using namespace sf;

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {

public:
    float w,h,dx,dy,x,y,speed;
    int dir, playerScore, health;
    bool life, isMove,isSelect,onGround;//добавили переменные состояния нахождения на земле
    enum { left,right,up,down,jump,stay } state;//добавляем тип перечисления - состояние объекта
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F, float X, float Y, float W, float H){

        dir = 0; speed = 0; playerScore = 0; health = 100; dx = 0; dy = 0;
        life = true; isMove = false; isSelect = false; onGround = false;
        File = F;
        w = W; h = H;
        image.loadFromFile("../assets/images/" + File);
        image.createMaskFromColor(Color(41, 33, 59));
        texture.loadFromImage(image);
        sprite.setTexture(texture);

        x = X; y = Y;
        sprite.setTextureRect(IntRect(0, 134, w, h));
        sprite.setOrigin(w / 2, h / 2);
    }
    void control(){
        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            state = left;
            speed = 0.1;
            //currentFrame += 0.005*time;
            //if (currentFrame > 3) currentFrame -= 3;
            //p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 135, 96, 54));
        }
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            state = right;
            speed = 0.1;
            //	currentFrame += 0.005*time;
            //	if (currentFrame > 3) currentFrame -= 3;
            //	p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 232, 96, 54));
        }

        if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {
            state = jump; dy = -0.4; onGround = false;//то состояние равно прыжок,прыгнули и сообщили, что мы не на земле
            //currentFrame += 0.005*time;
            //if (currentFrame > 3) currentFrame -= 3;
            //p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 307, 96, 96));
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) {
            state = down;
            speed = 0.1;

            //currentFrame += 0.005*time;
            //if (currentFrame > 3) currentFrame -= 3;
            //p.sprite.setTextureRect(IntRect(96 * int(currentFrame), 0, 96, 96));
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
            case down: break;//будет состояние во время спуска персонажа (например по лестнице)
            case jump: break;//здесь может быть вызов анимации
            case stay: break;//и здесь тоже
        }
        x += dx*time;
        checkCollisionWithMap(dx, 0);//обрабатываем столкновение по Х
        y += dy*time;
        checkCollisionWithMap(0, dy);//обрабатываем столкновение по Y
        if (!isMove) speed = 0;
        sprite.setPosition(x+w/2, y+h/2); //задаем позицию спрайта в место его центра
        if (health <= 0){life = false;}
        dy = dy + 0.0015*time;//делаем притяжение к земле
    }

    float getplayercoordinateX(){
        return x;
    }
    float getplayercoordinateY(){
        return y;
    }

    void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
    {
        for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
            for (int j = x / 32; j<(x + w) / 32; j++)
            {
                if (TileMap[i][j] == '0')//если элемент наш тайлик земли? то
                {
                    if (Dy>0){ y = i * 32 - h;  dy = 0; onGround = true; }//по Y вниз=>идем в пол(стоим на месте) или падаем. В этот момент надо вытолкнуть персонажа и поставить его на землю, при этом говорим что мы на земле тем самым снова можем прыгать
                    if (Dy<0){y = i * 32 + 32;  dy = 0;}//столкновение с верхними краями карты(может и не пригодиться)
                    if (Dx>0){x = j * 32 - w; }//с правым краем карты
                    if (Dx<0){x = j * 32 + 32;}// с левым краем карты
                }
            }
    }
};

class SpriteManager{//это задел на следующие уроки,прошу не обращать внимания на эти изменения)
public:
    Image image;
    Texture texture;
    Sprite sprite;
    String name;
    String file;
    int widthOfSprite;
    int heightOfSprite;
    SpriteManager(String File,String Name){
        file = File;
        name = Name;
        image.loadFromFile("../assets/images/" + file);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
    }
};


int main()
{


    RenderWindow window(VideoMode(640, 480), "Lesson 20. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));

    Font font;
    font.loadFromFile("../assets/fonts/CyrilicOld.ttf");
    Text text("", font, 20);
    text.setColor(Color::Black);


    Image map_image;
    map_image.loadFromFile("../assets/images/map.png");
    Texture map;
    map.loadFromImage(map_image);
    Sprite s_map;
    s_map.setTexture(map);

    Image quest_image;
    quest_image.loadFromFile("../assets/images/missionbg.jpg");
    quest_image.createMaskFromColor(Color(0, 0, 0));
    Texture quest_texture;
    quest_texture.loadFromImage(quest_image);
    Sprite s_quest;
    s_quest.setTexture(quest_texture);
    s_quest.setTextureRect(IntRect(0, 0, 340, 510));
    s_quest.setScale(0.6f, 0.6f);

    SpriteManager playerSprite("hero.png", "Hero");//это задел на следующие уроки,прошу не обращать внимания)

    Player p("hero.png", 250, 500, 96, 54);

    float currentFrame = 0;
    Clock clock;
    float dX = 0;
    float dY = 0;
    int tempX = 0;//временная коорд Х.Снимаем ее после нажатия прав клав мыши
    int tempY = 0;//коорд Y
    float distance = 0;//это расстояние от объекта до тыка курсора
    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();

        clock.restart();
        time = time / 800;

        Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
        Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)


        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed)//если нажата клавиша мыши
                if (event.key.code == Mouse::Left){//а именно левая
                    if (p.sprite.getGlobalBounds().contains(pos.x, pos.y))//и при этом координата курсора попадает в спрайт
                    {
                        p.sprite.setColor(Color::Green);//красим спрайт в зеленый,тем самым говоря игроку,что он выбрал персонажа и может сделать ход
                        p.isSelect = true;
                    }
                }


            if (p.isSelect)//если выбрали объект
                if (event.type == Event::MouseButtonPressed)//если нажата клавиша мыши
                    if (event.key.code == Mouse::Right){//а именно правая
                        p.isMove = true;//то начинаем движение
                        p.isSelect = false;//объект уже не выбран
                        p.sprite.setColor(Color::White);//возвращаем обычный цвет спрайту
                        tempX = pos.x;//забираем координату нажатия курсора Х
                        tempY = pos.y;//и Y
                        float dX = pos.x - p.x;//вектор , колинеарный прямой, которая пересекает спрайт и курсор
                        float dY = pos.y - p.y;//он же, координата y
                        float rotation = (atan2(dY, dX)) * 180 / 3.14159265;//получаем угол в радианах и переводим его в градусы
                        std::cout << rotation << "\n";//смотрим на градусы в консольке
                        p.sprite.setRotation(rotation);//поворачиваем спрайт на эти градусы
                    }
        }


        if (p.isMove){
            distance = sqrt((tempX - p.x)*(tempX - p.x) + (tempY - p.y)*(tempY - p.y));//считаем дистанцию (расстояние от точки А до точки Б). используя формулу длины вектора

            if (distance > 2){//этим условием убираем дергание во время конечной позиции спрайта

                p.x += 0.1*time*(tempX - p.x) / distance;//идем по иксу с помощью вектора нормали
                p.y += 0.1*time*(tempY - p.y) / distance;//идем по игреку так же
            }
            else { p.isMove = false; std::cout << "priehali\n"; }//говорим что уже никуда не идем и выводим веселое сообщение в консоль
        }







        ///////////////////////////////////////////Управление персонажем с анимацией////////////////////////////////////////////////////////////////////////
        if (p.life) {getplayercoordinateforview(p.getplayercoordinateX(), p.getplayercoordinateY());
        }


        p.update(time);


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
        window.draw(p.sprite);
        window.display();
    }
    return 0;
}