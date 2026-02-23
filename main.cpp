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


//contains all functions necessary for the game but not included by the other namespaces, such as camera movement and game states
namespace util {

    class Camera {
    private:
    protected:
    public:
        double x;
        double y;
        double zoom;
    };

    //function to shift each element of an array over by one, overloaded for arrays of char, int and float.
    bool shift(char list[], int size, int increment = 1, bool cycle=false){
        increment = increment %size;
        char list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = 'x';
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<size;i++) {
                if (list2[i] == 'x') {
                    list2[i] = list[((i-increment)+size)%size];
                }
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to 'x' when value is deleted
    bool shift(int list[], int size, int increment = 1, bool cycle=false){
        increment = increment %size;
        int list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = -1;
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<startI;i++) {
                list2[i] = list[(i-increment)%size];
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to -1 when value is deleted
    bool shift(double list[], int size, int increment = 1, bool cycle=false) {
        increment = increment %size;
        double list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = -1;
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<startI;i++) {
                list2[i] = list[(i-increment)%size];
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to -1.0 when value is deleted

    //function to split a string at all instances of a delimiter
    std::vector<std::string> split(std::string &s, char delimiter = ' ') {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = s.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(s.substr(start, end - start));
            start = end + 1;
            end = s.find(delimiter, start);
        }
        tokens.push_back(s.substr(start)); // Add the last token

        return tokens;
    }

    template <typename T>
    class circularBuffer {
    private:
        std::vector<T> buffer;
        int headIdx = 0;
        int tailIdx = 0;
        int capacity = 0;
    public:
        circularBuffer(int size) {
            capacity = size;
            buffer.assign(capacity + 1, T{});
        }
        T read() {
            T b = buffer[tailIdx];
            tailIdx = (tailIdx + 1) %(capacity + 1);
            return b;
        }

        void write(T value) {
            buffer[headIdx] = value;
            headIdx = (headIdx + 1) %(capacity + 1);
        }

        void writeWithOverWrite(T value) {
            buffer[headIdx] = value;
            headIdx = (headIdx + 1) %(capacity + 1);
            if (headIdx == tailIdx) {
                tailIdx = (tailIdx + 1) %(capacity + 1);
            }
        }

        T peek(int idx) {
            int count = (headIdx - tailIdx + (capacity + 1)) % (capacity + 1);

            if (idx < 0 || idx >= count) {
                throw std::out_of_range("Index out of buffer bounds");
            }

            return buffer[(tailIdx + idx) % (capacity + 1)];
        }

        T peekLast() {
            if (isEmpty()) {
                throw std::out_of_range("Index out of buffer bounds");
            }

            return buffer[(headIdx + capacity) % (capacity + 1)];
        }

        std::vector<T> peekFull() {
            std::vector<T> vect;
            int current = tailIdx;

            vect.reserve(capacity);
            for (;current!=headIdx; current = (current + 1) % (capacity + 1)) {
                vect.push_back(buffer[current]);
            }
            return vect;
        }
        std::vector<T> peekFullBack() {
            std::vector<T> vect;
            int current = (headIdx + capacity) % (capacity + 1);

            vect.reserve(capacity);
            for (;current!=tailIdx; current = (current + capacity) % (capacity + 1)) {
                vect.push_back(buffer[current]);
            }
            return vect;
        }

        bool isFull() {
            return (headIdx + 1) % (capacity + 1) == tailIdx;
        }

        bool isEmpty() {
            return  headIdx == tailIdx;
        }

        bool clear() {
            while (!isEmpty()) {
                read();
            }
            return true;
        }


    };

    bool endsWith(const std::vector<int>& set, const std::vector<int>& subset) {
        if (subset.size() > set.size()) {
            return false;
        }

        return std::equal(subset.begin(), subset.end(), set.end() - subset.size());
    }
    bool endsWithFromIndex(const std::vector<int>& set, const std::vector<int>& subset, int startIndex) {
        if (subset.size() > set.size() - startIndex + 1 || subset.size() - startIndex <= 0) {
            return false;
        }

        //std::cout<<subset[startIndex]<<" "<<set[set.size() - startIndex + 1]<<std::endl;
        return std::equal(subset.begin() + startIndex, subset.end(), set.end() - subset.size() + startIndex);
    }



    enum class direction {
        LEFT = -1,
        NONE = 0,
        RIGHT = 1
    };

    enum inputType {
        HARD_BACK = 0,
        DOWN_BACK,
        DOWN,
        DOWN_FRONT,
        BACK,
        NEUTRAL,
        FRONT,
        UP_BACK,
        UP,
        UP_FRONT,
        HARD_FRONT,
        NORMAL,
        SPECIAL,
        GRAB,
        PARRY,
        G,
        A,
        STRICT_NEUTRAL,
        NO_INPUT,
        ANY_INPUT
    };

    enum class hitboxType {
        PUNCH = 0,
        KICK = 1,
        WEAPON = 2,
        PROJECTILE = 10,
        ENERGY = 11,
        BEAM = 12,
        GRAB = 20,
        HIT_GRAB = 21,
        THROW = 22,
        OTHER = 30
    };

    enum class moveType {
        NORMAL = 0,
        SPECIAL = 2,
        METER = 4,
        SUPER = 5,
    };

    enum class damageType {
        FORCE = 0,
        BLUNT = 1,
        SHARP = 2,
        PIERCE = 3,
        MAGIC = 4,
        ENERGY = 5,
        EMOTIONAL = 6,
    };

    enum class height {
        LOW = 0,//must be blocked crouching
        MID = 1,//can be blocked standing or crouching
        HIGH = 2,//must be blocked standing
    };

    enum class gameState {
        TITLE = 0,
        TITLE_LOAD = 1,
        SETTINGS1 = 2,
        SETTINGS2 = 3,
        SETTINGS3 = 4,
        FIGHTER_MAKER = 6,
        SINGLE_PLAYER = 10,
        SINGLE_LOAD = 11,
        SINGLE_SELECT = 12,
        TRAINING_MODE = 13,
        TRAINING_LOAD = 14,
        MULTIPLAYER = 20,
    };

    enum class fightState {
        MATCH_LOAD = 0,
        MATCH_START = 1,
        ROUND_LOAD = 2,
        ROUND_START = 3,
        FIGHT = 4,
        ROUND_END = 5,
        SHOP_LOAD = 6,
        PLAYER1_SHOP = 7,
        PLAYER2_SHOP = 8,
        SHOP_END = 9,
        MATCH_END = 10
    };


}

namespace actors {
    class Fighter;
}

namespace animation {
    /*animType simply denotes the BASE input of the animation.
     *animations such as jab1 and jab2 are delineated by an id variable
     *in the animation.
     */
    enum animType {
        //passive animations and basic movement
        NONE = -1,
        idle = 0,
        walk0 = 1,
        walk1 = 2,
        walk2 = 3,
        crouched = 7,
        crouch = 8,
        uncrouch = 9,
        air0,
        air1,
        air2,
        dash1,
        dash2,
        dash3,
        dash4,
        jump1,
        jump0,
        jump2,
        dash5,
        land,


        g5a = 25, g2a, g4a, g6a, g8a, g1a, g3a, g7a, g9a,

        // --- Aerial normals (auto: 25–33) ---
        j5a, j2a, j4a, j6a, j8a, j1a, j3a, j7a, j9a,


        // --- Grounded specials (auto: 34–42) ---
        g5b, g2b, g4b, g6b, g8b, g1b, g3b, g7b, g9b,


        // --- Aerial specials (auto: 43–51) ---
        j5b, j2b, j4b, j6b, j8b, j1b, j3b, j7b, j9b,


        // --- Reserved special inputs (explicit block 56–87) ---
        g236a = 56, g214a,
        g236b, g214b,
        j236a, j214a,
        j236b, j214b,

        g623a, g412a,
        g623b, g412b,
        j623a, j412a,
        j623b, j412b,

        g632a, g421a,
        g632b, g421b,
        j632a, j421a,
        j632b, j421b,

        g41236a, g63214a,
        g41236b, g63214b,
        j41236a, j63214a,
        j41236b, j63214b,

        // --- Grabs (auto: 89-91) ---
        g4c = 95, g5c, g6c,

        grabthrow = 100,

        //grab reactions
        grabreaction = 101,
        throwreaction = 102,

        justblock = 103,
        blockpoise = 104,
        block = 105,


        //hit reactions
        hit00 = 107, hit01 = 108, hit02 = 109,
        hit10 = 104, hit11 = 105, hit12 = 106,
        hit20 = 101, hit21 = 102, hit22 = 103,

        jhit0 = 110, jhit1 = 111, jhit2 = 112,

        hitSpec1 = 113, hitSpec2 = 114, hitSpec3 = 115,

        knockDown1 = 116, knockDown2 = 117,
        downLand = 118,

        getupG1 = 119, getupG2 = 120, getupG3 = 121, getupG4 = 122,
        getupA1 = 123,

        parry = 124,
        burst = 125,

        ANIM_TYPE_COUNT

    };

    std::string getAnimName(animType anim) {
        switch (anim) {
            case animType::NONE:
                return "none";
            case animType::idle:
                return "idle";
            case animType::walk0:
                return "walk0";
            case animType::walk1:
                return "walk1";
            case animType::walk2:
                return "walk2";
            case animType::jump0:
                return "jump0";
            case animType::jump1:
                return "jump1";
            case animType::jump2:
                return "jump2";
            case animType::crouch:
                return "crouch";
            case animType::uncrouch:
                return "uncrouch";
            case animType::crouched:
                return "crouched";
            case animType::dash1:
                return "dash1";
            case animType::dash2:
                return "dash2";
            case animType::dash3:
                return "dash3";
            case animType::dash4:
                return "dash4";
            case animType::justblock:
                return "justblock";
            case animType::block:
                return "block";
            case animType::blockpoise:
                return "blockpoise";
            case animType::air0:
            case animType::air1:
            case animType::air2:
                return "air";
            case animType::land:
                return "land";
            default:
                return "not handled yet";
        }
    }

}

//stores data for DTOs
namespace data {
    std::array<std::vector<int>, (int)animation::ANIM_TYPE_COUNT> inputRefs;
    std::array<int, (int)animation::ANIM_TYPE_COUNT> leniencies;

    /*used to initialize the array of inputs without some
     *excessive braced-init list that I couldn't read, plus defining input leniencies*/
    void initInputRefs() {
        using animation::animType;
        using util::inputType::DOWN_BACK;
        using util::inputType::DOWN;
        using util::inputType::DOWN_FRONT;
        using util::inputType::BACK;
        using util::inputType::NEUTRAL;
        using util::inputType::FRONT;
        using util::inputType::UP_BACK;
        using util::inputType::UP;
        using util::inputType::UP_FRONT;
        using util::inputType::NORMAL;
        using util::inputType::SPECIAL;
        using util::inputType::PARRY;
        using util::inputType::GRAB;
        using util::inputType::G;
        using util::inputType::A;
        using util::inputType::STRICT_NEUTRAL;
        using util::inputType::NO_INPUT;

        for (int i = 0; i < animation::ANIM_TYPE_COUNT; i++) {
            inputRefs[i].reserve(8);
        }

        inputRefs[animation::idle] = {G, NEUTRAL};

        inputRefs[animation::walk1] = {G, FRONT};
        inputRefs[animation::walk2] = {G, BACK};

        inputRefs[animation::jump0] = {G, UP_BACK};
        inputRefs[animation::jump1] = {G, UP};
        inputRefs[animation::jump2] = {G, UP_FRONT};
        inputRefs[animation::idle] = {G, NEUTRAL};

        inputRefs[animation::crouch] = {G, STRICT_NEUTRAL, DOWN};
        inputRefs[animation::crouched] = {G, DOWN};
        inputRefs[animation::uncrouch] = {G, DOWN, STRICT_NEUTRAL};

        inputRefs[animation::dash1] = {G, STRICT_NEUTRAL, FRONT, FRONT};
        inputRefs[animation::dash2] = {G, STRICT_NEUTRAL, BACK, BACK};
        inputRefs[animation::dash3] = {A, STRICT_NEUTRAL, FRONT, FRONT};
        inputRefs[animation::dash4] = {A, STRICT_NEUTRAL, BACK, BACK};

        inputRefs[animation::justblock] = {G, PARRY};
        inputRefs[animation::block] = {G, BACK, BACK};
        inputRefs[animation::blockpoise] = {G, DOWN_BACK};

        inputRefs[animation::air0] = {A, NEUTRAL};
        inputRefs[animation::air1] = {A, NEUTRAL};
        inputRefs[animation::air2] = {A, NEUTRAL};
        inputRefs[animation::land] = {A, NO_INPUT};


        inputRefs[animation::g7a] = {G, UP_BACK, NORMAL};   inputRefs[animation::g8a] = {G, UP, NORMAL};        inputRefs[animation::g9a] = {G, UP_FRONT, NORMAL};
        inputRefs[animation::g4a] = {G, BACK, NORMAL};      inputRefs[animation::g5a] = {G, NEUTRAL, NORMAL};   inputRefs[animation::g6a] = {G, FRONT, NORMAL};
        inputRefs[animation::g1a] = {G, DOWN_BACK, NORMAL}; inputRefs[animation::g2a] = {G, DOWN, NORMAL};      inputRefs[animation::g3a] = {G, DOWN_FRONT, NORMAL};

        inputRefs[animation::j7a] = {A, UP_BACK, NORMAL};   inputRefs[animation::j8a] = {A, UP, NORMAL};        inputRefs[animation::j9a] = {A, UP_FRONT, NORMAL};
        inputRefs[animation::j4a] = {A, BACK, NORMAL};      inputRefs[animation::j5a] = {A, NEUTRAL, NORMAL};   inputRefs[animation::j6a] = {A, FRONT, NORMAL};
        inputRefs[animation::j1a] = {A, DOWN_BACK, NORMAL}; inputRefs[animation::j2a] = {A, DOWN, NORMAL};      inputRefs[animation::j3a] = {A, DOWN_FRONT, NORMAL};

        inputRefs[animation::g7b] = {G, UP_BACK, SPECIAL};   inputRefs[animation::g8b] = {G, UP, SPECIAL};        inputRefs[animation::g9b] = {G, UP_FRONT, SPECIAL};
        inputRefs[animation::g4b] = {G, BACK, SPECIAL};      inputRefs[animation::g5b] = {G, NEUTRAL, SPECIAL};   inputRefs[animation::g6b] = {G, FRONT, SPECIAL};
        inputRefs[animation::g1b] = {G, DOWN_BACK, SPECIAL}; inputRefs[animation::g2b] = {G, DOWN, SPECIAL};      inputRefs[animation::g3b] = {G, DOWN_FRONT, SPECIAL};

        inputRefs[animation::j7b] = {A, UP_BACK, SPECIAL};   inputRefs[animation::j8b] = {A, UP, SPECIAL};        inputRefs[animation::j9b] = {A, UP_FRONT, SPECIAL};
        inputRefs[animation::j4b] = {A, BACK, SPECIAL};      inputRefs[animation::j5b] = {A, NEUTRAL, SPECIAL};   inputRefs[animation::j6b] = {A, FRONT, SPECIAL};
        inputRefs[animation::j1b] = {A, DOWN_BACK, SPECIAL}; inputRefs[animation::j2b] = {A, DOWN, SPECIAL};      inputRefs[animation::j3b] = {A, DOWN_FRONT, SPECIAL};


        inputRefs[animation::g236a] = {G, DOWN, DOWN_FRONT, FRONT ,NORMAL}; inputRefs[animation::g214a] = {G, DOWN, DOWN_BACK, BACK ,NORMAL};
        inputRefs[animation::g623a] = {G, FRONT, DOWN, DOWN_FRONT ,NORMAL}; inputRefs[animation::g412a] = {G, BACK, DOWN_BACK, DOWN ,NORMAL};
        inputRefs[animation::g632a] = {G, FRONT, DOWN_FRONT, DOWN ,NORMAL}; inputRefs[animation::g421a] = {G, BACK, DOWN, DOWN_BACK ,NORMAL};
        inputRefs[animation::g41236a] = {G, BACK, DOWN_BACK, DOWN, DOWN_FRONT, FRONT ,NORMAL};
        inputRefs[animation::g63214a] = {G, FRONT, DOWN_FRONT, DOWN, DOWN_BACK, BACK ,NORMAL};

        inputRefs[animation::g236b] = {G, DOWN, DOWN_FRONT, FRONT ,SPECIAL}; inputRefs[animation::g214b] = {G, DOWN, DOWN_BACK, BACK ,SPECIAL};
        inputRefs[animation::g623b] = {G, FRONT, DOWN, DOWN_FRONT ,SPECIAL}; inputRefs[animation::g412b] = {G, BACK, DOWN_BACK, DOWN ,SPECIAL};
        inputRefs[animation::g632b] = {G, FRONT, DOWN_FRONT, DOWN ,SPECIAL}; inputRefs[animation::g421b] = {G, BACK, DOWN, DOWN_BACK ,SPECIAL};
        inputRefs[animation::g41236b] = {G, BACK, DOWN_BACK, DOWN, DOWN_FRONT, FRONT ,SPECIAL};
        inputRefs[animation::g63214b] = {G, FRONT, DOWN_FRONT, DOWN, DOWN_BACK, BACK ,SPECIAL};

        inputRefs[animation::j236a] = {A, DOWN, DOWN_FRONT, FRONT ,NORMAL}; inputRefs[animation::j214a] = {A, DOWN, DOWN_BACK, BACK ,NORMAL};
        inputRefs[animation::j623a] = {A, FRONT, DOWN, DOWN_FRONT ,NORMAL}; inputRefs[animation::j412a] = {A, BACK, DOWN_BACK, DOWN ,NORMAL};
        inputRefs[animation::j632a] = {A, FRONT, DOWN_FRONT, DOWN ,NORMAL}; inputRefs[animation::j421a] = {A, BACK, DOWN, DOWN_BACK ,NORMAL};
        inputRefs[animation::j41236a] = {A, BACK, DOWN_BACK, DOWN, DOWN_FRONT, FRONT ,NORMAL};
        inputRefs[animation::j63214a] = {A, FRONT, DOWN_FRONT, DOWN, DOWN_BACK, BACK ,NORMAL};

        inputRefs[animation::j236b] = {A, DOWN, DOWN_FRONT, FRONT ,SPECIAL}; inputRefs[animation::j214b] = {A, DOWN, DOWN_BACK, BACK ,SPECIAL};
        inputRefs[animation::j623b] = {A, FRONT, DOWN, DOWN_FRONT ,SPECIAL}; inputRefs[animation::j412b] = {A, BACK, DOWN_BACK, DOWN ,SPECIAL};
        inputRefs[animation::j632b] = {A, FRONT, DOWN_FRONT, DOWN ,SPECIAL}; inputRefs[animation::j421b] = {A, BACK, DOWN, DOWN_BACK ,SPECIAL};
        inputRefs[animation::j41236b] = {A, BACK, DOWN_BACK, DOWN, DOWN_FRONT, FRONT ,SPECIAL};
        inputRefs[animation::j63214b] = {A, FRONT, DOWN_FRONT, DOWN, DOWN_BACK, BACK ,SPECIAL};


        inputRefs[animation::g4c] = {G, BACK, GRAB};
        inputRefs[animation::g5c] = {G, NEUTRAL, GRAB};
        inputRefs[animation::g6c] = {G, FRONT, GRAB};
        inputRefs[animation::grabthrow] = {G, NO_INPUT};
        inputRefs[animation::grabreaction] = {G, NO_INPUT};
        inputRefs[animation::throwreaction] = {G, NO_INPUT};


        inputRefs[animation::hit20] = {G, NO_INPUT}; inputRefs[animation::hit21] = {G, NO_INPUT}; inputRefs[animation::hit22] = {G, NO_INPUT};
        inputRefs[animation::hit10] = {G, NO_INPUT}; inputRefs[animation::hit11] = {G, NO_INPUT}; inputRefs[animation::hit12] = {G, NO_INPUT};
        inputRefs[animation::hit00] = {G, NO_INPUT}; inputRefs[animation::hit01] = {G, NO_INPUT}; inputRefs[animation::hit02] = {G, NO_INPUT};

        inputRefs[animation::jhit0] = {A, NO_INPUT}; inputRefs[animation::jhit1] = {A, NO_INPUT}; inputRefs[animation::jhit2] = {A, NO_INPUT};


        for (int b : leniencies) {
            b = 1;
        }

        for (int i = animation::idle; i<=animation::j9b; i++) {
            leniencies[i] = 0;
        }
        leniencies[animation::dash1] = 2;
        leniencies[animation::dash2] = 2;
        leniencies[animation::dash3] = 2;
        leniencies[animation::dash4] = 2;

        for (int i = animation::g4c; i<animation::ANIM_TYPE_COUNT; i++) {
            leniencies[i] = 0;
        }


    }

    //the ids of stats in stats arrays
    enum statID {maxHP, speed, airSpeed, grabMult, meleeMult, weaponMult,
        projectileMult, meterMult, maxStamina, meleeRes, projectileRes,
        weaponRes, grabRes, projectileSpeed, lifeSteal, ultBoost, stanceChange,
        upThrow, rageBoost, grabArmor, impactSprint, controlProjectiles, longParry,
        downDash, burst, launcher, poisonGrab, icyWeapons, fieryMelee, stunProjectiles,
        cCanceling, doubleDash, dashAttack, groundedHitbox, invincibleDash, blockBreaker};

    //the types of modification that an upgrade can perform on a stat
    enum modifyType{ADD, SUBTRACT, MULTIPLY, DIVIDE, SET};

    //modifies stats in a fighterBuilder
    class Upgrade{
    public:
        std::string name;
        data::statID stat;
        modifyType type;
        double amount;
        double applyTo(double stat) {
            switch (type) {
                case ADD:
                    return stat + amount;
                case SUBTRACT:
                    return stat - amount;
                case MULTIPLY:
                    return stat * amount;
                case DIVIDE:
                    return stat / amount;
                case SET:
                    return amount;
            }
            return -1;
        }


    };

    //DTO for Fighters
    class FighterBuilder {
    public:
        std::array<const double, 36> defaultStats =
            {1000,5,5,
            1,1,1,1,1,
            300,
            0,0,0,0,
            1,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };
        std::array<double, 36> baseStats;
        std::array<double, 36> buildStats;
        //std::vector<Upgrade> upgrades;
        std::vector<std::string> moveFiles;

        FighterBuilder(std::array<double, 36> bsts, std::vector<std::string> moves) {
            for (int i = 0; i < 36; i++) {
                baseStats[i] = bsts[i];
                buildStats[i] = bsts[i];
            }
            for (auto move : moves) {
                moveFiles.push_back(move);
            }
        }

        FighterBuilder() {
            for (int i = 0; i < 36; i++) {
                baseStats[i] = defaultStats[i];
                buildStats[i] = defaultStats[i];
            }
        }
    };

    //DTO for Stages
    class StageBuilder {

    };
}

//contains the stuff necessary for rendering
namespace graphics {

    enum class layerID {
        SKY = -1,
        BACKDROP1 = 0,
        BACKDROP2 = 1,
        BACKDROP3 = 2,
        BACKDROP4 = 3,
        GROUND = 4,
        EFFECTS_BACK = 5,
        ENTITIES_BACK = 6,
        ENTITIES_FRONT = 7,
        EFFECTS_FRONT = 8,
        FOREGROUND = 9,
    };

    class ActiveSprite{
    public:
        bool active = false;
        int framesSinceLastChange = 0;
        int x = 0;
        int y = 0;
        int renderOrder = 0;
        sf::Sprite*sprite = nullptr;
        ActiveSprite(const sf::Texture &texture, bool startActive):sprite(new sf::Sprite(texture)) {

        }

        const sf::Texture* getTexture() {
            return sprite->getTexture();
        }

        void setTexture(const sf::Texture *texture) {
            sprite->setTexture(*texture);
        }

        void setTextureRect(const sf::IntRect& rect) {
            sprite->setTextureRect(rect);
        }

        ~ActiveSprite() {
            delete sprite;
        }
    };

    class Layer : public sf::Drawable , public sf::Transformable{
    public:

        sf::VertexArray spriteVA;
        std::vector<ActiveSprite*> sprites;
        layerID id;

        Layer() {
            sprites.reserve(1);
            spriteVA = sf::VertexArray(sf::Quads, 4);
        }
        Layer(int maxNumSprites) {
            sprites.reserve(maxNumSprites);
            spriteVA = sf::VertexArray(sf::Quads, maxNumSprites * 4);
        }
    private:
        sf::VertexArray m_vertices;
        sf::Texture m_texture;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            // apply the transform
            states.transform *= getTransform();
            // apply the tileset texture
            states.texture = &m_texture;
            // draw the vertex array
            target.draw(m_vertices, states);
        }

    };

    class AnimatedElement {
    public:
    protected:
    private:
    };


    class Backdrop {
    private:
        std::vector<std::array<double, 2>> layerPositions;
        std::vector<double> layerDepths;
        std::vector<int> layerFrames;

    protected:
    public:


    };



    const sf::Vector2u internalRes(320, 180); // the game's internal resolution
    sf::Vector2u windowSize(960, 540); // actual size of the game window

    sf::RenderTexture rt; // the texture that the game renders to
}

namespace ui {
    enum class pressFunction {
        NONE = -1,
        increment_loop = 0,
        decrement_loop = 1,
        increment_limit = 2,
        decrement_limit = 3,
        toggle = 4,
        drag = 5

    };

