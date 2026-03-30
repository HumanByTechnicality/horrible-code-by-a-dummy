#include <iostream>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <windows.h>
#include <fstream>
#include <array>
#include <SFML/OpenGL.hpp>
#include <cstring>
#include <cmath>

#include "pstl/parallel_backend_serial.h"
#include "SFML/Window/Joystick.hpp"
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <vector>
#include "include/UTIL.h"
#include "include/DATA.h"
#include "include/ANIMATION.h"
#include "include/COLLISION.h"
#include "include/UI.h"
#include "include/GRAPHICS.h"
#include "include/ACTORS.h"


/**
 DANGER-CAUTION-NOTICE-WARNING:
 You are now entering a region of disorganized and incoherent spaghetti
 code from a former python programmer. Any attempt at optimization or
 debugging is done at the risk of the participant and the use of proper
 safety procedures is highly advised. Take breaks. Go outside. Read a book.
 Play a better video game. Disregard the Geneva Suggestions and commit
 several atrocities against the human race. Whatever you choose to do, it
 is a better use of your valuable time than attempting to clean up this
 tangled mess of incoherent logic. You have been warned.

 Time wasted here: 06h:02m:07sec
 Oopsie. I forgot to keep track a while ago. Well over 50h at this point.

 SUNLIGHT! GARLIC! RUNNING WATER! GRAIN ON THE GROUND! LACK OF ENEMY STAND USERS!
 that should keep Dio away. No more freezing and stuttering please.

 @author Matthew Wang

a fly swatter to keep away the bugs
__--___________
_--|TTTTTTTTTTT|
_--|TTTTTTTTTTT|
 _-|TTTTTTTTTTT|/\,
   |TTTTTTTTTTpow! >-_
   |===========|\/'   \___
        |||                \
        |||                 |
        |||               0*0 <- dead bug
        |||
        |||
        |||
        |||
        \_/
 */




enum playerAction {
    SELECT, DESELECT, PAUSE, WALK_LEFT, CROUCH, WALK_RIGHT, JUMP,
    NORMAL, SPECIAL, PARRY, GRAB, UNKNOWN_ACTION, ACTION_COUNT

};


enum inputType {
    KEY, MOUSE, JOYSTICK, BUTTON, INPUT_TYPE_COUNT
};


//TODO: plan Player class
class Agent {
private:
protected:
public:
};
struct Input {
    inputType type;
    int action;
    bool press = true;
};

class Player : public Agent {
private:
    const std::array<std::array<std::vector<Input>, INPUT_TYPE_COUNT>, ACTION_COUNT> defaultActionMaps = {
        //SELECT
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::J}},//KEY
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //DESELECT
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::K}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //PAUSE
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::Escape}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //WALK_LEFT
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::A}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //CROUCH
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::S}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //WALK_RIGHT
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::D}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //JUMP
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::W}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //NORMAL
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::J}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //SPECIAL
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::K}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //PARRY
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::L}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //GRAB
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,sf::Keyboard::H}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,0}},
            },
        //UNKNOWN_ACTION
        std::array<std::vector<Input>, INPUT_TYPE_COUNT>{
            std::vector<Input>{{KEY,-1}},
            std::vector<Input>{{MOUSE,-1}},
            std::vector<Input>{{JOYSTICK,-1}},
            std::vector<Input>{{BUTTON,-1}},
            },
    };
    std::array<std::array<std::vector<Input>, INPUT_TYPE_COUNT>, ACTION_COUNT> actionMaps;
