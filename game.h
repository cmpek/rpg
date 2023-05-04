#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "view.h"
#include "mission.h"
#include "entyties.h"

#include "life.h"
#include "menu.h"
#include <vector>
#include <list>
#include <random>

void changeLevel(TileMap &lvl, int &numberLevel) {
    std::string levelMap = "../assets/maps/map.tmx";
    switch (numberLevel) {
        case 1:
            levelMap = "../assets/maps/map.tmx";
            break;
        case 2:
            levelMap = "../assets/maps/map2.tmx";
            break;
        case 3:
            levelMap = "../assets/maps/map3.tmx";
            break;
    }
    lvl.load(levelMap);
}

bool startGame(RenderWindow &window, int &numberLevel) {
    srand(time(NULL));
    menu(window);
    view.reset(FloatRect(0, 0, 640, 480));

    Font font;
    font.loadFromFile("../assets/fonts/CyrilicOld.ttf");
    Text text("", font, 20);
    text.setFillColor(Color::White);

    TileMap lvl;
    changeLevel(lvl, numberLevel);

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

    SoundBuffer shootBuffer;//создаём буфер для звука
    shootBuffer.loadFromFile("../assets/sounds/shoot.ogg");//загружаем в него звук
    Sound shoot(shootBuffer);//создаем звук и загружаем в него звук из буфера

    //Music music;//создаем объект музыки
    //music.openFromFile("../assets/sounds/music.ogg");//загружаем файл
    //music.play();//воспроизводим музыку

    LifeBar lifeBarPlayer;//экземпляр класса полоски здоровья

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
                shoot.play();//играем звук пули
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    entities.push_back(new Bullet(BulletImage, "Bullet", lvl, p.x, p.y, 16, 16, p.state));
                    shoot.play();//играем звук пули
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

        if (Keyboard::isKeyPressed(Keyboard::T)) { // если T, переходим на следующий уровень и перезапускаем
            numberLevel++;
            return true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Tab)) { //если таб, то перезагружаем игру
            return true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Escape)) { //если эскейп, то выходим из игры
            return false;
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
        lifeBarPlayer.update(p.health);
        lifeBarPlayer.draw(window);//рисуем полоску здоровья
        window.draw(p.sprite);
        window.display();
    }
    return false;
}

void gameRunning(RenderWindow &window, int &numberLevel) {//ф-ция перезагружает игру , если это необходимо
    if (startGame(window, numberLevel)) { ////если startGame() == true, то вызываем занова ф-цию isGameRunning, которая в свою очередь опять вызывает startGame()
        gameRunning(window, numberLevel);
    }
}