    enum class releaseFunction {
        NONE = -1,
        increment_loop = 0,
        decrement_loop = 1,
        increment_limit = 2,
        decrement_limit = 3,
        toggle = 4,
        set = 5
    };

    enum class indicatorType {
        on_off = 0,
        continuous_RIGHT = 1,
        continuous_UP = 2,
        continuous_DOWN = 3,
        continuous_LEFT = 4,
        discrete_RIGHT = 5,
        discrete_UP = 6,
        discrete_DOWN = 7,
        discrete_LEFT = 8,
        discrete_IMAGE = 9,
        counter = 10
    };

    //any ui element that can be selected TODO:plan Button class
    class Button {
    private:
        int x = 0;//x position of top left corner
        int y = 0;//y position of top left corner
        int width = 0;//total width
        int height = 0;//total height

        bool selected = false;//whether the slider is currently selected

        int numStates = 0;//total number of unique states
        int state = 0;//current state (0 to numStates-1)

        int* readVal = nullptr;//value read from to update display
        int* writeVal = nullptr;//value modified by the button when pressed

        pressFunction fP = pressFunction::NONE;
        releaseFunction fR = releaseFunction::NONE;
        indicatorType type = indicatorType::on_off;
    protected:
    public:
        void drag (int xC, int yC) {
            switch (type) {
                case indicatorType::discrete_UP:
                case indicatorType::continuous_UP:
                    state = (double)(y+height-yC)/height * numStates;
                    break;
                case indicatorType::discrete_DOWN:
                case indicatorType::continuous_DOWN:
                    state = (double)(yC-y)/height * numStates;
                    break;
                case indicatorType::discrete_LEFT:
                case indicatorType::continuous_LEFT:
                    state = (double)(x+width-xC)/height * numStates;
                    break;
                case indicatorType::discrete_RIGHT:
                case indicatorType::continuous_RIGHT:
                    state = (double)(xC-x)/height * numStates;
                    break;
                default:
                    break;
            }

            if (state < 0 ) {
                state = 0;
            }

            if (state >= numStates) {
                state = numStates-1;
            }
        }

