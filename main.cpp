#include <SFML/Graphics.hpp>
#include <iostream>
#include "map.h" //подключили код с картой

using namespace sf;



////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
public:
    float x, y, w, h, dx, dy, speed ;
    int dir ;
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F, float X, float Y, float W, float H){
        dx=0; dy=0; speed=0; dir = 0;
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
        sprite.setPosition(x,y);
    }
};


int main()
{
    RenderWindow window(sf::VideoMode(640, 480), "Lesson 9. kychka-pc.ru");

    Image map_image;//объект изображения для карты
    map_image.loadFromFile("../assets/images/map.png");//загружаем файл для карты
    Texture map;//текстура карты
    map.loadFromImage(map_image);//заряжаем текстуру картинкой
    Sprite s_map;//создаём спрайт для карты
    s_map.setTexture(map);//заливаем текстуру спрайтом


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
        if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A)))) {
            p.dir = 1; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 96, 96, 96));
        }

        if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D)))) {
            p.dir = 0; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 192, 96, 96));
        }

        if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W)))) {
            p.dir = 3; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 288, 96, 96));

        }

        if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S)))) {
            p.dir = 2; p.speed = 0.1;
            CurrentFrame += 0.005*time;
            if (CurrentFrame > 3) CurrentFrame -= 3;
            p.sprite.setTextureRect(IntRect(96 * int(CurrentFrame), 0, 96, 96));

        }
        p.update(time);


        window.clear();

        /////////////////////////////Рисуем карту/////////////////////
        for (int i = 0; i < HEIGHT_MAP; i++)
            for (int j = 0; j < WIDTH_MAP; j++)
            {
                if (TileMap[i][j] == ' ')  s_map.setTextureRect(IntRect(0, 0, 32, 32)); //если встретили символ пробел, то рисуем 1й квадратик
                if (TileMap[i][j] == 's')  s_map.setTextureRect(IntRect(32, 0, 32, 32));//если встретили символ s, то рисуем 2й квадратик
                if ((TileMap[i][j] == '0')) s_map.setTextureRect(IntRect(64, 0, 32, 32));//если встретили символ 0, то рисуем 3й квадратик


                s_map.setPosition(j * 32, i * 32);//по сути раскидывает квадратики, превращая в карту. то есть задает каждому из них позицию. если убрать, то вся карта нарисуется в одном квадрате 32*32 и мы увидим один квадрат

                window.draw(s_map);//рисуем квадратики на экран
            }


        window.draw(p.sprite);
        window.display();
    }

    return 0;
}