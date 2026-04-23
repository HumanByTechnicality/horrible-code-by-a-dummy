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

 SUNLIGHT! GARLIC! RUNNING WATER! GRAIN ON THE GROUND! LACK OF ENEMY STAND USERS! LIVING BUT ABSENT JOESTARS!
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

 * |--\  |  |
 * |__/  |  |
 * |  \  |  |
 * |__/
 * ______________________________________________________________________________________________________
 * ______________________________________________________________________________________________________
 * ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||
 * std::cout<<"pr "<<i<<" "<<projectiles[i]->getSpritePos(0,0).x<<" "<<projectiles[i]->getSpritePos(0,0).y;
 * INCARCERATED FOR: esoteric pointer bullshit causing the game to break when two projectiles are spawned
 * APPREHENDED BY: Gemini, found: Projectile::getProjectiles(), ACTORS.h, 4/7/2026, lunch period E
 * ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||
 * ======================================================================================================
 * ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||
 * collision::HitBox* getHitBoxes(int idx) {auto hb = *activeHitBoxes; return hb[idx];};
 * INCARCERATED FOR: messing up the update order when two projectiles are attempting to collide with the opponent
 * APREHENDED BY: Me, found: actors::Projectile::getHitBoxes(), ACTORS.h, 4/8/2026
 * ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||
 * ______________________________________________________________________________________________________
 * ______________________________________________________________________________________________________

 **/




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





namespace game {
    /*STATEFLAGS:signals sent by the game to determine what the next course of action is*/
    enum stateFlag {
        STATE_null = -1,
        STATE_loading,
        STATE_entering,
        STATE_active,
        STATE_paused,
        STATE_exiting,
        SWAP_base,
        SWAP_menu_base,
        SWAP_menu_main,
        SWAP_menu_settings,
        SWAP_menu_editor,
        SWAP_menu_shop,
        SWAP_menu_training_shop,
        SWAP_menu_end,
        SWAP_combat_base,
        SWAP_combat_offline,
        SWAP_combat_online,
        SWAP_combat_training,
        SWAP_combat_end,
        SWAP_end,
    };

    struct stateData {
        stateFlag OriginType = SWAP_base;
        std::array<std::array<int,80>,6> ints;
        std::array<std::array<int,80>,6> doubles;
        std::array<std::array<std::string,20>,2> strings;
    };
    /**GAMESTATES: bastardous amalgamations of various logic which
 *can vary based upon the current game state**/
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