        void press(int xC, int yC) {
            switch (fP) {
                case pressFunction::NONE:
                    break;
                case pressFunction::increment_loop:
                    state ++;
                    if (state >= numStates) {
                        state = 0;
                    }
                    break;
                case pressFunction::increment_limit:
                    state ++;
                    if (state >= numStates) {
                        state = numStates;
                    }
                    break;
                case pressFunction::decrement_loop:
                    state --;
                    if (state < 0) {
                        state = numStates-1;
                    }
                    break;
                case pressFunction::decrement_limit:
                    state --;
                    if (state < 0) {
                        state = 0;
                    }
                    break;
                case pressFunction::toggle:
                    state = !state;
                    break;
                case pressFunction::drag:
                    drag(xC, yC);
                    break;

            }
            selected = true;
        }
        void release(int x, int y) {
            selected = false;
        }
        void update(int yC = 0, int xC = 0) {
            if (selected && fP == pressFunction::drag) {
                drag(xC, yC);
            }
        }

    };

    //any ui element that changes according to a state TODO:plan Indicator class
    class Indicator {
    private:
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;

        int numStates = 0;
        int state = 0;

        int* readVal = nullptr;

        indicatorType type = indicatorType::on_off;
    protected:
    public:
        void press(int x, int y) {

        }
        void release(int x, int y) {

        }
        void update() {

        }
    };

}


namespace actors {
    class Actor;
    class Fighter;
}




namespace collision {
    using util::direction;
    using util::hitboxType;
    using util::damageType;
    using util::height;

