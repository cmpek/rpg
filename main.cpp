#include <SFML/Graphics.hpp>
#include "view.h"
#include <iostream>
#include "mission.h"
#include "iostream"
#include "level.h"
#include <vector>
#include <list>

using namespace sf;

////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
    std::vector<Object> obj;
    float dx, dy, x, y, speed, moveTimer;
    int w, h, health;
    bool life, isMove, onGround;
    Texture texture;
    Sprite sprite;
    String name;

    Entity(Image &image, String Name, float X, float Y, int W, int H) {
        x = X;
        y = Y;
        w = W;
        h = H;
        name = Name;
        moveTimer = 0;
        speed = 0;
        health = 100;
        dx = 0;
        dy = 0;
        life = true;
        onGround = false;
        isMove = false;
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        sprite.setOrigin(w / 2, h / 2);
    }

    FloatRect getRect() {
        return FloatRect(x, y, w, h);
    }

    virtual void update(float time) = 0;
};

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player : public Entity {
public:
    enum {
        left, right, up, down, jump, stay
    } state;
    int playerScore;

    Player(Image &image, String Name, TileMap &lev, float X, float Y, int W, int H) : Entity(image, Name, X, Y, W, H) {
        playerScore = 0;
        state = stay;
        obj = lev.getAllObjects();
        if (name == "Player1") {
            sprite.setTextureRect(IntRect(4, 19, w, h));
        }
    }

    void control() {
        if (Keyboard::isKeyPressed) {
            if (Keyboard::isKeyPressed(Keyboard::Left)) {
                state = left;
                speed = 0.1;
            }
            if (Keyboard::isKeyPressed(Keyboard::Right)) {
                state = right;
                speed = 0.1;
            }

            if ((Keyboard::isKeyPressed(Keyboard::Up)) && (onGround)) {
                state = jump;
                dy = -0.6;
                onGround = false;
            }

            if (Keyboard::isKeyPressed(Keyboard::Down)) {
                state = down;
            }
        }
    }

    void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
    {
        for (int i = 0; i < obj.size(); i++) {
            if (getRect().intersects(obj[i].rect)) {
                if (obj[i].name == "solid") {
                    if (Dy > 0) {
                        y = obj[i].rect.top - h;
                        dy = 0;
                        onGround = true;
                    }
                    if (Dy < 0) {
                        y = obj[i].rect.top + obj[i].rect.height;
                        dy = 0;
                    }
                    if (Dx > 0) { x = obj[i].rect.left - w; }
                    if (Dx < 0) { x = obj[i].rect.left + obj[i].rect.width; }
                }
            }
        }
    }

    void update(float time) {
        control();
        switch (state) {
            case right:
                dx = speed;
                break;
            case left:
                dx = -speed;
                break;
            case up:
                break;
            case down:
                dx = 0;
                break;
            case stay:
                break;
        }
        x += dx * time;
        checkCollisionWithMap(dx, 0);
        y += dy * time;
        checkCollisionWithMap(0, dy);
        sprite.setPosition(x + w / 2, y + h / 2);
        if (health <= 0) {
            life = false;
        }
        if (!isMove) {
            speed = 0;
        }
        setPlayerCoordinateForView(x, y);
        if (life) {
            setPlayerCoordinateForView(x, y);
        }
        dy = dy + 0.0015 * time;
    }
};


class Enemy : public Entity {
public:
    Enemy(Image &image, String Name, TileMap &lvl, float X, float Y, int W, int H) : Entity(image, Name, X, Y, W, H) {
        obj = lvl.getObjectsByName("solid");
        if (name == "EasyEnemy") {
            sprite.setTextureRect(IntRect(0, 0, w, h));
            dx = 0.1;
        }
    }

    void checkCollisionWithMap(float Dx, float Dy) {
        for (int i = 0; i < obj.size(); i++) {
            if (getRect().intersects(obj[i].rect)) {

                if (Dy > 0) {
                    y = obj[i].rect.top - h;
                    dy = 0;
                    onGround = true;
                }
                if (Dy < 0) {
                    y = obj[i].rect.top + obj[i].rect.height;
                    dy = 0;
                }
                if (Dx > 0) {
                    x = obj[i].rect.left - w;
                    dx = -0.1;
                    sprite.scale(-1, 1);
                }
                if (Dx < 0) {
                    x = obj[i].rect.left + obj[i].rect.width;
                    dx = 0.1;
                    sprite.scale(-1, 1);
                }

            }
        }
    }

    void update(float time) {
        if (name == "EasyEnemy") {
            checkCollisionWithMap(dx, 0);
            x += dx * time;
            sprite.setPosition(x + w / 2, y + h / 2);
            if (health <= 0) {
                life = false;
            }
        }
    }
};

int main() {
    RenderWindow window(VideoMode(640, 480), "Lesson 23. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));

    TileMap lvl;
    lvl.load("../assets/maps/map.tmx");

    Image heroImage;
    heroImage.loadFromFile("../assets/images/player.gif");

    Image easyEnemyImage;
    easyEnemyImage.loadFromFile("../assets/images/easyEnemy.png");
    easyEnemyImage.createMaskFromColor(Color(255, 0, 0));

    std::list<Entity *> entities;//создаю список, сюда буду кидать объекты.например врагов.
    std::list<Entity *>::iterator it;   //итератор чтобы проходить по эл-там списка

    std::vector<Object> e = lvl.getObjectsByName("easyEnemy");//все объекты врага на tmx карте хранятся в этом векторе
    for (int i = 0; i < e.size(); i++) {//проходимся по элементам этого вектора(а именно по врагам)
        entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 132, 64)); //и закидываем в список всех наших врагов с карты
    }

    Object player = lvl.getObject("player");
    Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 40, 30);//объект класса игрока

    Clock clock;
    while (window.isOpen()) {

        float time = clock.getElapsedTime().asMicroseconds();

        clock.restart();
        time = time / 800;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        p.update(time);
        for (auto entity: entities) { //для всех элементов списка(пока это только враги,но могут быть и пули к примеру) активируем ф-цию update
            entity->update(time);
            if (!entity->life) {    // если этот объект мертв, то удаляем его
                continue;
            }
            if (entity->getRect().intersects(p.getRect())) { //если прямоугольник спрайта объекта пересекается с игроком
                if (entity->name == "EasyEnemy") {   //и при этом имя объекта EasyEnemy,то..
                    if (p.dy > 0 && !p.onGround) { //если прыгнули на врага,то даем врагу скорость 0,отпрыгиваем от него чуть вверх,даем ему здоровье 0
                        entity->dx = 0;
                        p.dy = -0.2;
                        entity->health = 0;
                    } else {
                        p.health -= 5;    //иначе враг подошел к нам сбоку и нанес урон
                    }
                }
            }
        }
        window.setView(view);
        window.clear(Color(77, 83, 140));
        window.draw(lvl);

        for (auto entity: entities) {
            if (entity->life) {
                window.draw(entity->sprite); //рисуем entities объекты (сейчас это только враги)
            }
        }
        window.draw(p.sprite);
        window.display();
        for (it = entities.begin(); it != entities.end(); it++) //говорим что проходимся от начала до конца
        {
            Entity *enemy = *it;
            if (!enemy->life) {
                it = entities.erase(it);
                entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, enemy->x + 200, enemy->y, 132, 64));
                delete enemy;
            }
        }
    }
    return 0;
}