protected:
public:
    const float maxInputTime = 0.5;
    std::array<bool, sf::Keyboard::KeyCount> keysDown;
    std::array<bool, sf::Keyboard::KeyCount> keysDownLast;
    std::vector<Input> inputBuffer;
    util::circularBuffer<sf::Time> inputTimes;
    util::circularBuffer<sf::Time> inputDirTimes;
    //NOTE: -1 means NO change in input direction; it is added to keep input directions in sync.
    util::circularBuffer<int> inputDirections;

    sf::Vector2f cursorPos;
    sf::Vector2f selection;

    Player() : inputTimes(20), inputDirTimes(20), inputDirections(20) {
        //initialize action maps to default values
        for (int action = 0; action < ACTION_COUNT; action++) {
            for (int inputList = 0; inputList < INPUT_TYPE_COUNT; inputList++) {
                for (int i = 0; i < defaultActionMaps[action][inputList].size(); i++) {
                    actionMaps[action][inputList].push_back(defaultActionMaps[action][inputList][i]);
                    //std::cout<<actionMaps[action][inputList][i].action<<std::endl;
                }
            }
        }

        //initialize keysDown
        for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
            keysDown[i] = false;
        }

        inputBuffer.emplace_back(INPUT_TYPE_COUNT);
    }

    void addInput(sf::Clock &clock, int id, inputType type, bool press, util::direction facing) {
        int currDir = 5;
        //inputBuffer.emplace_back(type, id, press);
        inputTimes.writeWithOverWrite(clock.getElapsedTime());
        //std::cout<< "added an imput methinks"<<std::endl;
    }

    void addDirection(sf::Clock &clock, util::direction facing) {
        int currDir = 5;

        if (keysDown[actionMaps[GRAB][KEY][0].action]) {
            currDir = (int)util::inputType::GRAB;
        }
        else if (keysDown[actionMaps[SPECIAL][KEY][0].action] && !keysDownLast[actionMaps[SPECIAL][KEY][0].action]) {
            currDir = (int)util::inputType::SPECIAL;
        }
        else if (keysDown[actionMaps[NORMAL][KEY][0].action] && !keysDownLast[actionMaps[NORMAL][KEY][0].action]) {
            currDir = (int)util::inputType::NORMAL;
            //std::cout<<"normal pressed";
        }
        else {
            if (keysDown[actionMaps[WALK_LEFT][KEY][0].action]) {
                currDir -= (int)facing;
            }
            if (keysDown[actionMaps[CROUCH][KEY][0].action]) {
                currDir -= 3;
            }
            if (keysDown[actionMaps[WALK_RIGHT][KEY][0].action]) {
                currDir += (int)facing;
            }
            if (keysDown[actionMaps[JUMP][KEY][0].action]) {
                currDir += 3;
            }
        }

        if (currDir == util::NEUTRAL) {
            currDir = (int)util::inputType::STRICT_NEUTRAL;
        }
        inputDirections.writeWithOverWrite(currDir);
        inputDirTimes.writeWithOverWrite(clock.getElapsedTime());
    }

    void clearQueue(actors::Fighter* f) {
        inputDirections.clear();
        f->clearClearCall();
    }

    void updateCombatInputs(const sf::Clock & clock) {
        int size = inputBuffer.size();
        /*for (int i  =0; i< size; i++) {
            //std::cout<<inputTimes.size()<<std::endl;

            if (clock.getElapsedTime() - inputTimes[i] > sf::seconds(maxInputTime)) {
                inputTimes.erase(inputTimes.begin() + i);
                inputBuffer.erase(inputBuffer.begin() + i);

                size--;
                i--;
            }
        }

        size = inputDirections.size();
        for (int i  =0; i< size; i++) {
            //std::cout<<inputTimes.size()<<std::endl;

            if (clock.getElapsedTime() - directionTimes[i] > sf::seconds(maxInputTime)) {
                inputDirections.erase(inputDirections.begin() + i);
                directionTimes.erase(directionTimes.begin() + i);

                size--;
                i--;
            }
        }*/

    }

    void printKeys() {
        for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
            std::string code = sf::Keyboard::getDescription(sf::Keyboard::delocalize(sf::Keyboard::Key(i)));
            //std::cout<<code<<":"<<keysDown[i]<<" ";
        }
        //std::cout<<std::endl;
    }

    void printDirections() {
        std::vector<int> in = inputDirections.peekFull();
        for (int i = 0; i< in.size(); i++) {
            //std::cout<<in.at(i)<<" ";
        }
        //std::cout<<std::endl;
    }

};

class AI : public Agent{
private:
protected:
public:
};




/**GAMESTATES: bastardous amalgamations of various logic which
 *can vary based upon the current game state**/
namespace game {
    class GameState {
    private:
        std::array<graphics::Layer, 10> layers;
        sf::Time timer;
        std::array<Player,2> players;
        bool isSinglePlayer = false;
    protected:
        int numFrames = 0;
    public:
        GameState() = default;

        virtual void enter(){}

        virtual void handleEvents(const sf::Event &event, sf::Clock &clock){}

        virtual void update(sf::Clock & clock){}

        virtual void draw(sf::RenderWindow& window) {
            for (auto layer : layers) {
                window.draw(layer);
            }

        }

        virtual void exit(){}

        virtual ~GameState() = default;
    };