    enum class hurtboxType {
        NONE = -1,
        HURTBOX = 0,
        BLOCKBOX = 1,
        COUNTER = 2,
        REFLECT = 3
    };

    //a set of rectangles which defines a collider TODO: write CollisionBox class
    class CollisionBox{
    private:
    protected:
        std::vector<std::array<int,4>> nullBounds = {{0,0,0,0}};
        std::vector<sf::Rect<double>> localBounds;//the bounds of the collision box with respect to some origin
        std::vector<sf::Rect<double>> globalBounds;//the bounds of the collision box with respect to the stage
        bool exist = false;

        //initializes local variables TODO: write create method for IntRect overload
        void create(std::vector<sf::IntRect> &bounds) {}
        //initializes local variables TODO: write create method for int overload
        void create(std::vector<std::array<int,4>> &bounds) {}
    public:
        direction facing = direction::LEFT;
        //creates a new collision box TODO: write CollisionBox method for int overload
        explicit CollisionBox(std::vector<std::array<int,4>> &bounds) {
            for (int i = 0; i<bounds.size(); i++) {
                localBounds.emplace_back(bounds[i][0], bounds[i][1], bounds[i][2], bounds[i][3]);
                globalBounds.push_back(localBounds.back());
            }

        }
        //creates a new collision box TODO: write CollisionBox method for IntRect overload
        explicit CollisionBox(std::vector<sf::Rect<double>> &bounds) {
            for (int i = 0; i<bounds.size(); i++) {
                localBounds.push_back(bounds[i]);
                globalBounds.push_back(localBounds.back());
            }
        }

        //changes globalBounds to account for the position of the origin TODO: write move method
        void move(int x, int y, bool direction){}

