#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "level.h"

using namespace sf;

////////////////////////////////////Общий класс родитель//////////////////////////
class Entity {
public:
    std::vector<Object> obj;
    float dx, dy, x, y, speed, moveTimer;
    int w, h, health;
    bool life, isMove, onGround, showDialogText, showPlayer;;
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
    bool isShoot;

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

            if (Keyboard::isKeyPressed(Keyboard::Space)) {
                isShoot = true;
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
                    if (Dx > 0) {
                        x = obj[i].rect.left - w;
                    }
                    if (Dx < 0) {
                        x = obj[i].rect.left + obj[i].rect.width;
                    }
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
    float timerReversal = 0;

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
                    reversal();
                }
                if (Dx < 0) {
                    x = obj[i].rect.left + obj[i].rect.width;
                    reversal();
                }

            }
        }
    }

    void reversal() {
        sprite.scale(-1, 1);
        dx *= -1;
    }

    void randomRevers(float time) {
        timerReversal += time;
        if (timerReversal > 3200) {
            if (rand() % 2 == 0) {
                reversal();
            }
            timerReversal = 0;
        }
    }

    void update(float time) {

        if (name == "EasyEnemy") {
            if (showPlayer) {
                this->dx = 0.25;
                reversal();
                showPlayer = false;
            } else {
                randomRevers(time);
            }

            checkCollisionWithMap(dx, 0);
            x += dx * time;
            sprite.setPosition(x + w / 2, y + h / 2);
            if (health <= 0) {
                life = false;
            }

        }
    }
};

class MovingPlatform : public Entity {
public:
    MovingPlatform(Image &image, String Name, TileMap &lvl, float X, float Y, int W, int H) : Entity(image, Name, X, Y, W, H) {
        sprite.setTextureRect(IntRect(0, 0, W, H));
        dx = 0.08;
    }

    void update(float time) {
        x += dx * time;
        moveTimer += time;
        if (moveTimer > 2000) {
            dx *= -1;
            moveTimer = 0;
        }
        sprite.setPosition(x + w / 2, y + h / 2);
    }
};

class Bullet : public Entity {//класс пули
public:
    int direction;//направление пули

    //всё так же, только взяли в конце состояние игрока (int dir)
    Bullet(Image &image, String Name, TileMap &lvl, float X, float Y, int W, int H, int dir) : Entity(image, Name, X, Y, W, H) {
        obj = lvl.getObjectsByName("solid");    //инициализируем .получаем нужные объекты для взаимодействия пули с картой
        x = X;
        y = Y;
        direction = dir;
        speed = 0.8;
        w = h = 16;
        life = true;
        //выше инициализация в конструкторе
    }


    void update(float time) {
        switch (direction) {
            case 0: //интовое значение state = left
                dx = -speed;
                dy = 0;
                break;
            case 1: //интовое значение state = right
                dx = speed;
                dy = 0;
                break;
            case 2: //интовое значение state = up
                dx = 0;
                dy = -speed;
                break;
            case 3: //интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
                dx = 0;
                dy = -speed;
                break;
            case 4: //интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
                dx = 0;
                dy = -speed;
                break;
            case 5: //интовое значение не имеющее отношения к направлению, пока просто стрельнем вверх, нам сейчас это не важно
                dx = 0;
                dy = -speed;
                break;
        }

        x += dx * time;//само движение пули по х
        y += dy * time;//по у

        if (x <= 0) { // задержка пули в левой стене, чтобы при проседании кадров она случайно не вылетела за предел карты и не было ошибки
            x = 1;
        }
        if (y <= 0) {
            y = 1;
        }

        for (int i = 0; i < obj.size(); i++) {//проход по объектам solid
            if (getRect().intersects(obj[i].rect)) {//если этот объект столкнулся с пулей,
                life = false;// то пуля умирает
            }
        }

        sprite.setPosition(x + w / 2, y + h / 2);//задается позицию пуле
    }
};