    class MenuState : public GameState {
    private:
        std::vector<ui::Indicator> indicators = {};
        std::vector<ui::Button> buttons = {};

    protected:

    void checkButtonClick(int x,int y) {

    }
    public:
        MenuState() : GameState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~MenuState() override {}
    };

    class MainMenu : public MenuState {
    private:
    protected:
    public:
        MainMenu() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {

        }

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~MainMenu() override {}
    };

    class Settings : public MenuState {
    private:
    protected:
    public:
        Settings() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~Settings() override {}
    };

    class FighterCreate : public MenuState {
    private:
    protected:
    public:
        FighterCreate() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~FighterCreate() override {}
    };


    class Shop : public MenuState {
    private:
    protected:
    public:
        Shop() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~Shop() override {}
    };


    class TrainingShop : public MenuState {
    private:
    protected:
    public:
        TrainingShop() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~TrainingShop() override {}
    };


    class CombatState : public GameState {
    private:
        sf::Texture temp;
        std::array<sf::Sprite,2> sprite;
        std::array<actors::Fighter*, 2> fighters;
        Player player = Player();
        std::array<std::array<std::vector<sf::Rect<double>>,2>,2> boxes;
    protected:
    public:
        CombatState() : GameState() {
            fighters[0] = new actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","5b1A","3a1A","2a1A",
                "crouch", "crouched","uncrouch",
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A",
            "hit00","hit01","hit02","hit10","hit11","hit12","hit20","hit21","hit22"});
            fighters[1] = new actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","5b1A","3a1A","2a1A",
                "crouch", "crouched","uncrouch",
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A",
            "hit00","hit01","hit02","hit10","hit11","hit12","hit20","hit21","hit22"});
            fighters[0]->turnTo(util::direction::RIGHT);
            fighters[1]->turnTo(util::direction::LEFT);
            fighters[1]->setX(200);
            temp = sf::Texture();
            sprite = {sf::Sprite(), sf::Sprite()};

        }

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override {
            if (event.type == sf::Event::KeyPressed) {
                player.keysDown[event.key.code] = true;

                player.addInput(clock, event.key.code, KEY, true, fighters[0]->getFacing());
                if (event.key.code == sf::Keyboard::T) {
                    std::vector<std::array<int,4>> bounds = {};
                    auto hb = collision::HitBox(bounds,{0,10,2,0,2,2,4,14,6,0,0,0,0,-1});
                    fighters[0]->hit(hb);
                }
                if (event.key.code == sf::Keyboard::Backspace) {
                    std::cout<<"now wait just one second:"<<std::endl;
                }
            }
            if (event.type == sf::Event::KeyReleased) {
                player.keysDown[event.key.code] = false;

                player.addInput(clock, event.key.code, KEY, false, fighters[0]->getFacing());
                    //player.printKeys();
            }
        }

        void update(sf::Clock &clock) override {
            player.updateCombatInputs(clock);
            if (fighters[0]->checkForClearCall()) {
                player.clearQueue(fighters[0]);
            }

            player.printDirections();
            if (fighters[0]->outwardState[0] >fighters[1]->outwardState[0]){
                fighters[0]->turnTo(util::direction::LEFT);
                fighters[1]->turnTo(util::direction::RIGHT);

            }
            else {
                fighters[0]->turnTo(util::direction::RIGHT);
                fighters[1]->turnTo(util::direction::LEFT);
            }
            player.addDirection(clock, fighters[0]->getFacing());
            std::vector<int> inputs = player.inputDirections.peekFullBack();

            fighters[0]->takeInput(inputs,clock);
            //std::cout<<"fighter 0"<<std::endl;
            fighters[0]->update();
            //std::cout<<"fighter 1"<<std::endl;
            fighters[1]->update();




            std::array<collision::hitData,2> hitDat = collision::hitCheck(fighters);
            if (hitDat[0].box != nullptr) {
                fighters[1]->hit(hitDat[0].box);
                fighters[0]->doHit(hitDat[0].box);
            }
            if (hitDat[1].box != nullptr) {
                fighters[0]->hit(hitDat[1].box);
                fighters[0]->doHit(hitDat[0].box);
            }

            //fighters[0]->updateProjectiles(numFrames);
            //boxes = {fighters[0]->getBoxes(), fighters[1]->getBoxes()};

            for (int i = 0; i< sf::Keyboard::KeyCount; i++) {
                player.keysDownLast[i] = player.keysDown[i];
            }

            numFrames++;
        }

        void draw(sf::RenderWindow& window) override {

            //std::cout << pos.x << ", " << pos.y << std::endl;
            auto avg = 0;//(fighters[0]->outwardState[0] + fighters[1]->outwardState[0])/2-150;
            if (fighters[0]->renderFront() < fighters[1]->renderFront()) {
                for (int i = 0; i< 2; i++) {
                    sf::Vector2f pos = fighters[i]->getSpritePos(avg,0);
                    temp = *fighters[i]->getTexture();

                    sprite[i].setTexture(temp);
                    sprite[i].setScale(4.0f * fighters[i]->outwardState[2], 4.0f);
                    sprite[i].setTextureRect(fighters[i]->getTextureRect(-1,-1));
                    sprite[i].setPosition(pos.x*4,graphics::windowSize.y-pos.y*4);
                    window.draw(sprite[i]);
                }
            }
            else {
                for (int i = 1; i>-1; i--) {
                    sf::Vector2f pos = fighters[i]->getSpritePos(avg,0);
                    temp = *fighters[i]->getTexture();

                    sprite[i].setTexture(temp);
                    sprite[i].setScale(4.0f * fighters[i]->outwardState[2], 4.0f);
                    sprite[i].setTextureRect(fighters[i]->getTextureRect(-1,-1));
                    sprite[i].setPosition(pos.x*4,graphics::windowSize.y-pos.y*4);
                    window.draw(sprite[i]);
                }
            }

            /*try {
                for (auto boxey:boxes) {
                    for (sf::Rect<double> box : boxey.at(0)) {
                        sf::RectangleShape rect = sf::RectangleShape({(float)box.width, (float)box.height});
                        rect.setPosition({(float)box.left, graphics::windowSize.y-(float)box.top - (float)box.height + 1+56.f});
                        rect.setFillColor(sf::Color{0,255,0,50});

                        window.draw(rect);
                    }
                }
            }catch (std::out_of_range &e) {

            }

            try {
                for (auto boxey:boxes) {
                    for (sf::Rect<double> box : boxey.at(1)) {
                        sf::RectangleShape rect = sf::RectangleShape({(float)box.width, (float)box.height});
                        rect.setPosition({(float)box.left, graphics::windowSize.y-(float)box.top - (float)box.height + 1 +56.f});
                        rect.setFillColor(sf::Color{255,100,100,100});

                        window.draw(rect);
                    }
                }
            }catch (std::out_of_range &e) {

            }*/
        }

        void exit() override{}

        ~CombatState() override {
            delete fighters[0];
        }
    };


    class OfflineCombat : public CombatState {
    private:
    protected:
    public:
        OfflineCombat() : CombatState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~OfflineCombat() override {};
    };


    class TrainingCombat : public CombatState {
    private:
    protected:
    public:
        TrainingCombat() : CombatState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~TrainingCombat() override {}
    };


    class OnlineCombat : public CombatState {
    private:
    protected:
    public:
        OnlineCombat() : CombatState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit() override{}

        ~OnlineCombat() override {};
    };
}