        //returns whether this collision box intersects with another collision box TODO: write intersects method
        bool intersects(CollisionBox &other) {
            if (this->exist && other.exist) {
                for (int i = 0; i<this->globalBounds.size(); i++) {
                    for (int j = 0; j<other.globalBounds.size(); j++) {
                        if (this->globalBounds[i].intersects(other.globalBounds[j])) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    };

    //TODO: plan HitBox class
    class HitBox: public CollisionBox {
    friend class actors::Fighter;
    private:
        bool grab;//whether this hitbox trigger a grab animation on hit
        double damage;//the amount of damage applied on hit
        int knockBackX;//the amount of knockback applied to a grounded target in the X direction
        int knockBackY;//the amount of knockback applied to a grounded target in the Y direction
        int knockBackXA;//the amount of knockback applied to an aerial target in the X direction
        int knockBackYA;//the amount of knockback applied to an aerial target in the Y direction
        int knockBackTime;//the amount of time that knockback on a grounded opponent is applied over
        bool knockDown;//whether this hitbox applies knockdown on hit

        int id;//the id of this hitbox

        int hitStun;//the amount of stun applied on a vulnerable target
        int blockStun;//the amount of stun applied on a blocking target

        damageType dType;//the type of damage this hitbox deals
        hitboxType bType;//the type of attack associated with this hitbox
        height bHeight;//the height of this hitbox

        animation::animType trigger;//type of the animation triggered on hit
        int triggerID;//ID of the animation triggered
    protected:
    public:
        HitBox(std::vector<std::array<int,4>> &bounds, std::vector<std::string> datLines) : CollisionBox(bounds) {
            knockBackX = 0;
            knockBackY = 0;
            knockBackXA = 0;
            knockBackYA = 0;
            knockBackTime = 0;
            hitStun = 0;
            blockStun = 0;
            damage = 0;
            knockDown = false;
            hitStun = 0;
            blockStun = 0;
            dType = damageType::SHARP;
            bType = hitboxType::OTHER;
            bHeight = height::MID;


            exist = true;


        }

        HitBox(): CollisionBox(nullBounds) {
            exist = false;
        }


        //returns the damage type of the hitbox
        hitboxType getBType() {
            return bType;
        }

        //returns the damage of the hitbox
        double getDamage(actors::Fighter* own);
    };

    //TODO: plan HurtBox class
    class HurtBox: public CollisionBox {
    private:
    protected:
    public:
        hurtboxType bType;

        HurtBox():
        CollisionBox(nullBounds), bType(hurtboxType::NONE) {
            exist = false;
        }

        explicit HurtBox(std::vector<std::array<int,4>> bounds, hurtboxType Type) :
        CollisionBox(bounds), bType(Type) {
            exist = true;
        }
    };

    //TODO: plan StageBox class
    class StageBox: public CollisionBox {
    private:
    protected:
    public:
        StageBox(): CollisionBox(nullBounds) {
            exist = false;
        }

        explicit StageBox(std::vector<std::array<int,4>> bounds) : CollisionBox(bounds) {
            exist = true;
        }
    };

    //checks if two CollisionBoxes intersect each other TODO: write intersects method
    bool intersects(CollisionBox &c1, CollisionBox &c2) {
        return c1.intersects(c2);
    }

    std::vector<HitBox> NULLHITBOXES = {HitBox()};
    std::vector<HurtBox> NULLHURTBOXES = {HurtBox()};

    //TODO: ensure hitboxes are sorted in order of decreasing priority before calling this function
    HitBox* hitCheck(std::vector<HitBox> &HiB, std::vector<HurtBox> &HuB) {
        for (int i = 0; i<HiB.size(); i++) {
            for (int j = 0; j<HuB.size(); j++) {
                if (HiB[i].intersects(HuB[j])) {
                    return &HiB[i];
                }
            }
        }
        return &NULLHITBOXES[0];
    }

}

namespace animation {

    struct TextureWrapper {
        std::string fileID;
        sf::Texture texture;
    };

    sf::Texture NullTexture;
    bool nullTextureLoaded = NullTexture.loadFromFile("../__pic/NULLTEXTURE.png");
    std::vector<TextureWrapper> textures = {TextureWrapper{}};

    class Animation {
    protected:
        std::vector<collision::HitBox> hitBoxes;//the set of hitboxes associated with this animation
        std::vector<collision::HurtBox> hurtBoxes;//the set of hurtboxes associated with this animation

        sf::Texture* texture;//the texture associated with this animation

        std::vector<int> frameorder;//the order of frames in this animation, by layer

        std::vector<std::array<double,2>> movements;//da movements
        std::vector<double> gravity;//when and how hard gravity applies to the move

        int width;//the width of one frame
        int height;//the height of one frame
        int anchor;//the x anchor of one frame
        int xOffset;//the offset from x=0 of the top left corner of the first frame
        int yOffset;//the offset from y=0 of the top left corner of the first frame

        bool exist;//whether this animation has been properly initialized
        int ID = 0;
        int wrapperIndex = 0;

        bool active = false;

        animType animType;

        double momentumCarry = 0;
    public:
        std::vector<graphics::layerID> layers;//the layers covered by the animation
        int numLayers;//the number of layers the animation covers
        int frameNum;//the number of unique frames in an animation
        int length;//the length of the animation

        Animation(std::string id) {
            //initialize default values (remove "garbage")
            width = 0;
            height = 0;
            anchor = 0;
            xOffset = 0;
            yOffset = 0;
            momentumCarry = 0;
            exist = false;
            active = false;
            length = 0;
            TextureWrapper texture;

            frameorder.reserve(400);
            hitBoxes.reserve(400);
            hurtBoxes.reserve(400);

            std::string directory = "../";
            std::ifstream vals(directory + "__val/" + id + ".txt");

            if (!vals.is_open()) {
                std::cerr << "Error opening file: " << directory << std::endl;
            }

            else {
                std::vector<std::string> lines;
                std::string line;
                const char*Line;

                while (std::getline(vals, line)) {
                    //set line values to lowercase
                    Line = line.c_str();
                    for (int i = 0; Line[i] != '\0'; i++) {
                        std::tolower(Line[i]);
                    }
                    line = std::string(Line);

                    //separate flag from data on each line
                    std::vector<std::string> flag = util::split(line, ':');

                    //sets image file and texture that the image draws from
                    if (flag[0] == "file") {
                        bool existing = false;
                        for (int i = 0; i< textures.size(); i++) {
                            if (flag[1]==textures[i].fileID) {
                                existing = true;
                                wrapperIndex = i;
                            }
                        }
                        if (!existing) {
                            sf::Texture textureA;
                            textureA.loadFromFile(directory + "__pic/" + flag[1]);
                            textures.push_back(TextureWrapper{flag[1], textureA});
                            wrapperIndex = textures.size() - 1;
                        }
                    }

                    if (flag[0] == "active") {
                        active = stoi(flag[1]);
                    }

                    // sets frameNum
                    if (flag[0] == "framenum") {
                        frameNum = std::stoi(flag[1]);
                    }

                    if (flag[0] == "momentumcarry") {
                        momentumCarry = std::stod(flag[1]);
                    }

                    //sets frameOrder
                    else if (flag[0] == "frameorder") {
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            if (data2.size()>1) {
                                int numFrames = std::stoi(data2[0]);
                                for (int j = 0; j< numFrames; j++) {
                                    frameorder.push_back(std::stoi(data2[1]));
                                    //std::cout << std::stoi(data2[1]) << std::endl;
                                    movements.push_back({0,0});
                                    gravity.push_back(0.0);
                                    length++;
                                }
                            }
                        }
                    }

                    //sets frameOrder
                    else if (flag[0] == "gravity") {
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            if (data2.size()>1) {
                                int numFrames = std::stoi(data2[0]);
                                for (int j = 0; j< numFrames; j++) {
                                    gravity[num] = std::stod(data2[1]);
                                    std::cout << std::stoi(data2[1]) << std::endl;
                                    num++;
                                }
                            }
                        }
                    }

                    else if (flag[0] == "framesize") {
                        std::vector<std::string> data = util::split(flag[1], 'x');
                        width = std::stoi(data[0]);
                        height = std::stoi(data[1]);
                    }
                    else if (flag[0] == "id") {
                        ID = std::stoi(flag[1]);
                        animType =static_cast<animation::animType>(ID/100);
                    }

                    else if (flag[0] == "frameanchor") {
                        std::vector<std::string> data = util::split(flag[1], 'x');
                        anchor = std::stoi(data[0]);
                    }

                    else if (flag[0] == "movement") {
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            int numFrames = std::stoi(data2[0]);
                            if (data2.size()>1) {
                                std::vector<std::string> data3 = util::split(data2[1], ',');
                                //std::cout<<numFrames<<std::endl;
                                if (data3.size()>1) {
                                    for (int k = 0; k< numFrames; k++) {
                                        movements[num][0] = stod(data3[0]);
                                        movements[num][1] = stod(data3[1]);
                                        //std::cout<<"added movement: "<< movements[num][0]<<" "<<num<<std::endl;
                                        num++;
                                    }
                                }
                            }
                        }

                    }


                }
            }
        }

        Animation() {
            width = 0;
            height = 0;
            anchor = 0;
            xOffset = 0;
            yOffset = 0;
            exist = false;
        }

        ~Animation() {}

       [[nodiscard]] const sf::Texture* getTexture() const{
            return &textures.at(wrapperIndex).texture;
        }

        [[nodiscard]] sf::IntRect getFrame(int frame) const{
            return {xOffset, yOffset + height * frameorder[frame], width, height};
        }

        bool operator<(const Animation& other) const {
            return ID < other.ID;
        }

        bool operator>(const Animation& other) const {
            return ID > other.ID;
        }

        bool operator==(const Animation& other) const {
            return ID == other.ID;
        }

        [[nodiscard]] int getAnchor() const{
            return anchor;
        }

        [[nodiscard]] int getWidth() const{
            return width;
        }
        [[nodiscard]] int getHeight() {
            return height;
        }

        [[nodiscard]] bool getActive() const{
            return active;
        }

        [[nodiscard]] enum animType getAnimType() const{
            return animType;
        }

        [[nodiscard]] double getMomentum() const{
            return momentumCarry;
        }

        [[nodiscard]] double getGravity(int frame) const {
            return gravity[frame];
        }

        [[nodiscard]] int getID() const {
            return ID;
        }
        [[nodiscard]] int getInputID() const {
            return ID/100;
        }
        [[nodiscard]] int getOrdinality() const {
            return ID%100/10;
        }
        [[nodiscard]] int getVariant() const {
            return ID%10;
        }

        [[nodiscard]] double getXMove(int frame) const {
            return movements[frame][0];
        }

        [[nodiscard]] double getYMove(int frame) const {
            return movements[frame][1];
        }

    };

    class moveBuffer {
    private:
    protected:
    public:
    };
}

enum playerAction {
    SELECT, DESELECT, PAUSE, WALK_LEFT, CROUCH, WALK_RIGHT, JUMP,
    NORMAL, SPECIAL, PARRY, GRAB, UNKNOWN_ACTION, ACTION_COUNT

};
namespace agents {
    class Player;
}

namespace actors {
    using util::direction;
    using animation::Animation;
    using animation::animType;

    //The general class for anything that moves TODO: plan Actor class
    class Actor {
    private:
    protected:
        double x = 0;//x position of the actor on the screen
        double y = 0;//y position of the actor on the screen
        double dx = 0;//x velocity of the actor
        double dy = 0;//y velocity of the actor

        direction facing = direction::LEFT;//the direction the actor is facing

        std::vector<Animation> animations;//list of this actor's set of animations

        std::vector<collision::HitBox> *activeHitBoxes;//references to the hitboxes active for the fighter
        std::vector<collision::HurtBox> *activeHurtBoxes;//references to the hurtboxes active for the fighter

        virtual void getHitBoxes() {

        }

        virtual void getHurtBoxes() {

        }
    public:
        std::array<int,3> outwardState;

        virtual ~Actor() = default;

        Actor() {
        }

        Actor(std::vector<std::string> animList) {
            x = 0;//x position of the actor on the screen
            y = 0;//y position of the actor on the screen
            dx = 0;//x velocity of the actor
            dy = 0;//y velocity of the actor

            facing = direction::LEFT;

            activeHitBoxes = nullptr;
            activeHurtBoxes = nullptr;

            for (int i = 0; i < animList.size(); i++) {
                animations.emplace_back(animList[i]);
            }


        }



        virtual void update() {
            x += dx;
            y += dy;
            outwardState[0] = x;
            outwardState[1] = y;
            outwardState[2] = static_cast<int>(facing);
        }

        virtual void hit(collision::HitBox &box){}
    };

    struct moveRequest {
        int moveID = -1;
        sf::Time time = sf::Time::Zero;
        util::inputType state = util::inputType::NO_INPUT;
        bool lenient = false;

        void clear() { moveID = -1; time = sf::Time::Zero;}
        bool isValid() const { return moveID != -1; }
    };

    //Any character TODO: plan Fighter class
    class Fighter;

    class Fighter: public Actor {
    friend class collision::CollisionBox;
    private:
        Fighter* enemy = this;
    protected:
        //general gameplay flags and values
        bool forceToIdle = false;
        std::array<int, 9> inputsList;
        int numUsedInputs = 0;
        int hitStun = 0;//frames of hitstun left
        int blockStun = 0;//frames of blockstun left
        int knockedDown = 0;//frames of knockdown left
        int knockBackTime = 0;//the amount of time that knockback will be applied over
        int knockBackX = 0;//the amount of knockback in the x direction
        int knockBackY = 0;//the amount of knockback in the y direction
        double momentumX = 0;//the amount of momentum in the x direction
        double momentumY = 0;//the amount of momentum in the y direction
        double px = 0;
        double py = 0;
        double EXmeter = 0;//the amount of EX meter that the fighter has built up
        int hp = 0;//the current health of the fighter (in tenths of a percent)
        double gravity = 0.4;//the gravity of the fighter
        double jumpheight = 7;
        int dashCoolDown = 0;
        double armor = 0;//the armor hp equivalent of the fighter
        bool blockHigh = false;//whether the fighter is blocking high
        bool blockLow = false;//whether the fighter is blocking low
        bool landing = false;//whether the fighter is landing
        bool jumping = false;//whether the fighter is jumping
        bool crouching = false;//whether the fighter is crouching
        bool uncrouching = false;//whether the fighter is uncrouching
        bool crouched = false;//whether the fighter is crouched
        bool actionable = true;//whether the fighter is capable of performing actions
        bool inMove = false;//whether the fighter is currently in an active move
        bool isDead = false;//whether the fighter is K.O'd

