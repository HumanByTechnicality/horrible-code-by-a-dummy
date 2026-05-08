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
* * ======================================================================================================
 * ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||  ||
 * ../__val/5c1A.txt
 * INCARCERATED FOR: somehow causing the debugger to reach into esoteric assembly language bugs whenever it was included in a moveset
 * APREHENDED BY: Me, found: game::CombatState::CombatState()
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
    /*
     * maps actions to inputs and allows those mappings to be modified later by the player
     */
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

    std::array<bool, sf::Keyboard::KeyCount> keysDown;//list containing all keys held down on the current frame
    std::array<bool, sf::Keyboard::KeyCount> keysDownLast;//list containing all keys held down on the last frame

    std::vector<Input> inputBuffer;//the buffer containing the li

    util::circularBuffer<sf::Time> inputTimes;//the times when specific inputs were executed
    util::circularBuffer<sf::Time> inputDirTimes;//the times when specific directions were held

    //NOTE: -1 means NO change in input direction; it is added to keep input directions in sync.
    util::circularBuffer<int> inputDirections;//the buffer holding all input directions to be interpreted by the game

    sf::Vector2f cursorPos;//the position of the cursor
    sf::Vector2f selection;//the current selected button (currently unused)

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

    //adds an input time to the queue of input times (used to do more, now kept to prevent extensive restructuring
    void addInput(sf::Clock &clock, int id, inputType type, bool press, util::direction facing) {
        int currDir = 5;
        //inputBuffer.emplace_back(type, id, press);
        inputTimes.writeWithOverWrite(clock.getElapsedTime());
        //std::cout<< "added an imput methinks"<<std::endl;
    }

    //adds a direction to the directions buffer according to the current held keys
    void addDirection(sf::Clock &clock, util::direction facing) {
        //default to neutral direction
        int currDir = 5;

        //check for attacking buttons (overrides all other directions)
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

        //directional checks
        else {
            //check for Left input from keyboard
            if (keysDown[actionMaps[WALK_LEFT][KEY][0].action]) {
                currDir -= (int)facing;
            }
            //check for Down input from keyboard
            if (keysDown[actionMaps[CROUCH][KEY][0].action]) {
                currDir -= 3;
            }
            //check for Right input from keyboard
            if (keysDown[actionMaps[WALK_RIGHT][KEY][0].action]) {
                currDir += (int)facing;
            }
            //check for Up input from keyboard
            if (keysDown[actionMaps[JUMP][KEY][0].action]) {
                currDir += 3;
            }
        }

        //Strict neutral only when the input is completely neutral
        if (currDir == util::NEUTRAL) {
            currDir = (int)util::inputType::STRICT_NEUTRAL;
        }

        //write to inputs buffers
        inputDirections.writeWithOverWrite(currDir);
        inputDirTimes.writeWithOverWrite(clock.getElapsedTime());
    }

    //clears the queue of inputs to prevent repeated use of moves
    void clearQueue(actors::Fighter* f) {
        inputDirections.clear();
        f->clearClearCall();
    }

    //unused function, I forgot what this does
    void updateCombatInputs(const sf::Clock & clock) {
        int size = inputBuffer.size();
    }

    //prints the key states to the console
    void printKeys() {
        for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
            std::string code = sf::Keyboard::getDescription(sf::Keyboard::delocalize(sf::Keyboard::Key(i)));
            //std::cout<<code<<":"<<keysDown[i]<<" ";
        }
        //std::cout<<std::endl;
    }

    //prints the directions buffer to the console
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
    struct GameTracker {
        std::unordered_map<std::string, int> intTags; // a set of tags for tracking the gamestate, to be interpreted as int
        std::unordered_map<std::string, int> decTags; // a set of tags for tracking the game state, to be interpreted as fixed-point decimals
        std::unordered_map<std::string, bool> boolTags; // a set of tags for tracking the game state, to be interpreted as booleans
        sf::Font font;

        //load the fonts and various things
        void load() {
            std::cout<<"loading font: pixelfont.ttf"<<std::endl;
            font.loadFromFile("../__dat/pixelfont.ttf");
            std::cout<<"pixelfont.ttf: complete"<<std::endl;
            std::cout<<"finished loading game"<<std::endl;
        }

        //removes a key from the set of int tags
        void removeInt(const std::string& key) {
            intTags.erase(key);
        }

        //removes a key from the set of decimal tags
        void removeDec(const std::string& key) {
            decTags.erase(key);
        }

        //removes a key from the set of bool tags
        void removeBool(const std::string& key) {
            boolTags.erase(key);
        }
    };

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
        SWAP_menu_onlineSelection,
        SWAP_end,
    };

    //originally intended to hol over state data when swapping states before the gameTracker was devised. useless now.
    struct stateData {
        stateFlag OriginType = SWAP_base;
        std::array<std::array<int,80>,6> ints;
        std::array<std::array<int,80>,6> doubles;
        std::array<std::array<std::string,20>,2> strings;
        
    };

    GameTracker track = GameTracker();//initializes the game tracker within static memory

    /**loads and logs data such as stats, unlocks, and builds**/
    class gameLogger {
        void loadStats(){}

        void saveStats(){}

        void loadBuild(){}

        void loadStory(){}

        void logToNet(){}


    };

    /**GAMESTATES: bastardous amalgamations of various logic which
 *can vary the program's behavior based upon the current game state**/
    class GameState {
    private:
        std::array<graphics::Layer, 10> layers;
        sf::Time timer;
        std::array<Player*,2> players;
        bool isSinglePlayer = false;


    protected:
        int numFrames = 0;
        std::vector<sf::Text> labels;
    public:
        GameState() = default;

        virtual void enter(GameTracker *tracker) {
            tracker->intTags["gameState"] = (int) SWAP_base;
            tracker->intTags["swapState"] = (int) SWAP_base;
        }

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

        virtual void exit(GameTracker *tracker) {
            tracker->intTags["lastState"] = (int) SWAP_base;
            return;
        }

        virtual ~GameState() = default;
    };

    struct ButtonWrapper {

    };

    struct IndicatorWrapper {

    };

    /**Any Gamestate which involves a menu of some kind**/
    class MenuState : public GameState {
    private:

    protected:

        bool usingMouse = true;

        std::vector<ui::Indicator> indicators = {};
        std::vector<ui::TextIndicator> textIndicators = {};
        std::vector<ui::Button> buttons = {};

        int indexHovered = -1;
        int indexSelected = -1;
        float alpha = 0.0;
        std::vector<sf::Texture> textures;
        sf::Sprite back = sf::Sprite();
        std::vector<sf::Sprite> others;
        std::vector<int> buttonTextureIDs;
        int backdropTextureID = 0;
        int selectedButton = -1;

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
            selectedButton = indexSelected;
        }
    }

    void release(int x, int y) {
        if (indexSelected != -1) {
            buttons[indexSelected].release(x,y);
        }
        indexSelected = -1;
    }

    public:
        MenuState(std::string file) : GameState() {
            std::ifstream vals("../__dat/" + file + ".txt");

            if (!vals.is_open()) {
                std::cerr << "Error opening file " << std::endl;
            } else {
                int parseType = 0;

                std::string butRead = "H";
                std::string butWrite = "H";

                int pressFunc = 0;
                int numStates = 0;
                int releaseFunc = 0;

                std::string texFile;
                int textID = 0;

                std::vector<int> stateMap = {};

                sf::IntRect rect;
                sf::IntRect coll;

                int imgBehavior = 0;

                std::string line;

                while (std::getline(vals, line)) {
                    //separate flag from data on each line
                    std::vector<std::string> flag = util::split(line, ':');

                    //set flag[0] to lowercase
                    std::transform(flag[0].begin(), flag[0] .end(), flag[0].begin(),
                        [](unsigned char c){ return std::tolower(c); });

                    if (parseType == 0) {
                        if (flag[0] == "buttons") {
                            parseType = 1;
                        }
                        else if (flag[0] == "gamestate") {
                        }
                        else if (flag[0] == "backdrop") {
                            textures.push_back(sf::Texture());
                            textures[textures.size() - 1].loadFromFile(flag[1]);
                            back.setScale(graphics::scale, graphics::scale);
                            back.setPosition(0,0);
                        }

                    }
                    else if (parseType == 1) {
                        if (flag[0] == "read") {
                            butRead = flag[1];
                        }
                        else if (flag[0] == "write") {
                            butWrite = flag[1];
                        }
                        else if (flag[0] == "pressfunc") {
                            auto dat = util::split(flag[1],' ');
                            pressFunc = std::stoi(dat[0]);
                            numStates = std::stoi(dat[1]);
                        }
                        else if (flag[0] == "releasefunc") {
                            releaseFunc = std::stoi(flag[1]);
                        }
                        else if (flag[0] == "icon") {
                            auto dat = util::split(flag[1],' ');
                            texFile = dat[0];
                            rect = sf::IntRect(std::stoi(dat[1]),std::stoi(dat[2]),std::stoi(dat[3]),std::stoi(dat[4]));
                        }
                        else if (flag[0] == "collider") {
                            auto dat = util::split(flag[1],' ');
                            coll = sf::IntRect(std::stoi(dat[0]),std::stoi(dat[1]),std::stoi(dat[2]),std::stoi(dat[3]));
                        }
                        else if (flag[0] == "imgbehavior") {
                            imgBehavior = std::stoi(flag[1]);
                        }
                        else if (flag[0] == "statemap") {
                            auto dat = util::split(flag[1],' ');
                            for (auto datum: dat) {
                                stateMap.push_back(std::stoi(datum));
                            }
                        }
                        else if (flag[0] == "save") {
                            textures.push_back(sf::Texture());
                            textures[textures.size() - 1].loadFromFile(texFile);
                            buttonTextureIDs.push_back(textures.size() - 1);

                            bool skipR = false;
                            bool skipW = false;

                            if (butRead != "H") track.intTags.try_emplace(butRead, 0);
                            else skipR = true;
                            if (butWrite != "H") track.intTags.try_emplace(butWrite, 0);
                            else skipW = true;

                            auto wit = track.intTags.find(butWrite);
                            auto rit = track.intTags.find(butRead);
                            int * write = nullptr;
                            int * read = nullptr;

                            if (wit != track.intTags.end() && !skipW) {
                                write = &(wit->second);
                            }
                            if (rit != track.intTags.end() && !skipR) {
                                read = &(wit->second);
                            }

                            buttons.emplace_back(coll,rect,numStates,stateMap,imgBehavior,write, read);
                            buttons[buttons.size()-1].setBehavior(static_cast<ui::pressFunction>(pressFunc), static_cast<ui::releaseFunction>(releaseFunc));
                            others.push_back(sf::Sprite());
                        }
                        else if (flag[0] == "end") {
                            parseType = 0;
                        }

                    }


                }

                for (int i = 0; i< buttons.size(); i++) {
                    others[i].setTexture(textures[buttonTextureIDs[i]]);
                    others[i].setPosition(buttons[i].getRealImgX(), buttons[i].getRealImgY());
                    others[i].setScale(graphics::scale, graphics::scale);
                }
                back.setTexture(textures[backdropTextureID]);
                textIndicators.push_back(ui::TextIndicator({
                    "          Training",
                    "                                             VS",
                    "                                                     Tournament",
                    "                                                                                  Fighter",
                    "                                                                                         Settings"
                }, {static_cast<float>(20 * graphics::scale),static_cast<float>(145* graphics::scale),70,140},&track.font,5,{0,1,2,3,4,5},&indexHovered,14 * graphics::scale));

            }
        }
        MenuState() : GameState() {

        }

        void enter(GameTracker *tracker) override {
            tracker->intTags["gameState"] = (int) SWAP_menu_base;
            tracker->intTags["swapState"] = (int) SWAP_base;
        }

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
            for (auto& ti : textIndicators) {
                //TODO: implement textIndicator updates
                ti.updateDrawable();
            }

            if (alpha >0) alpha -= 0.045;
            else alpha = 0;
            if (track.intTags["swapState"] != (int) SWAP_base) {
                std::cout<<"You should probably swap to the next state right about now."<<std::endl;
            }
            if (alpha >0) alpha -= 0.045;
            else alpha = 0;
        }

        void draw(sf::RenderWindow& window, sf::Shader* shader) override {
            window.draw(back);
            shader->setUniform("alpha",alpha);
            for (int i = 0; i<others.size(); i++) {
                if (i == indexSelected) shader->setUniform("execute", true);
                else shader->setUniform("execute", false);

                window.draw(others[i],shader);
            }

            for (auto& ti : textIndicators) {
                window.draw(ti.getDrawable());
            }
        }

        void exit(GameTracker *tracker) override {
            return;
        }

        ~MenuState() override {}
    };

    /*class MainMenu : public MenuState {
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

        void enter(GameTracker *tracker) override{}

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

        void exit(GameTracker *tracker) override {
            return;
        }

        ~MainMenu() override {}
    };*/

    class Settings : public MenuState {
    private:
    protected:
    public:
        Settings() : MenuState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
        }

        ~Settings() override {}
    };

    class FighterCreate : public MenuState {
    private:
    protected:
    public:
        FighterCreate() : MenuState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
        }

        ~FighterCreate() override {}
    };


    class Shop : public MenuState {
    private:
    protected:
    public:
        Shop() : MenuState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
        }

        ~Shop() override {}
    };


    class TrainingShop : public MenuState {
    private:
    protected:
    public:
        TrainingShop() : MenuState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
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
        int freeze = 0;//counter for impact and hitstop
        int timeTracker = 0;//generic time counter
        bool paused = false;//whether the game is paused
        int camX = 0;//x position of the camera
        double camY = 0;//y position of the camera
        int hits[2] = {0,0};//the ids of the boxes that fighters were hit by
        double healthbarWidth = 83;//width of the health bar on the UI
        double healthbarHeight = 8;//height of the health bar on the UI
        int timeLeftInRound = 60;//seconds left in the round (or multiples of 60 frames)
        sf::Time startTime;//time when the round started

        std::array<int,390> tilesets = {//initializes a tileset
            //TODO: load tilesets from a file instead of a hardcoded initialization
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

        sf::Sprite UI = sf::Sprite();//UI sprite
        sf::Sprite UIoverlay = sf::Sprite();//overlay over the UI
        sf::Sprite text = sf::Sprite();//text in general

        graphics::Camera cam;//the camera object

        sf::Sprite playerIcons[2] = {sf::Sprite(), sf::Sprite()};

        sf::RectangleShape healthBar[2] = {sf::RectangleShape(),sf::RectangleShape()};

        sf::Texture temp;
        graphics::TileMap tileMap = graphics::TileMap();

        std::array<sf::Sprite,2> sprite;
        std::array<sf::Sprite,16> sprite2;
        std::array<actors::Fighter*, 2> fighters;
        Player *  player = new Player();
        std::array<std::array<std::vector<sf::Rect<double>>,2>,2> boxes;

        sf::Texture UItex = sf::Texture();
        sf::Texture icoTex = sf::Texture();
        sf::Texture textTex = sf::Texture();

        //animation::ParticleHandler ph = animation::ParticleHandler();
    protected:
        bool roundActive = false;
    public:
        CombatState() : GameState() {
            fighters[0] = new actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","6b2A","5b1A","3a1A","2a1A","236a1A","236a2A","1a1A","5c1A",
                "crouch", "crouched","uncrouch", "214a1A","2b1A","j2a1A",//grabs will cause the game to kill itself. will fix later.
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A",
            "hit00","hit01","hit02","hit10","hit11","hit12","hit20","hit21","hit22",
                "hitA0","knocked","getupG1","getupG2",
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
            textTex.loadFromFile("../__pic/startText.png");

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
            text.setPosition(84*graphics::scale, 60 * graphics::scale);
            text.setScale(graphics::scale,graphics::scale);
            text.setTexture(textTex);
            text.setTextureRect({0,32,131,32});

            /*ph.addParticle(animation::particleType(
                animation::Sb_static, {0,0,0,0},
                animation::Mb_straight, {0,0,0,0},
                animation::Lb_none, {0,0,0,0},
                animation::Tb_blip, {4,0,0,0},
                {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3}
                ));*/
            labels.push_back(sf::Text(std::to_string(timeLeftInRound), track.font, 10 * graphics::scale));
            labels[0].setPosition(150 * graphics::scale - labels[0].getGlobalBounds().width/2, 14 * graphics::scale);

        }

        void enter(GameTracker *tracker) override {
            tracker->intTags["gameState"] = (int) SWAP_combat_base;
        }

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override {
            if (event.type == sf::Event::KeyPressed) {
                player->keysDown[event.key.code] = true;

                player->addInput(clock, event.key.code, KEY, true, fighters[0]->getFacing());
                if (event.key.code == sf::Keyboard::T) {
                    std::vector<std::array<int,4>> bounds = {};
                    auto hb = collision::HitBox(bounds,{0,50,2,0,0,-3,4,14,6,0,0,0,0,-1});
                    fighters[0]->hit(&hb);
                }
                if (event.key.code == sf::Keyboard::Backspace) {
                    std::cout<<"now wait just one second:"<<std::endl;


                }
            }
            if (event.type == sf::Event::KeyReleased) {
                player->keysDown[event.key.code] = false;

                player->addInput(clock, event.key.code, KEY, false, fighters[0]->getFacing());
                    //player.printKeys();
            }
        }
        void update(sf::Clock &clock) override {
            if (numFrames == 180) {
                roundActive = true;
                startTime= clock.getElapsedTime();
                text.setTextureRect({0,0,0,0});
            }
            if (numFrames == 150) {
                text.setTextureRect({0,0,131,32});
            }
            player->updateCombatInputs(clock);

            combatLogic(clock);


        }

        void combatLogic(sf::Clock & clock) {
            if (paused) {

            }
            else {
                if (fighters[0]->checkForClearCall()) {
                    player->clearQueue(fighters[0]);
                }

                player->printDirections();
                if (fighters[0]->outwardState[0] >fighters[1]->outwardState[0]){
                    fighters[0]->turnTo(util::direction::LEFT);
                    fighters[1]->turnTo(util::direction::RIGHT);

                }
                else if (fighters[0]->outwardState[0] < fighters[1]->outwardState[0]){
                    fighters[0]->turnTo(util::direction::RIGHT);
                    fighters[1]->turnTo(util::direction::LEFT);
                }
                if (roundActive) {
                    if (timeTracker >=60) {
                        timeLeftInRound --;
                        timeTracker -= 60;
                        labels[0].setString(std::to_string(timeLeftInRound));
                        labels[0].setPosition(150 * graphics::scale - labels[0].getGlobalBounds().width/2, 14 * graphics::scale);
                        if (timeLeftInRound == 0) {
                            if (fighters[1]->getHealthProp()<fighters[0]->getHealthProp()) {
                                std::cout<<"player1 wins!"<<std::endl;
                                std::exit(8);
                            }if (fighters[0]->getHealthProp()<fighters[1]->getHealthProp()) {
                                std::cout<<"CPU wins!"<<std::endl;
                                std::exit(9);
                            }
                            else {
                                std::cout<<"it's an MF tie!"<<std::endl;
                                std::exit(10);
                            }

                        }
                    }
                    player->addDirection(clock, fighters[0]->getFacing());
                    std::vector<int> inputs = player->inputDirections.peekFullBack();

                    //std::cout<<"fighter 0"<<std::endl;



                    fighters[0]->takeInput(inputs,clock);

                    timeTracker ++;
                }

                if (freeze > 0 ) {
                    freeze --;
                }
                else {

                    fighters[0]->update();
                    //std::cout<<"fighter 1"<<std::endl;
                    fighters[1]->update();
                    //std::cout<<std::endl;

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
                    if (fighters[1]->getHealthProp()<=0 && fighters[1]->getHitStunAndDown() <= 0) {
                        std::cout<<"player1 wins!"<<std::endl;
                        std::exit(8);
                    }if (fighters[0]->getHealthProp()<=0 && fighters[0]->getHitStunAndDown() <= 0) {
                        std::cout<<"CPU wins!"<<std::endl;
                        std::exit(9);
                    }


                    //fighters[0]->updateProjectiles(numFrames);
                    boxes = {fighters[0]->getBoxes(), fighters[1]->getBoxes()};
                }


                //camera shenaniganry. GO!
                camX = (fighters[0]->outwardState[0] + fighters[1]->outwardState[0])/2-graphics::internalRes.x/2;
                camY = std::min(std::max(fighters[0]->outwardState[1],fighters[1]->outwardState[1]) * 0.6 + std::min(fighters[0]->outwardState[1],fighters[1]->outwardState[1])* 0.4, std::min(fighters[0]->outwardState[1],fighters[1]->outwardState[1])+30.0) - 24.f;
                cam.update(camX, camY);

                //health bars
                healthBar[0].setSize(sf::Vector2f{float(healthbarWidth * fighters[0]->getHealthProp() * graphics::scale),healthBar[0].getSize().y});
                healthBar[1].setSize(sf::Vector2f{float(-healthbarWidth * fighters[1]->getHealthProp() * graphics::scale),healthBar[0].getSize().y});
            }

            for (int i = 0; i< sf::Keyboard::KeyCount; i++) {
                player->keysDownLast[i] = player->keysDown[i];
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
                    shader->setUniform("meter", fighters[i]->getMeterFx());
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
                    shader->setUniform("meter", fighters[i]->getMeterFx());
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
            shader->setUniform("meter", fighters[0]->getMeterFx());
            shader->setUniform("fighter",0);
            window.draw(playerIcons[0], shader);
            shader->setUniform("meter", fighters[1]->getMeterFx());
            shader->setUniform("fighter",1);
            window.draw(playerIcons[1], shader);
            window.draw(healthBar[0]);
            window.draw(healthBar[1]);
            window.draw(UIoverlay);
            window.draw(labels[0]);
            window.draw(text);
        }

        void exit(GameTracker *tracker) override {
            tracker->intTags["lastState"] = (int) SWAP_combat_base;
        }

        ~CombatState() override {
            delete fighters[0];
            delete fighters[1];
            delete player;
        }
    };



    class OfflineCombat : public CombatState {
    private:
    protected:
    public:
        OfflineCombat() : CombatState(){}

        void enter(GameTracker *tracker) override {
            tracker->intTags["gameState"] = (int) SWAP_combat_offline;
        }

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            tracker->intTags["lastState"] = (int) SWAP_combat_offline;
        }

        ~OfflineCombat() override {};
    };


    class TrainingCombat : public CombatState {
    private:
    protected:
    public:
        TrainingCombat() : CombatState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
        }

        ~TrainingCombat() override {}
    };


    class OnlineCombat : public CombatState {
    private:
    protected:
    public:
        OnlineCombat() : CombatState(){}

        void enter(GameTracker *tracker) override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock, sf::Vector2i mousePos) override{}

        void update(sf::Clock &clock) override {}

        void draw(sf::RenderWindow& window) override {}

        void exit(GameTracker *tracker) override {
            return;
        }

        ~OnlineCombat() override {};
    };

    class StoryFight : public CombatState {
        private:
        protected:
        public:
    };

}




