#include <SFML/Graphics.hpp>
#include "map.h" //подключили код с картой
#include "view.h"//подключили код с видом камеры
#include <iostream>
#include <sstream>
#include "mission.h"

using namespace sf;

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
    /* это задел на следующие уроки,прошу не обращать внимания)
private: float w, h, dx, dy, x, y, speed;
         int dir, playerScore, health;
         bool life;
         */
public:
    float w, h, dx, dy,x,y, speed;
    int dir, playerScore, health;
    bool life;
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(Sprite F, float X, float Y, float W, float H){
        dir = 0; speed = 0; playerScore = 0; health = 100; dx=0;dy=0;
        life = true;
        //File = F;
        w = W; h = H;
        //image.loadFromFile("images/" + File);
        image.createMaskFromColor(Color(41, 33, 59));
        //texture.loadFromImage(image);
        //sprite.setTexture(texture);
        sprite = F;
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
        sprite.setOrigin(w / 2, h / 2);
        interactionWithMap();
        if (health <= 0){ life = false; }

    }

    float getWidth(){//получить ширину объека
        return w;
    }
    void setWidth(float width){//установить ширину объекта
        w = width;
    }

    float getHeight(){//взять ширину объекта
        return h;
    }
    void setHeight(float height){//задать ширину объекта
        h = height;
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
                    playerScore++;
                    TileMap[i][j] = ' ';
                }

                if (TileMap[i][j] == 'f') {
                    health -= 40;
                    TileMap[i][j] = ' ';
                }

                if (TileMap[i][j] == 'h') {
                    health += 20;
                    TileMap[i][j] = ' ';
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


    RenderWindow window(VideoMode(640, 480), "Lesson 17. kychka-pc.ru");
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

    Player p(playerSprite.sprite, 250, 250, 96.0, 96.0);

    float currentFrame = 0;
    Clock clock;
    bool isMove = false;//переменная для щелчка мыши по спрайту
    float dX = 0;//корректировка движения по х
    float dY = 0;//по у
    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();

        clock.restart();
        time = time / 800;

        Vector2i pixelPos = Mouse::getPosition(window);//забираем коорд курсора
        Vector2f pos = window.mapPixelToCoords(pixelPos);//переводим их в игровые (уходим от коорд окна)
        std::cout << pixelPos.x << "\n";//смотрим на координату Х позиции курсора в консоли (она не будет больше ширины окна)
        std::cout << pos.x << "\n";//смотрим на Х,которая преобразовалась в мировые координаты

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == Event::MouseButtonPressed)//если нажата клавиша мыши
                if (event.key.code == Mouse::Left)//а именно левая
                    if (p.sprite.getGlobalBounds().contains(pos.x, pos.y))//и при этом координата курсора попадает в спрайт
                    {
                        std::cout << "isClicked!\n";//выводим в консоль сообщение об этом
                        dX = pos.x - p.sprite.getPosition().x;//делаем разность между позицией курсора и спрайта.для корректировки нажатия
                        dY = pos.y - p.sprite.getPosition().y;//тоже самое по игреку
                        isMove = true;//можем двигать спрайт
                    }
            if (event.type == Event::MouseButtonReleased)//если отпустили клавишу
                if (event.key.code == Mouse::Left) //а именно левую
                    isMove = false; //то не можем двигать спрайт
            p.sprite.setColor(Color::White);//и даем ему прежний цвет
        }
        if (isMove) {//если можем двигать
            p.sprite.setColor(Color::Green);//красим спрайт в зеленый
            p.x = pos.x-dX;//двигаем спрайт по Х
            p.y = pos.y-dY;//двигаем по Y
            //p.sprite.setPosition(pos.x - dX, pos.y - dY);//можно и так написать,если у вас нету х и у
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