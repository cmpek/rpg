#include <SFML/Graphics.hpp>

using namespace sf;

void menu(RenderWindow &window) {
    Texture menuTexture1, menuTexture2, menuTexture3, aboutTexture, menuBackground;
    menuTexture1.loadFromFile("../assets/images/menu-1.png");
    menuTexture2.loadFromFile("../assets/images/menu-2.png");
    menuTexture3.loadFromFile("../assets/images/menu-3.png");
    aboutTexture.loadFromFile("../assets/images/menu-about.png");
    menuBackground.loadFromFile("../assets/images/menu-bg.png");
    Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), about(aboutTexture), menuBg(menuBackground);
    bool isMenu = 1;
    int menuNum = 0;
    menu1.setPosition(50, 30);
    menu2.setPosition(50, 90);
    menu3.setPosition(50, 150);
    menuBg.setPosition(0, 0);
//////////////////////////////МЕНЮ///////////////////
    while (isMenu) {
        menu1.setColor(Color::White);
        menu2.setColor(Color::White);
        menu3.setColor(Color::White);
        menuNum = 0;
        window.clear(Color(129, 181, 221));
        if (IntRect(50, 30, 300, 50).contains(Mouse::getPosition(window))) {
            menu1.setColor(Color::Blue);
            menuNum = 1;
        }
        if (IntRect(50, 90, 300, 50).contains(Mouse::getPosition(window))) {
            menu2.setColor(Color::Blue);
            menuNum = 2;
        }
        if (IntRect(50, 150, 300, 50).contains(Mouse::getPosition(window))) {
            menu3.setColor(Color::Blue);
            menuNum = 3;
        }
        if (Mouse::isButtonPressed(Mouse::Left)) {
            if (menuNum == 1) {     //если нажали первую кнопку, то выходим из меню
                isMenu = false;
            }
            if (menuNum == 2) {
                window.draw(about);
                window.display();
                while (!Keyboard::isKeyPressed(Keyboard::Escape)) {

                }
            }
            if (menuNum == 3) {
                window.close();
                isMenu = false;
            }
        }
        window.draw(menuBg);
        window.draw(menu1);
        window.draw(menu2);
        window.draw(menu3);
        window.display();
    }
}
