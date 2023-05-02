#include <SFML/Graphics.hpp>
#include "view.h"
#include <iostream>
#include "mission.h"
#include "iostream"
#include "level.h"
#include <vector>
#include <list>
#include <random>

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
                std::cout << time << "\n";
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

int main() {
    srand(time(NULL));
    RenderWindow window(VideoMode(640, 480), "Lesson 23. kychka-pc.ru");
    view.reset(FloatRect(0, 0, 640, 480));

    Font font;
    font.loadFromFile("../assets/fonts/CyrilicOld.ttf");
    Text text("", font, 20);
    text.setFillColor(Color::White);

    TileMap lvl;
    lvl.load("../assets/maps/map.tmx");

    Image heroImage;
    heroImage.loadFromFile("../assets/images/player.gif");

    Image easyEnemyImage;
    easyEnemyImage.loadFromFile("../assets/images/easyEnemy.png");
    easyEnemyImage.createMaskFromColor(Color(255, 0, 0));

    Image movePlatformImage;
    movePlatformImage.loadFromFile("../assets/images/platform.png");

    Image BulletImage;//изображение для пули
    BulletImage.loadFromFile("../assets/images/bullet.png");//загрузили картинку в объект изображения
    BulletImage.createMaskFromColor(Color(0, 0, 0));//маска для пули по черному цвету

    std::list<Entity *> entities;//создаю список, сюда буду кидать объекты.например врагов.
    std::list<Entity *>::iterator it;   //итератор чтобы проходить по эл-там списка
    std::list<Entity *>::iterator it2;//второй итератор.для взаимодействия между объектами списка

    std::vector<Object> e = lvl.getObjectsByName("easyEnemy");//все объекты врага на tmx карте хранятся в этом векторе
    for (int i = 0; i < e.size(); i++) {//проходимся по элементам этого вектора(а именно по врагам)
        entities.push_back(new Enemy(easyEnemyImage, "EasyEnemy", lvl, e[i].rect.left, e[i].rect.top, 100, 48)); //и закидываем в список всех наших врагов с карты
    }

    Object player = lvl.getObject("player");
    Player p(heroImage, "Player1", lvl, player.rect.left, player.rect.top, 40, 30);//объект класса игрока

    e = lvl.getObjectsByName("MovingPlatform");//забираем все платформы в вектор
    for (int i = 0; i < e.size(); i++) {//закидываем платформу в список.передаем изображение имя уровень координаты появления (взяли из tmx карты), а так же размеры
        entities.push_back(new MovingPlatform(movePlatformImage, "MovingPlatform", lvl, e[i].rect.left, e[i].rect.top, 64, 22));
    }

    Clock clock;
    float dialogAppTimer = 0;
    while (window.isOpen()) {

        float time = clock.getElapsedTime().asMicroseconds();

        clock.restart();
        time = time / 800;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            //если выстрелили, то появляется пуля. enum передаем как int
            if (p.isShoot) {
                p.isShoot = false;
                entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y, 16, 16, p.state));
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y, 16, 16, p.state));
                }
            }
        }
        for (it = entities.begin(); it != entities.end(); it++) //говорим что проходимся от начала до конца
        {
            Entity *entity = *it;
            entity->update(time);
            if (!entity->life) {
                it = entities.erase(it);
                delete entity;
            }
        }
        for (auto entity: entities) { //для всех элементов списка(пока это только враги,но могут быть и пули к примеру) активируем ф-цию update

            if ((entity->name == "MovingPlatform") && (entity->getRect().intersects(p.getRect()))) { //если игрок столкнулся с объектом списка и имя этого объекта movingplatform
                if (p.dy > 0 || p.onGround == false) {//при этом игрок находится в состоянии после прыжка, т.е падает вниз
                    if (p.y + p.h < entity->y + entity->h) {//если игрок находится выше платформы, т.е это его ноги минимум (тк мы уже проверяли что он столкнулся с платформой)
                        p.y = entity->y - p.h + 3;
                        p.x += entity->dx * time;
                        p.dy = 0;
                        p.onGround = true; // то выталкиваем игрока так, чтобы он как бы стоял на платформе
                    }
                }
            }

            if (entity->name == "EasyEnemy") {
                /// враг видит игрока
                float epWidth = entity->x - p.x;
                float peWidth = p.x - entity->x;
                float epHeight = abs((entity->y + entity->h) - (p.y + p.h));

                if (epHeight < 10 &&
                    ((epWidth < 150 && epWidth > 0) || (peWidth < 150 && peWidth > 0))) { //и если игрок находится ниже врага и находится на расстояние 400 слева от него, то...
                    entity->showDialogText = true;
                    entity->showPlayer = true;
                }

                if (entity->showDialogText) //
                {
                    text.setString(L"ААААААH!!!");
                    text.setPosition(entity->x + 55, entity->y - 63); //задаём позицию текста относительно коорд. конкретного врага (так чтобы помещался в облачко)
                    dialogAppTimer += time; //активируем таймер
                    if (dialogAppTimer > 2000) { //если таймер дошёл до 2-х секунд, то...
                        entity->showDialogText = false; // шоуДиалогТект приравниваем к фолс
                        dialogAppTimer = 0; //обнуляем таймер
                    }
                }
                //если прямоугольник спрайта объекта пересекается с игроком
                if (entity->getRect().intersects(p.getRect())) {
                    if (entity->dx > 0)//если враг идет вправо
                    {
                        entity->x = p.x - (float) entity->w; //отталкиваем его от игрока влево (впритык)
                        entity->dx = 0;//останавливаем
                    }
                    if (entity->dx < 0)//если враг идет влево
                    {
                        entity->x = p.x + (float) p.w; //аналогично - отталкиваем вправо
                        entity->dx = 0;//останавливаем
                    }


                    ///////выталкивание игрока
                    if (p.dx < 0) { //если столкнулись с врагом и игрок идет влево то выталкиваем игрока
                        p.x = entity->x + (float) entity->w;
                    }
                    if (p.dx > 0) { //если столкнулись с врагом и игрок идет вправо то выталкиваем игрока
                        p.x = entity->x - (float) p.w;
                    }
                }
            }
            if (!entity->life) {    // если этот объект мертв, то удаляем его
                continue;
            }
            for (it2 = entities.begin(); it2 != entities.end(); it2++) {
                if (entity->getRect() != (*it2)->getRect()) {//при этом это должны быть разные прямоугольники
                    //если столкнулись два объекта и они враги
                    if (entity->getRect().intersects((*it2)->getRect()) && entity->name == "EasyEnemy" && (*it2)->name == "EasyEnemy") {
                        entity->dx *= -1;//меняем направление движения врага
                        entity->sprite.scale(-1, 1);//отражаем спрайт по горизонтали
                    }
                }
            }

        }

        p.update(time);
        window.setView(view);
        window.clear(Color(77, 83, 140));
        window.draw(lvl);

        for (auto entity: entities) {
            if (entity->life) {
                window.draw(entity->sprite); //рисуем entities объекты (сейчас это только враги)
                if (entity->showDialogText) {
                    window.draw(text); //рисуем текст
                }
            }
        }
        window.draw(p.sprite);
        window.display();
    }
    return 0;
}