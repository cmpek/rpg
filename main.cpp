#include <SFML/Graphics.hpp>
#include "map.h" //подключили код с картой
#include "view.h"//подключили код с видом камеры
#include <iostream>
#include <sstream>
#include "mission.h"

using namespace sf;

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player {
private: float  by = 0;
public:
    float w, h, dx, dy,x,y, speed;
    int dir, playerScore, health;
    bool life;
    String File;
    Image image;
    Texture texture;
    Sprite sprite;
    Player(String F, float X, float Y, float W, float H){
        dir = 0; speed = 0; playerScore = 0; health = 100; dx=0;dy=0;
        life = true;
        File = F;
        w = W; h = H;
        image.loadFromFile("../assets/images/" + File);
        image.createMaskFromColor(Color(41, 33, 59));
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        x = X; y = Y;
        sprite.setTextureRect(IntRect(0, 0, w, h));
        sprite.setOrigin(w / 2, h / 2);
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
        interactionWithMap(time);
        if (health <= 0){ life = false; }

    }

    float getplayercoordinateX(){
        return x;
    }
    float getplayercoordinateY(){
        return y;
    }




    void interactionWithMap(int time)
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

int main()
{


    RenderWindow window(VideoMode(1366, 768), "Lesson 16. kychka-pc.ru");
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

    Player p("hero.png", 250, 250, 96.0, 96.0);

    bool showMissionText = true;

    float currentFrame = 0;
    Clock clock;
    Clock gameTimeClock;
    int gameTime = 0;
    int createObjectForMapTimer = 0;//timer for random
    while (window.isOpen())
    {

        float time = clock.getElapsedTime().asMicroseconds();

        if (p.life) gameTime = gameTimeClock.getElapsedTime().asSeconds(); else  { view.move(0.8, 0); }
        clock.restart();
        time = time / 800;


        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed)
                if ((event.key.code == Keyboard::Tab)) {


                    switch (showMissionText) {

                        case true: {
                            std::ostringstream playerHealthString;
                            playerHealthString << p.health;
                            std::ostringstream task;
                            task << getTextMission(getCurrentMission(p.getplayercoordinateX()));
                            text.setString(L"Здоровье: " + playerHealthString.str() + "\n" + task.str());
                            showMissionText = false;
                            break;
                        }
                        case false: {
                            text.setString("");
                            showMissionText = true;
                            break;
                        }
                    }
                }
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



        sf::Vector2i localPosition = Mouse::getPosition(window);



        if (localPosition.x < 3) { view.move(-0.2*time, 0); }
        if (localPosition.x > window.getSize().x-3) { view.move(0.2*time, 0); }
        if (localPosition.y > window.getSize().y-3) { view.move(0, 0.2*time); }
        if (localPosition.y < 3) {  view.move(0, -0.2*time); }

        createObjectForMapTimer += time;//наращиваем таймер
        if (createObjectForMapTimer>3000){
            randomMapGenerate();//генерация случ камней
            createObjectForMapTimer = 0;//обнуляем таймер
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


        if (!showMissionText) {
            text.setPosition(view.getCenter().x + 125, view.getCenter().y - 130);
            s_quest.setPosition(view.getCenter().x + 115, view.getCenter().y - 130);
            window.draw(s_quest); window.draw(text);
        }

        window.draw(p.sprite);


        window.display();
    }

    return 0;
}