        direction walking = direction::NONE;
        direction lastWalking = direction::NONE;

        int combo = 0;//the number of hits in this fighter's combo
        int juggle = 0;//the number of hits on the opposing fighter since they've landed

        //animation shit
        bool endActive = false;//whether the current active animation is ending
        bool endPassive = false;//whether the current passive animation is ending
        int activeFrame = 0;//the frame of the active animation
        int passiveFrame = 0;//the frame of the passive animation
        int currentFrame = 0;//the frame of the currently playing animation
        animType activeAnimation = animType::NONE;//the type of the active animation
        animType passiveAnimation = animType::NONE;//the type of the passive animation
        animType lastPassiveAnimation = animType::NONE;//the type of the passive animation on the last frame
        animType currentAnimation = animType::NONE;//the type of the current animation
        animType lastAnimation = animType::NONE;//the type of the last animation
        int passiveOrd = 0;
        int activeID = -1;//the ID of the active animation
        int passiveID = 1;//the ID of the passive animation
        int currentID = 0;//the ID of the current animation
        int lastID = 0;//the ID of the last animation
        int currentIndex = 0;//the index of the current animation

        //modifiable stats
        int maxHp = 1000;//the max hp of the fighter (in tenths of a percent)
        double speed = 3;//the speed of the fighter
        double airspeed = 4;//the airspeed of the fighter
        double grabMult = 1;//the damage multiplier applied to the fighter's grab hitboxes
        double meleeMult = 1;//the damage multiplier applied to the fighter's meelee hitboxes
        double weaponMult = 1;//the damage multiplier applied to the fighter's weapon hitboxes
        double projectileMult = 1;//the damage multiplier applied to the fighter's projectile hitboxes
        double meterMult = 1;//the meter multiplier applied to the fighter's meter gains
        double maxStamina = 50;//the hp equivalent of the fighter's max stamina
        double meleeRes = 0;//resistance to melee damage
        double projectileRes = 0;//resistance to projectile damage
        double grabRes = 0;//resistance to grab damage
        double weaponRes = 0;//resistance to weapon damage
        double projectileSpeed = 1;//the speed of the fighter's projectiles

        //ability flags
        bool lifeSteal = false;//gains life steal on meter moves
        bool ultBoost = false;//increased stats after using ultimate
        bool stanceChange = false;//can swap offenses with resistances using 68426c
        bool upThrow = false;//adds an upward throw
        bool rageBoost = false;//gains mult as hp decreases
        bool grabArmor = false;//gains armor during basic grab animation
        bool impactSprint = false;//replaces forward dash with a damaging sprint
        bool controlProjectiles = false;//direction held changes projectile speed
        bool longParry = false;//gives parry a longer window
        bool downDash = false;//adds an aerial down dash
        bool burst = false;//adds 1 burst meter
        bool launcher = false;//half circle forward performs a launcher
        bool poisonGrab = false;//grabs poison the opponent, dealing extra damage over time
        bool stunProjectiles = false;//projectiles stun the opponent, leaving them vulnerable for longer
        bool cCanceling = false;//can cancel out of anything w/ meter
        bool doubleDash = false;//gains a second aerial dash
        bool dashAttack = false;//gains a new attack out of dash
        bool groundedHitbox = false;//adds a grounded hitbox on down aerial
        bool invincibleDash = false;//gives the character an invincible dash
        bool blockBreaker = false;//gives the character increased damage on block
        bool fieryMelee = false;//melee attacks burn the opponent, dealing a small amount of extra damage
        bool icyWeapons = false;//weapons freeze the opponent, slowing down their movement

        moveRequest currentRequest;
        bool callForQueueClear = false;

        //misc.
        void endPassiveAnim(bool reset = false) {
            endPassive = false;
            passiveFrame = 0;
            if (reset) {
                passiveAnimation = animType::idle;
            }

        }

        void endActiveAnim() {
            endActive = false;
            activeFrame = 0;
            activeAnimation = animType::NONE;
            activeID = -1;
            actionable = true;
            currentFrame = 0;
            px = 0;
            py = 0;
            momentumX = 0;
            momentumY = 0;

        }

        void cullDupeInputs(std::vector<int> &inputs) {
            int last = 0;
            numUsedInputs = 0;
            for (int i = 0; i < 9; i++) {
                inputsList[i] = util::NEUTRAL;
            }
            for (int i = 0; i <=inputs.size()-1 && numUsedInputs!=9; i++) {
                if (inputs[i] != last) {
                    inputsList[8-numUsedInputs] = inputs[i];
                    numUsedInputs++;
                }
                last = inputs[i];
            }
            for (int input:inputsList) {
                //std::cout << input <<" ";
            }
            //std::cout<<std::endl;
        }

        //checks if the move moveID can be used in the current state
        bool canBeUsed(int moveID, util::inputType startState, bool lenient) const {
            //std::cout<<"checking for usability "<< actionable<<" "<<currentID<<" "<<currentFrame<<std::endl;
            bool cancel;

            switch (animations[currentID].getAnimType()) {
                case animType::idle:
                case animType::walk1:
                case animType::walk2:
                case animType::crouch:
                case animType::crouched:
                case animType::uncrouch:
                    cancel = true;
                    break;
                case animType::dash1:
                case animType::dash2:
                case animType::dash3:
                case animType::dash4:
                    if (dashCoolDown>0) {
                        return false;
                    }
                    if (currentFrame > 14) {
                        cancel = true;
                    }
                    break;

                default:
                    break;
            }

            bool enoughMeter = true;
            bool state = ((y<=0) == (startState == util::G));
            //std::cout<<animations[moveID].getActive()<<" "<<state<<" "<<enoughMeter<<std::endl;
            return moveID >=0 && state && enoughMeter && (actionable || cancel);
        }

        //begins the specified move if it can be used
        void doAnimation(int moveID, util::inputType startState) {
            if (canBeUsed(moveID,startState,true)) {
                 currentAnimation = animations[moveID].getAnimType();
            }
        }