int main() {

    //intialize necessary variables
    //game::stateData tempData;
    sf::Shader shader;
    sf::Shader shader2;
    if (!shader.loadFromFile( "../__val/shader.frag", sf::Shader::Fragment)) {
        return 4;
    }
    if (!shader2.loadFromFile( "../__val/shader2.glsl", sf::Shader::Fragment)) {
        return 5;
    }
    game::track.load();



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
    shader.setUniform("meter", 1);

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


    game::track.load();
    game::GameState* gs = new game::MenuState("mainMenu");//game::MenuState("mainMenu");
    gs->enter(&game::track);

    data::UpgradeHandler uH = data::UpgradeHandler();
    data::FighterBuilder fb({"5a1a","hpup","2a1b"},uH);






    // inside the main loop, between window.clear() and window.display()


    int frameNum = 0;
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
            //std::cout<<"id:"<<frameNum<<" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl<<std::endl<<std::endl;
            frameNum++;

            //fighters[2].update();
            //st.update()
        }

        //yay
        if (timeSinceDraw > frameRateLimit) {
            gameWindow.clear(sf::Color(110,140,160,255));
            timeSinceDraw = frameRateLimit;
            gs->draw(gameWindow, &shader2);
            gameWindow.display();

        }


    }
    delete gs;

    return 0;
}