int main() {

    //intialize necessary variables
    auto gameWindow = sf::RenderWindow(sf::VideoMode(1280,720),"fightingCoach");//the window that the game is drawn to
    graphics::rt.create(graphics::internalRes.x, graphics::internalRes.y);
    data::initInputRefs();

    const sf::Time timePerFrame = sf::seconds(1.f/60.f);
    sf::Clock clock;
    sf::Clock clock2;
    clock2.restart();
    sf::Time timeSinceUpdate = sf::Time::Zero;
    sf::Time elapsedTime;

    sf::Event event;
    gameWindow.setFramerateLimit(0);

    game::CombatState gs = game::CombatState();

    while (gameWindow.isOpen()) {
        gameWindow.clear(sf::Color::Black);

        while (gameWindow.pollEvent(event)) {

            // Check for the "Close" request (e.g., clicking the 'X')
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            gs.handleEvents(event, clock2);

        }
        elapsedTime = clock.restart();
        timeSinceUpdate += elapsedTime;




        while (timeSinceUpdate > timePerFrame) {
            timeSinceUpdate -= timePerFrame;
            gs.update(clock2);
            //fighters[2].update();
            //st.update()
        }

        //yay

        gs.draw(gameWindow);

        gameWindow.display();
    }

    return 0;
}