        virtual void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos){}

        virtual void update(sf::Clock & clock){}

        virtual void draw(sf::RenderWindow& window) {
            for (auto layer : layers) {
                window.draw(layer);
            }

        }
        virtual void draw(sf::RenderWindow& window, sf::Shader* shader) {
            for (auto layer : layers) {
                window.draw(layer);
            }

        }

        virtual stateData exit() {
            return stateData{SWAP_base};
        }

        virtual ~GameState() = default;
    };


    class MenuState : public GameState {
    private:

    protected:

        bool usingMouse = true;
        std::vector<ui::Indicator> indicators = {};
        std::vector<ui::Button> buttons = {};
        int indexHovered = -1;
        int indexSelected = -1;
        float alpha = 0.0;
        std::vector<sf::Texture> textures;
        std::array<int,64> tags;
        sf::Sprite back = sf::Sprite();
        std::vector<sf::Sprite> others;

    void checkHover(int x,int y) {
        if (usingMouse) {
            for (int i = 0; i < buttons.size(); i++) {
                if (buttons[i].checkHover(x,y)) {
                    indexHovered = i;
                    return;
                }
            }
            indexSelected = -1;
        }
    }

    void click(int x, int y) {
        if (indexHovered != -1) {
            buttons[indexHovered].press(x,y);
            indexSelected = indexHovered;
        }
    }

    void release(int x, int y) {
        if (indexSelected != -1) {
            buttons[indexSelected].release(x,y);
        }
        indexSelected = -1;
    }

    public:
        MenuState() : GameState() {

        }

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override {
            bool found = false;
            for (int i = 0; i<buttons.size(); i++) {
                if (!found) {
                    indexHovered = -1;
                    if (buttons[i].checkHover(mousePos.x/graphics::scale, mousePos.y/graphics::scale)) {
                        indexHovered = i;
                        //std::cout<<"Found hovering: "<<i<<std::endl;
                        found = true;
                        if (event.type == sf::Event::MouseButtonPressed) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                click(mousePos.x, mousePos.y);
                                alpha = 0.9;
                            }

                        }
                        if (event.type == sf::Event::MouseButtonReleased) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                release(mousePos.x, mousePos.y);
                            }

                        }
                    }
                }else {
                    buttons[i].setHover(false);
                }

            }

        }

        void update(sf::Clock &clock) override {
            for (int i = 0; i<buttons.size(); i++) {
                others[i].setTextureRect(buttons[i].getTextureRect());
                buttons[i].update();
            }
            std::cout<<tags[0]<<","<<tags[1]<<","<<tags[2]<<","<<tags[3]<<","<<tags[4]<<std::endl;

            if (alpha >0) alpha -= 0.045;
            else alpha = 0;
        }

        void draw(sf::RenderWindow& window, sf::Shader* shader) override {
            window.draw(back);
            shader->setUniform("alpha",alpha);
            for (int i = 0; i<5; i++) {
                if (tags[i] != 0) shader->setUniform("execute", true);
                else shader->setUniform("execute", false);

                window.draw(others[i],shader);
            }
        }

        stateData exit() override {
            return stateData{SWAP_menu_base};
        }

        ~MenuState() override {}
    };

    class MainMenu : public MenuState {
    private:
    protected:
    public:
        MainMenu() : MenuState() {
            textures.resize(6);
            others.resize(5);

            textures[0].loadFromFile("../__pic/titleBack.png");
            back.setTexture(textures[0]);
            back.setScale(graphics::scale, graphics::scale);
            back.setPosition(0,0);

            textures[1].loadFromFile("../__pic/titleDummy.png");
            others[0].setTexture(textures[1]);
            others[0].setScale(graphics::scale, graphics::scale);
            buttons.push_back(ui::Button({65,108,31,31},
                {-6,-6,40,40},2,{0,1},&tags[0]));
            others[0].setPosition(buttons[0].getRealImgX(),buttons[0].getRealImgY());
            others[0].setTextureRect(buttons[0].getTextureRect());
            buttons[0].setBehavior(ui::pressFunction::toggle, ui::releaseFunction::NONE);


            textures[2].loadFromFile("../__pic/titleArcade.png");
            others[1].setTexture(textures[2]);
            others[1].setScale(graphics::scale, graphics::scale);
            buttons.push_back(ui::Button({107,78,30,61},
                {-6,-4,48,70},2,{0,1},&tags[1]));
            others[1].setPosition(buttons[1].getRealImgX(),buttons[1].getRealImgY());
            others[1].setTextureRect(buttons[1].getTextureRect());
            buttons[1].setBehavior(ui::pressFunction::toggle, ui::releaseFunction::NONE);


            textures[3].loadFromFile("../__pic/titlePoster.png");
            others[2].setTexture(textures[3]);
            others[2].setScale(graphics::scale, graphics::scale);
            buttons.push_back(ui::Button({149,73,29,39},
                {-5,-5,39,49},2,{0,1},&tags[2]));
            others[2].setPosition(buttons[2].getRealImgX(),buttons[2].getRealImgY());
            others[2].setTextureRect(buttons[2].getTextureRect());
            buttons[2].setBehavior(ui::pressFunction::toggle, ui::releaseFunction::NONE);

            textures[4].loadFromFile("../__pic/titleTable.png");
            others[3].setTexture(textures[4]);
            others[3].setScale(graphics::scale, graphics::scale);
            buttons.push_back(ui::Button({179,106,83,33},
                {-4,-9,92,46},2,{0,1},&tags[3]));
            others[3].setPosition(buttons[3].getRealImgX(),buttons[3].getRealImgY());
            others[3].setTextureRect(buttons[3].getTextureRect());
            buttons[3].setBehavior(ui::pressFunction::toggle, ui::releaseFunction::NONE);

            textures[5].loadFromFile("../__pic/titleTools.png");
            others[4].setTexture(textures[5]);
            others[4].setScale(graphics::scale, graphics::scale);
            buttons.push_back(ui::Button({269,123,31,29},
                {-6,-6,37,39},2,{0,1},&tags[4]));
            others[4].setPosition(buttons[4].getRealImgX(),buttons[4].getRealImgY());
            others[4].setTextureRect(buttons[4].getTextureRect());
            buttons[4].setBehavior(ui::pressFunction::toggle, ui::releaseFunction::NONE);

            for (int i = 0; i< buttons.size(); i++) {
                buttons[i].setWrite(&tags[i]);
            }
        }

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override {
            bool found = false;
            for (int i = 0; i<buttons.size(); i++) {
                if (!found) {
                    indexHovered = -1;
                    if (buttons[i].checkHover(mousePos.x/graphics::scale, mousePos.y/graphics::scale)) {
                        indexHovered = i;
                        //std::cout<<"Found hovering: "<<i<<std::endl;
                        found = true;
                        if (event.type == sf::Event::MouseButtonPressed) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                click(mousePos.x, mousePos.y);
                                alpha = 0.9;
                            }

                        }
                        if (event.type == sf::Event::MouseButtonReleased) {
                            if (event.mouseButton.button == sf::Mouse::Left) {
                                release(mousePos.x, mousePos.y);
                            }

                        }
                    }
                }else {
                    buttons[i].setHover(false);
                }

            }
        }

        void update(sf::Clock &clock) override {
            for (int i = 0; i<buttons.size(); i++) {
                others[i].setTextureRect(buttons[i].getTextureRect());
                buttons[i].update();
            }
            std::cout<<tags[0]<<","<<tags[1]<<","<<tags[2]<<","<<tags[3]<<","<<tags[4]<<std::endl;

            if (alpha >0) alpha -= 0.045;
            else alpha = 0;
        }

        void draw(sf::RenderWindow& window, sf::Shader* shader) override {
            window.draw(back);
            shader->setUniform("alpha",alpha);
            for (int i = 0; i<5; i++) {
                if (tags[i] != 0) shader->setUniform("execute", true);
                else shader->setUniform("execute", false);

                window.draw(others[i],shader);
            }
        }

        stateData exit() override {
            return stateData{SWAP_menu_main};
        }

        ~MainMenu() override {}
    };

    class Settings : public MenuState {
    private:
    protected:
    public:
        Settings() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_menu_settings};
        }

        ~Settings() override {}
    };

    class FighterCreate : public MenuState {
    private:
    protected:
    public:
        FighterCreate() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_menu_editor};
        }

        ~FighterCreate() override {}
    };


    class Shop : public MenuState {
    private:
    protected:
    public:
        Shop() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_menu_shop};
        }

        ~Shop() override {}
    };


    class TrainingShop : public MenuState {
    private:
    protected:
    public:
        TrainingShop() : MenuState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_menu_training_shop};
        }

        ~TrainingShop() override {}
    };

    enum CombatProgress {
        COMBAT_none,
        COMBAT_start,
        COMBAT_combat,
        COMBAT_end,
        COMBAT_exit,
    };

    class CombatState : public GameState {
    private:
        int freeze = 0;
        bool paused = false;
        int avg = 0;
        double camY = 0;
        int hits[2] = {0,0};
        double healthbarWidth =  83;
        double healthbarHeight = 8;

        std::array<int,390> tilesets = {
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,8,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
0,1,1,3,1,1,6,7,0,1,1,9,1,1,13,7,2,1,1,12,1,3,13,7,0,1,3,1,1,1,13,7,2,9,1,1,1,1,6,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,5,4,4,4,4,
4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        };

        sf::Sprite UI = sf::Sprite();
        sf::Sprite UIoverlay = sf::Sprite();

        graphics::Camera cam;

        sf::Sprite playerIcons[2] = {sf::Sprite(), sf::Sprite()};

        sf::RectangleShape healthBar[2] = {sf::RectangleShape(),sf::RectangleShape()};

        sf::Texture temp;
        graphics::TileMap tileMap = graphics::TileMap();

        std::array<sf::Sprite,2> sprite;
        std::array<sf::Sprite,16> sprite2;
        std::array<actors::Fighter*, 2> fighters;
        Player player = Player();
        std::array<std::array<std::vector<sf::Rect<double>>,2>,2> boxes;

        sf::Texture UItex = sf::Texture();
        sf::Texture icoTex = sf::Texture();

        //animation::ParticleHandler ph = animation::ParticleHandler();
    protected:
    public:
        CombatState() : GameState() {
            fighters[0] = new actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","6b2A","5b1A","3a1A","2a1A",
                "crouch", "crouched","uncrouch",
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A",
            "hit00","hit01","hit02","hit10","hit11","hit12","hit20","hit21","hit22",
                "hitA0","knocked","getupG1",
                "j2b1A","j2b2A","j2b3A","lightLand","heavyLand","knockDown"});
            fighters[1] = new actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","5b1A","3a1A","2a1A",
                "crouch", "crouched","uncrouch",
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A",
            "hit00","hit01","hit02","hit10","hit11","hit12","hit20","hit21","hit22",
            "hitA0","knocked","getupG1","lightLand","heavyLand","knockDown"});
            fighters[0]->turnTo(util::direction::RIGHT);
            fighters[1]->turnTo(util::direction::LEFT);
            fighters[0]->setX(-60);
            fighters[1]->setX(60);

            temp = sf::Texture();
            sprite = {sf::Sprite(), sf::Sprite()};

            UItex.loadFromFile("../__pic/UI.png");
            icoTex.loadFromFile("../__pic/ico1.png");

            UI.setTexture(UItex);
            UI.setTextureRect({0,0,300,41});
            UI.setScale(graphics::scale,graphics::scale);

            UIoverlay.setTexture(UItex);
            UIoverlay.setTextureRect({0,41,300,41});
            UIoverlay.setScale(graphics::scale,graphics::scale);

            playerIcons[0].setTexture(icoTex);
            playerIcons[0].setScale(graphics::scale,graphics::scale);
            playerIcons[0].setPosition(4 * graphics::scale,4 * graphics::scale);

            playerIcons[1].setTexture(icoTex);
            playerIcons[1].setScale(-graphics::scale,graphics::scale);
            playerIcons[1].setPosition(296 * graphics::scale,4 * graphics::scale);

            healthBar[0].setFillColor(sf::Color(0,255,61));
            /*healthBar[0].setOutlineColor(sf::Color(0,197,47));
            healthBar[0].setOutlineThickness(graphics::scale * 2.f);*/
            healthBar[0].setSize(sf::Vector2f{float(healthbarWidth  * graphics::scale), float(healthbarHeight * graphics::scale)});
            healthBar[0].setPosition(40 * graphics::scale,5 * graphics::scale);
            healthBar[1].setFillColor(sf::Color(0,255,61));
            /*healthBar[1].setOutlineColor(sf::Color(0,197,47));
            healthBar[1].setOutlineThickness(graphics::scale * 2.f);*/
            healthBar[1].setSize(sf::Vector2f{float(-healthbarWidth * graphics::scale ), float(healthbarHeight * graphics::scale)});
            healthBar[1].setPosition(260 * graphics::scale,5 * graphics::scale);


            tileMap.load("../__pic/ogres.png",sf::Vector2u{32,32},&tilesets[0],39,10);


            /*ph.addParticle(animation::particleType(
                animation::Sb_static, {0,0,0,0},
                animation::Mb_straight, {0,0,0,0},
                animation::Lb_none, {0,0,0,0},
                animation::Tb_blip, {4,0,0,0},
                {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3}
                ));*/

        }

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override {
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



            if (paused) {

            }
            else {
                if (fighters[0]->checkForClearCall()) {
                    player.clearQueue(fighters[0]);
                }

                player.printDirections();
                if (fighters[0]->outwardState[0] >fighters[1]->outwardState[0]){
                    fighters[0]->turnTo(util::direction::LEFT);
                    fighters[1]->turnTo(util::direction::RIGHT);

                }
                else if (fighters[0]->outwardState[0] < fighters[1]->outwardState[0]){
                    fighters[0]->turnTo(util::direction::RIGHT);
                    fighters[1]->turnTo(util::direction::LEFT);
                }
                player.addDirection(clock, fighters[0]->getFacing());
                std::vector<int> inputs = player.inputDirections.peekFullBack();

                //std::cout<<"fighter 0"<<std::endl;

                if (freeze > 0 ) {
                    freeze --;
                }
                else {
                    fighters[0]->takeInput(inputs,clock);

                    fighters[0]->update();
                    //std::cout<<"fighter 1"<<std::endl;
                    fighters[1]->update();
                    //std::cout<<std::endl;
                    healthBar[0].setSize(sf::Vector2f{float(healthbarWidth * fighters[0]->getHealthProp() * graphics::scale),healthBar[0].getSize().y});
                    healthBar[1].setSize(sf::Vector2f{float(-healthbarWidth * fighters[1]->getHealthProp() * graphics::scale),healthBar[0].getSize().y});

                    for (auto f:fighters) {
                        if (f->outwardState[0]>2000) {
                            f->setX(2000);
                        }
                        if (f->outwardState[0]<-2000) {
                            f->setX(-2000);
                        }
                    }




                    std::array<collision::hitData,2> hitDat = collision::hitCheck(fighters);
                    if (hitDat[0].box != nullptr) {
                        int hit = fighters[0]->doHit(hitDat[0].box, hitDat[0].id, fighters[1]->hit(hitDat[0].box));
                        switch (hit) {
                            case -1:
                                break;
                            case 1:
                                freeze = 8;
                                cam.applyShake(1.0,6);
                                break;
                            case 2:
                            case 4:
                                freeze = 10;
                                cam.applyShake(2.0,8);
                                break;
                            default:
                                freeze = 13;
                                cam.applyShake(4.0,10);
                                break;
                        }

                        hits[1] = 6;
                    }
                    if (hitDat[1].box != nullptr) {
                        int hit = fighters[1]->doHit(hitDat[1].box, hitDat[1].id,fighters[0]->hit(hitDat[1].box));
                        switch (hit) {
                            case -1:
                                break;
                            case 1:
                                freeze = 8;
                                cam.applyShake(1.0,6);
                                break;
                            case 2:
                            case 4:
                                freeze = 10;
                                cam.applyShake(2.0,8);
                                break;
                            default:
                                freeze = 13;
                                cam.applyShake(4.0,10);
                                break;
                        }

                        hits[0] = 6;
                    }

                    //fighters[0]->updateProjectiles(numFrames);
                    boxes = {fighters[0]->getBoxes(), fighters[1]->getBoxes()};
                }


                //amera shenaniganry. GO!
                avg = (fighters[0]->outwardState[0] + fighters[1]->outwardState[0])/2-graphics::internalRes.x/2;
                camY = std::min(std::max(fighters[0]->outwardState[1],fighters[1]->outwardState[1]) * 0.6 + std::min(fighters[0]->outwardState[1],fighters[1]->outwardState[1])* 0.4, std::min(fighters[0]->outwardState[1],fighters[1]->outwardState[1])+30.0) - 24.f;

                cam.update(avg, camY);
            }

            for (int i = 0; i< sf::Keyboard::KeyCount; i++) {
                player.keysDownLast[i] = player.keysDown[i];
            }


            numFrames++;
            if (hits[0]>0) {
                hits[0]--;
            }
            if (hits[1]>0) {
                hits[1]--;
            }
        }

        void draw(sf::RenderWindow& window, sf::Shader* shader) override {
            tileMap.setScale(graphics::scale,graphics::scale);
            tileMap.setPosition((-cam.x- tileMap.width/2) * graphics::scale, (-tileMap.height  + cam.y + graphics::internalRes.y + 72.f )*graphics::scale);
            window.draw(tileMap);
            //std::cout << pos.x << ", " << pos.y << std::endl;
            if (fighters[0]->renderFront() < fighters[1]->renderFront()) {
                for (int i = 0; i< 2; i++) {
                    sf::Vector2f pos = fighters[i]->getSpritePos(cam.x,cam.y);
                    temp = *fighters[i]->getTexture();

                    sprite[i].setTexture(temp);
                    sprite[i].setScale(graphics::scale * fighters[i]->outwardState[2], graphics::scale);
                    sprite[i].setTextureRect(fighters[i]->getTextureRect(-1,-1));
                    sprite[i].setPosition(pos.x*graphics::scale,graphics::windowSize.y-pos.y*graphics::scale);
                    shader->setUniform("fighter",i);
                    shader->setUniform("hit",hits[i]/6.f);
                    window.draw(sprite[i], shader);
                }
            }
            else {
                for (int i = 1; i>-1; i--) {
                    sf::Vector2f pos = fighters[i]->getSpritePos(cam.x,cam.y);
                    temp = *fighters[i]->getTexture();

                    sprite[i].setTexture(temp);
                    sprite[i].setScale(graphics::scale * fighters[i]->outwardState[2], graphics::scale);
                    sprite[i].setTextureRect(fighters[i]->getTextureRect(-1,-1));
                    sprite[i].setPosition(pos.x*graphics::scale,graphics::windowSize.y-pos.y*graphics::scale);
                    shader->setUniform("fighter",i);
                    shader->setUniform("hit",hits[i]/6.f);
                    window.draw(sprite[i], shader);
                }
            }

            for (auto &sp: sprite2) {
                sp.setPosition(-200,-200);
            }

            int spIndex = 0;
            for (auto& fig: fighters) {
                auto proj = fig->getProjectiles();
                for (auto pr: proj) {
                    sf::Vector2f pos = pr->getSpritePos(cam.x,cam.y);
                    temp = *(pr->getTexture());

                    sprite2[spIndex].setTexture(temp);
                    sprite2[spIndex].setScale(graphics::scale * pr->outwardState[2],graphics::scale);
                    sprite2[spIndex].setTextureRect(pr->getTextureRect());
                    sprite2[spIndex].setPosition(pos.x*graphics::scale,graphics::windowSize.y-pos.y*graphics::scale);

                    window.draw(sprite2[spIndex]);
                    spIndex++;
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
            window.draw(UI);
            shader->setUniform("fighter",0);
            window.draw(playerIcons[0], shader);
            shader->setUniform("fighter",1);
            window.draw(playerIcons[1], shader);
            window.draw(healthBar[0]);
            window.draw(healthBar[1]);
            window.draw(UIoverlay);
        }

        stateData exit() override {
            return stateData{SWAP_combat_base};
        }

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

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_combat_offline};
        }

        ~OfflineCombat() override {};
    };


    class TrainingCombat : public CombatState {
    private:
    protected:
    public:
        TrainingCombat() : CombatState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_combat_training};
        }

        ~TrainingCombat() override {}
    };


    class OnlineCombat : public CombatState {
    private:
    protected:
    public:
        OnlineCombat() : CombatState(){}

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        stateData exit() override {
            return stateData{SWAP_combat_online};
        }

        ~OnlineCombat() override {};
    };

}