        //returns what the player could have "meant" with the given input
        std::vector<int> checkIntent (int input) const{
            switch (input) {
                case util::STRICT_NEUTRAL:
                    return {util::NEUTRAL, util::STRICT_NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::FRONT:
                    return {util::FRONT, util::NEUTRAL, util::STRICT_NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::BACK:
                    return {util::BACK, util::NEUTRAL, util::STRICT_NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::DOWN:
                    return {util::DOWN, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::UP:
                    return {util::UP, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::DOWN_FRONT:
                    return {util::DOWN_FRONT, util::DOWN, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::DOWN_BACK:
                    return {util::DOWN_BACK, util::DOWN, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::UP_FRONT:
                    return {util::UP_FRONT, util::UP, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                case util::UP_BACK:
                    return {util::UP_BACK, util::UP, util::NEUTRAL, (y <= 0) ? util::G : util::A};
                default:
                    return {input, (y <= 0) ? util::G : util::A};
            }

        }

        //PRECONDITION: Moves list is sorted in order of priority (longest first)
        void checkForAction(const sf::Clock &clock) {
            int tolerance = 2;
            std::vector<int> inputChecks(animations.size(), 0);//contains the check status of each input; -1 indicates it failed the check
            std::vector<int> skippedIns(animations.size(), 0);
            std::vector<bool> isLenient(animations.size(), false);

            std::vector<int> currentIns(inputsList.begin(), inputsList.end());//simply casts everything to int
            std::vector<int> possibleInputs;
            //int state = (y <= 0) ? util::G : util::A;
            /*for (int i = 0; i< animations.size(); i++) {
                const std::vector<int> & moveData= data::inputRefs[animations[i].getAnimType()];
                if (util::endsWithFromIndex(currentIns,moveData, 1)) {
                    currentRequest = {i,clock.getElapsedTime(), static_cast<util::inputType>(moveData[0])};
                    return;
                }
            }*/

            for (int i = currentIns.size()-1; i>=0; i--) {
                possibleInputs = checkIntent(currentIns[i]);
                for (int j = 0; j<animations.size(); j++) {
                    const std::vector<int> & moveData= data::inputRefs[animations[j].getAnimType()];
                    if (inputChecks[j] < moveData.size() && inputChecks[j] != -1) {

                        bool inputWorks = false;
                        for (int input : possibleInputs) {
                            if (moveData[moveData.size()-inputChecks[j] - 1] == input) {
                                inputWorks = true;
                                inputChecks[j] ++;
                                skippedIns[j] = 0;
                                break;
                            }
                        }

                        if (!inputWorks) {
                            if (moveData[moveData.size()-inputChecks[j] - 1] == util::G ||
                                moveData[moveData.size()-inputChecks[j] - 1] == util::A ||
                                (util::NORMAL<= possibleInputs[0] && possibleInputs[0] <= util::GRAB)||
                                data::leniencies[animations[j].getAnimType()] ==0){
                                inputChecks[j] = -1;

                            }
                            else if (data::leniencies[animations[j].getAnimType()] ==2 &&
                                (currentIns[i] == util::UP ||currentIns[i] == util::UP_BACK
                                    ||currentIns[i] == util::UP_FRONT)) {
                                inputChecks[j] = -1;
                            }
                            else {
                                skippedIns[j] ++;
                                isLenient[j] = true;
                            }
                        }
                    }

                    if (skippedIns[j] >= tolerance) {
                        inputChecks[j] = -1;
                    }
                }
            }

            for (int i = 0; i< animations.size(); i++) {
                animType Type = animations[i].getAnimType();
                if (inputChecks[i] == data::inputRefs[Type].size() && (!forceToIdle ||
                    (Type != animType::uncrouch && Type != animType::land))) {
                    currentRequest = {i,clock.getElapsedTime(), (y <= 0) ? util::G : util::A, isLenient[i]};
                    //std::cout<<"action detected: "<<animations[i].getInputID()<<" "<<animation::getAnimName(static_cast<animType>(animations[i].getInputID()))<<std::endl;
                    return;
                }
            }
        }

        void executeAction(const sf::Clock &clock) {
            bool inCancelWindow = false;//not yet implemented
            bool exc = false;
            if (canBeUsed(currentRequest.moveID, currentRequest.state, currentRequest.lenient)) {
                if (currentRequest.isValid() || (clock.getElapsedTime() - currentRequest.time)<=sf::seconds(0.1)) {
                    //std::cout << "action executed: "<<currentRequest.moveID << std::endl;
                    animation::Animation* anim = &animations[currentRequest.moveID];
                    switch (anim->getAnimType()) {
                        case animType::dash1:
                        case animType::dash2:
                        case animType::dash3:
                        case animType::dash4:
                            if (dashCoolDown>0) {
                                exc = true;
                            }
                            else {
                                dashCoolDown = 40;
                            }

                    }
                    if (animations[currentRequest.moveID].getActive()&&!exc) {
                        actionable = false;
                        activeAnimation = animations[currentRequest.moveID].getAnimType();
                        activeID = currentRequest.moveID;
                        activeFrame = 0;
                        callForQueueClear = true;
                        px = dx* animations[currentRequest.moveID].getMomentum();
                        py = dy* animations[currentRequest.moveID].getMomentum();
                    }
                    else {

                        switch (anim->getAnimType()) {
                            case animation::idle:
                                if (crouched) {
                                    uncrouching = true;
                                    crouched = false;
                                }
                                walking = util::direction::NONE;

                                break;
                            case animation::walk1:
                                walking = facing;
                                break;
                            case animation::walk2:
                                if (facing == direction::LEFT) {
                                    walking = direction::RIGHT;
                                }
                                else {
                                    walking = direction::LEFT;
                                }

                                break;
                            case animation::air0:
                            case animation::air1:
                            case animation::air2:
                                break;
                            case animation::jump0:
                            case animation::jump1:
                            case animation::jump2:
                                if (y<=0 && !jumping && !landing) {
                                    jumping = true;
                                    passiveAnimation = anim->getAnimType();
                                    passiveFrame = 0;
                                }
                                break;
                            case animation::crouch:
                                if (!crouched && !crouching) {
                                    passiveFrame = 0;
                                    passiveAnimation = anim->getAnimType();
                                    passiveID = currentRequest.moveID;
                                    crouching = true;
                                    crouched = true;
                                }
                                break;
                            case animation::uncrouch:
                                if (crouched && !uncrouching) {
                                    passiveFrame = 0;
                                    passiveAnimation = anim->getAnimType();
                                    passiveID = currentRequest.moveID;
                                    passiveFrame = 0;
                                    uncrouching = true;
                                    crouched = false;
                                }
                                break;
                            case animation::crouched:
                            case animation::blockpoise:
                                if (!(crouching || uncrouching)) {
                                    crouched = true;
                                    crouching = false;
                                    uncrouching = false;
                                    forceToIdle = false;
                                }
                                break;
                            default:
                                break;

                        }
                    }
                    currentAnimation = animations[currentRequest.moveID].getAnimType();
                    currentID = currentRequest.moveID;
                    currentFrame = 0;
                    currentRequest = {};
                }
            }
        }

    public:
        Fighter() : Actor(), currentRequest(3) {
            hitStun = 0;
            blockStun = 0;
            knockedDown = 0;
            knockBackTime = 0;
            knockBackX = 0;
            knockBackY = 0;
            momentumX = 0;
            momentumY = 0;
            EXmeter = 0;
            hp = 0;
        }


        Fighter(std::vector<std::string> moveset) : Actor(moveset), currentRequest(3){
            hitStun = 0;
            blockStun = 0;
            knockedDown = 0;
            knockBackTime = 0;
            knockBackX = 0;
            knockBackY = 0;
            momentumX = 0;
            momentumY = 0;
            EXmeter = 0;
            hp = 0;
            std::sort(animations.begin(), animations.end(),
                [](const animation::Animation& a, const animation::Animation& b) {
                    return static_cast<int>(a.getInputID()) > static_cast<int>(b.getInputID());
                }
            );

        }

        Fighter(data::FighterBuilder &fb) : Actor(fb.moveFiles), currentRequest(3){

        }

        void takeInput(std::vector<int> &inputs, const sf::Clock &clock) {
            cullDupeInputs(inputs);
            checkForAction(clock);
            executeAction(clock);
        }

        void update() override {
            passiveOrd = 0;
            if (hitStun<=0 && blockStun<=0) {
                //passiveID = 0;
                if (landing) {
                    dx = 0;
                    y = 0;
                    dy = 0;
                    px = 0;
                    py = 0;
                    actionable = false;
                    passiveAnimation = animType::land;
                    if (endPassive) {
                        landing = false;
                        actionable = true;
                        passiveFrame = 0;
                    }
                }

                else if (jumping) {
                    actionable = false;
                    std::cout<<"jumping: "<<y<<" "<<dy<<" "<<py<<std::endl;
                    /*
                    if (lastWalking == direction::NONE) {
                        passiveAnimation = animType::jump1;
                    }

                    else if (lastWalking == facing) {
                        passiveAnimation = animType::jump2;
                    }

                    else {
                        passiveAnimation = animType::jump0;
                    }*/

                    if (endPassive) {
                        actionable = true;
                        jumping = false;
                        dy = jumpheight;
                        dx = airspeed * ((passiveAnimation == animation::jump0)?1:(passiveAnimation == animation::jump2)?-1:0);

                    }
                }


                else if (y > 0) {
                    std::cout <<"in the air: "<<y<<" "<< dy<<std::endl;
                    if (!animations[currentID].getActive()) {
                        if (dy>-6) {
                            dy -= gravity;
                        }
                        else {
                            dy = -6;
                        }

                    }
                    else {
                        py -= animations[currentID].getGravity(currentFrame)* gravity;
                    }
                    if (dx * (int)facing <-2) {
                        passiveAnimation = animType::air0;
                    }
                    else if (dx * (int)facing >2) {
                        passiveAnimation = animType::air2;
                    }
                    else {
                        passiveAnimation = animType::air1;
                    }

                    if (dy >=0) {
                        passiveFrame = 0;
                    }
                    else {
                        passiveFrame = 4;
                    }

                    if (y + dy + py <= 0 ) {
                        landing = true;
                        actionable = false;
                        endActiveAnim();
                        endPassiveAnim();
                        y = 0;
                        dy = 0;
                        passiveFrame = 0;
                        std::cout<<"landing start: "<<y<<" "<<dy<<std::endl;

                    }

                }


                else if (y<=0) {
                    y = 0;
                    //std::cout<<"walking: "<< static_cast<int>(walking)<<std::endl;
                    if (crouching) {
                        dx = 0;
                        passiveAnimation = animType::crouch;
                        walking = direction::NONE;
                        crouched = true;
                        if (endPassive) {
                            crouched = true;
                            crouching = false;
                            endPassiveAnim();
                            passiveAnimation = animType::crouched;
                        }
                    }

                    else if (uncrouching) {
                        dx = 0;
                        passiveAnimation = animType::uncrouch;
                        walking = direction::NONE;
                        crouched = false;
                        if (endPassive) {
                            uncrouching = false;
                            endPassiveAnim();
                            passiveAnimation = animType::idle;
                            forceToIdle = true;
                        }
                    }

                    else if (crouched) {
                        forceToIdle = false;
                        dx = 0;
                        passiveAnimation = animType::crouched;
                        walking = direction::NONE;
                        if (endPassive) {
                            endPassiveAnim();
                        }
                    }

                    else {
                        if (walking == direction::NONE) {
                            passiveAnimation = animType::idle;
                            dx = 0;
                        }
                        else if (walking == facing) {
                            passiveAnimation = animType::walk1;
                            dx = speed * (int)walking * (activeID == -1);
                        }
                        else if (walking != facing) {
                            passiveAnimation = animType::walk2;
                            dx = speed * (int)walking* (activeID == -1);

                        }
                        if (endPassive) {
                            endPassiveAnim();
                        }
                    }
                }
            }
            else if (hitStun > 0) {

            }
            else{

            }
            if (lastPassiveAnimation != passiveAnimation) {
                passiveFrame = 0;
            }
            for (int i = 0; i< animations.size(); i++) {
                if (animations[i].getAnimType()==passiveAnimation && animations[i].getOrdinality() == passiveOrd) {
                    passiveID = i;


                    //std::cout<<"passiveId: "<<passiveID<<std::endl;

                }
            }

            passiveFrame++;
            if (activeID > -1) {
                activeFrame++;
                currentFrame = activeFrame;
                currentID = activeID;
                dx = animations[activeID].getXMove(currentFrame) * static_cast<int>(facing) + px;
                dy = animations[activeID].getYMove(currentFrame) + py;
                //std::cout <<"active current: "<<activeID<<" "<<animations.size()<<std::endl;
            }
            else {
                currentFrame = passiveFrame;
                currentID = passiveID;

                //std::cout <<"passive current: "<<passiveID<<" "<<animations.size()<<std::endl;
            }
            if (passiveFrame >= animations[passiveID].length-1) {
                endPassive = true;
            }
            if (activeID > -1 && currentFrame >= animations[activeID].length-1) {
                endActive = true;
                endActiveAnim();
                currentID = passiveID;
            }
            if (dashCoolDown>0) {
                dashCoolDown--;
            }
            x += dx;
            y += dy;
            outwardState[0] = x;
            outwardState[1] = y;
            outwardState[2] = static_cast<int>(facing);
            lastPassiveAnimation = passiveAnimation;
        }
        void doAction(std::vector<int> directionBuffer, std::vector<sf::Time> timeBuffer) {
        }

        int getCombo() {
            return combo;
        }
        double getMult(util::hitboxType type) {
            using util::hitboxType;
            switch (type) {
                case hitboxType::PUNCH:
                    return meleeMult;
                case hitboxType::KICK:
                    return meleeMult;
                case hitboxType::WEAPON:
                    return weaponMult;
                case hitboxType::PROJECTILE:
                    return projectileMult;
                case hitboxType::ENERGY:
                    return projectileMult;
                case hitboxType::BEAM:
                    return projectileMult;
                case hitboxType::GRAB:
                    return grabMult;
                case hitboxType::HIT_GRAB:
                    return grabMult;
                case hitboxType::THROW:
                    return grabMult;
                case hitboxType::OTHER:
                    return 1;
            }
        }

        //applies the proper values to the fighter on hit
        void hit(collision::HitBox &box) override {
            using util::hitboxType;
            double dmg = box.getDamage(enemy);
            switch (box.bType) {
                case hitboxType::PUNCH:
                    dmg *= 1-meleeRes;
                    break;
                case hitboxType::KICK:
                    dmg *= 1-meleeRes;
                    break;
                case hitboxType::WEAPON:
                    dmg *= 1-weaponRes;
                    break;
                case hitboxType::PROJECTILE:
                    dmg *= 1-projectileRes;
                    break;
                case hitboxType::ENERGY:
                    dmg *= 1-projectileRes;
                    break;
                case hitboxType::BEAM:
                    dmg *= 1-meleeRes;
                    break;
                case hitboxType::GRAB:
                    dmg *= 1-grabRes;
                    break;
                case hitboxType::HIT_GRAB:
                    dmg *= 1-grabRes;
                    break;
                case hitboxType::THROW:
                    dmg *= 1-grabRes;
                    break;
                case hitboxType::OTHER:
                    dmg *= 1-projectileRes;
            }

            hp -= static_cast<int>(dmg);

            if (armor <=0) {
                if (y+dy>0) {
                    knockBackX = box.knockBackXA;
                    knockBackY = box.knockBackYA;
                }
                else {
                    knockBackTime = box.knockBackTime;
                    knockBackX = box.knockBackX;
                    knockBackY = box.knockBackY;
                }

                if (blockHigh || blockLow) {
                    blockStun = box.blockStun;
                }
                else {
                    hitStun = box.hitStun;
                }
            }



        }

        direction getFacing() {
            return facing;
        }
        const sf::Texture* getTexture(int anim) {
            return animations[anim].getTexture();
        }

        const sf::Texture* getTexture() {
            return animations[currentID].getTexture();
        }


        sf::IntRect getTextureRect(int anim, int frame) {
            int fr = frame;
            int an = anim;
            if (frame == -1) {
                fr = currentFrame;
            }
            if (anim == -1) {
                an = currentID;
            }
            return animations[an].getFrame(fr);
        }

        sf::Vector2f getSpritePos(double camX, double camY) {
            float Xd;
            if (facing == direction::RIGHT) {
                Xd = x - camX - animations[currentID].getWidth() + animations[currentID].getAnchor();
            }
            else {
                Xd = x - camX + animations[currentID].getAnchor();
            }
            float Yd = y-camY + animations[currentID].getHeight()-24;
            return {Xd, Yd};
        }

        bool checkForClearCall() {
            return callForQueueClear;
        }

        void clearClearCall() {
            callForQueueClear = false;
        }



    };

    //Any projectile TODO: plan Projectile class
    class Projectile: public Actor {
    private:
    protected:
    public:
    };

    //Any placed object TODO: plan Placeable class
    class Placeable: public Actor {
    private:
    protected:
    public:
    };
}
namespace collision {

    //returns the amount of damage dealt by a hitbox before resistance is applied
    double HitBox::getDamage(actors::Fighter* own) {

        double realDamage = damage * own->getMult(bType);

        if (own->getCombo() <= 3) {
            return realDamage;
        }
        return realDamage * 1.0/(own->getCombo()-2.0);
    }

}



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

    void clearQueue(actors::Fighter & f) {
        inputDirections.clear();
        f.clearClearCall();
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
        sf::Sprite sprite;
        std::array<actors::Fighter, 2> fighters;
        Player player = Player();
    protected:
    public:
        CombatState() : GameState() {
            fighters[0] = actors::Fighter({
                "jump0","jump1","jump2","air0","air1","air2","land",
                "dash2", "dash1", "dash3", "dash4","walk1","walk2",
                "6b1A","5b1A","3a1A","2a1A",
                "crouch", "crouched","uncrouch",
                "4a1A","5a1A","6a1A","idle", "4b1A","j5a1A", "j4a1A", "j6a1A"});
            temp = sf::Texture();
            sprite = sf::Sprite();

        }

        void enter() override{}

        void handleEvents(const sf::Event &event, sf::Clock &clock) override {
            if (event.type == sf::Event::KeyPressed) {
                player.keysDown[event.key.code] = true;

                player.addInput(clock, event.key.code, KEY, true, fighters[0].getFacing());
            }
            if (event.type == sf::Event::KeyReleased) {
                player.keysDown[event.key.code] = false;

                player.addInput(clock, event.key.code, KEY, false, fighters[0].getFacing());
                    //player.printKeys();
            }
        }

        void update(sf::Clock &clock) override {
            player.updateCombatInputs(clock);
            if (fighters[0].checkForClearCall()) {
                player.clearQueue(fighters[0]);
            }

            player.printDirections();
            player.addDirection(clock, fighters[0].getFacing());
            std::vector<int> inputs = player.inputDirections.peekFullBack();
            fighters[0].takeInput(inputs,clock);
            fighters[0].update();
            for (int i = 0; i< sf::Keyboard::KeyCount; i++) {
                player.keysDownLast[i] = player.keysDown[i];
            }
        }

        void draw(sf::RenderWindow& window) override {
            sf::Vector2f pos = fighters[0].getSpritePos(0,0);
            //std::cout << pos.x << ", " << pos.y << std::endl;
            temp = *fighters[0].getTexture();
            sprite.setTexture(temp);
            sprite.setScale(4.0f * fighters[0].outwardState[2], 4.0f);
            sprite.setTextureRect(fighters[0].getTextureRect(-1,-1));
            sprite.setPosition(pos.x*4,graphics::windowSize.y-pos.y*4);
            window.draw(sprite);
        }

        void exit() override{}

        ~CombatState() override {}
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

    game::GameState* gs = new game::CombatState();

    while (gameWindow.isOpen()) {
        gameWindow.clear(sf::Color::Transparent);

        while (gameWindow.pollEvent(event)) {

            // Check for the "Close" request (e.g., clicking the 'X')
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            gs->handleEvents(event, clock2);

        }
        elapsedTime = clock.restart();
        timeSinceUpdate += elapsedTime;




        while (timeSinceUpdate > timePerFrame) {
            timeSinceUpdate -= timePerFrame;
            gs->update(clock2);
            //fighters[2].update();
            //st.update()
        }

        //yay

        gs->draw(gameWindow);

        gameWindow.display();
    }

    delete gs;
    return 0;
}