int main() {

    //intialize necessary variables
    game::stateData tempData;
    sf::Shader shader;
    sf::Shader shader2;
    if (!shader.loadFromFile( "../__val/shader.frag", sf::Shader::Fragment)) {
        return 4;
    }
    if (!shader2.loadFromFile( "../__val/shader2.glsl", sf::Shader::Fragment)) {
        return 5;
    }



    sf::Glsl::Vec4 targets[3] = {
        sf::Glsl::Vec4(194.f/255,105.f/255,36.f/255,1.0),
        sf::Glsl::Vec4(136.f/255,144.f/255,95.f/255,1.0),
        sf::Glsl::Vec4(97.f/255,148.f/255,146.f/255,1.0),
    };

    sf::Glsl::Vec4 replacements1[3] = {
        sf::Glsl::Vec4(194.f/255,105.f/255,36.f/255,1.0),
        sf::Glsl::Vec4(136.f/255,144.f/255,95.f/255,1.0),
        sf::Glsl::Vec4(97.f/255,148.f/255,146.f/255,1.0),
    };

    sf::Glsl::Vec4 replacements2[3] = {
        sf::Glsl::Vec4(56.f/255,105.f/255,190.f/255,1.0),
        sf::Glsl::Vec4(39.f/255,150.f/255,204.f/255,1.0),
        sf::Glsl::Vec4(28.f/255,148.f/255,223.f/255,1.0),
    };

    shader.setUniformArray("targetColors", targets, 3);
    shader.setUniformArray("replaceColors1", replacements1, 3);
    shader.setUniformArray("replaceColors2", replacements2, 3);
    shader.setUniform("tolerance", 0.1f);

    sf::Glsl::Vec4 ignoreColor(0.f,0.f,0.f,1.f);
    shader2.setUniform("ignoreColor", ignoreColor);
    shader2.setUniform("tolerance", 0.05f);
    shader2.setUniform("alpha", 0.f);

    auto gameWindow = sf::RenderWindow(sf::VideoMode(graphics::windowSize.x,graphics::windowSize.y),"fightingCoach");//the window that the game is drawn to
    graphics::rt.create(graphics::internalRes.x, graphics::internalRes.y);
    data::initInputRefs();

    const sf::Time timePerFrame = sf::seconds(1.f/60.f);
    const sf::Time frameRateLimit = sf::seconds(1.f/120.f);
    sf::Clock clock;
    sf::Clock clock2;
    sf::Clock clock3;
    clock2.restart();
    sf::Time timeSinceUpdate = sf::Time::Zero;
    sf::Time timeSinceDraw = sf::Time::Zero;
    sf::Time elapsedTime;

    sf::Event event;
    gameWindow.setFramerateLimit(0);

    game::GameState* gs = new game::CombatState();

    while (gameWindow.isOpen()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(gameWindow);

        while (gameWindow.pollEvent(event)) {

            // Check for the "Close" request (e.g., clicking the 'X')
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            gs->handleEvents(event, clock2, mousePos);

        }
        elapsedTime = clock.restart();
        timeSinceUpdate += elapsedTime;
        timeSinceDraw += elapsedTime;




        while (timeSinceUpdate > timePerFrame) {
            timeSinceUpdate -= timePerFrame;
            gs->update(clock2);
            //fighters[2].update();
            //st.update()
        }

        //yay
        if (timeSinceDraw > frameRateLimit) {
            gameWindow.clear(sf::Color(110,140,160,255));
            timeSinceDraw = frameRateLimit;
            gs->draw(gameWindow, &shader);
            gameWindow.display();
        }


    }
    delete gs;

    return 0;
}