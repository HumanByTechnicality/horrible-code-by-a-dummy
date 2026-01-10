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

/*CONCEPT:
 * A game in which the player does not control the protagonist. rather, they
 * guide the protagonist through fighting game-like battles with a few set commands:
 *  2:attack mode (aggressive ai)
 *  3:defence mode (defensive ai)
 *  4:trick mode (tricky ai)
 *  trk>def,def>atk,atk>trk
 * All the non-normal modes are on the same cooldown, and last for a length decided
 * by the stats of the character. Every character is the same, save for their selection
 * of moves.
 * MOVES:
 * moves come in three types: attack, defense, and trick.
 *
 *
 *
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

    enum class direction {
        LEFT = -1,
        RIGHT = 1
    };

    enum class input {
        HARD_BACK = 0,
        DOWN_BACK = 1,
        DOWN = 2,
        DOWN_FRONT = 3,
        BACK = 4,
        NEUTRAL = 5,
        FRONT = 6,
        HARD_FRONT = 10,
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

//contains the stuff necessary for rendering
namespace graphics {

    const sf::Vector2u internalRes(320, 180); // the game's internal resolution
    sf::Vector2u windowSize(1280, 720); // actual size of the game window

    sf::RenderTexture rt; // the texture that the game renders to
}

namespace ui {

    //any ui element that can be selected TODO:plan Button class
    class Button {
    private:
    protected:
    public:
    };

    //any ui element that changes according to a state TODO:plan Indicator class
    class Indicator {
    private:
    protected:
    public:
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

    //a set of rectangles which defines a collider TODO: write CollisionBox class
    class CollisionBox{
    private:
    protected:
        std::vector<std::array<int,4>> nullBounds = {{0,0,0,0}};
        std::vector<sf::IntRect> localBounds;//the bounds of the collision box with respect to some origin
        std::vector<sf::IntRect> globalBounds;//the bounds of the collision box with respect to the stage
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
        explicit CollisionBox(std::vector<sf::IntRect> &bounds) {
            for (int i = 0; i<bounds.size(); i++) {
                localBounds.push_back(bounds[i]);
                globalBounds.push_back(localBounds.back());
            }
        }

        //changes globalBounds to account for the position of the origin TODO: write move method
        void move(int x, int y, bool direction){}

        //returns whether this collision box intersects with another collision box TODO: write intersects method
        bool intersects(CollisionBox &other) {
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

        int hitStun;//the amount of stun applied on a vulnerable target
        int blockStun;//the amount of stun applied on a blocking target

        damageType dType;//the type of damage this hitbox deals
        hitboxType bType;//the type of attack associated with this hitbox
        height bHeight;//the height of this hitbox
    protected:
    public:
        HitBox(std::vector<std::array<int,4>> &bounds) : CollisionBox(bounds) {

            exist = true;
        }

        HitBox(): CollisionBox(nullBounds) {
            exist = false;
        }


        //returns the damage type of the hitbox
        damageType getDType() {
            return dType;
        }

        //returns the damage of the hitbox
        double getDamage(actors::Fighter* own);
    };

    //TODO: plan HurtBox class
    class HurtBox: public CollisionBox {
    private:
    protected:
    public:
        HurtBox(): CollisionBox(nullBounds) {
            exist = false;
        }
    };

    //TODO: plan BlockBox class
    class BlockBox: public CollisionBox {
    private:
    protected:
    public:
    };

    //TODO: plan StageBox class
    class StageBox: public CollisionBox {
    private:
    protected:
    public:
    };

    //checks if two CollisionBoxes intersect each other TODO: write intersects method
    bool intersects(CollisionBox &c1, CollisionBox &c2) {
        return c1.intersects(c2);
    }

    std::vector<HitBox> NULLHITBOXES = {HitBox()};
    std::vector<HurtBox> NULLHURTBOXES = {HurtBox()};
}

namespace animation {

    class Animation {
    private:
    protected:
        std::vector<collision::HitBox> hitBoxes;//the set of hitboxes associated with this animation
        std::vector<collision::HurtBox> hurtBoxes;//the set of hurtboxes associated with this animation
        std::vector<collision::BlockBox> blockBoxes;//the set of blockboxes associated with this animation

        sf::Texture texture;//the texture associated with this animation

        std::vector<int> frameorder;//the order of frames in this animation

        int width;//the width of one frame
        int height;//the height of one frame
        int anchor;//the x anchor of one frame
        int xOffset;//the offset from x=0 of the top left corner of the first frame
        int yOffset;//the offset from y=0 of the top left corner of the first frame

        bool exist;
    public:
        Animation(std::string id) {
            //initialize default values (remove "garbage")
            width = 0;
            height = 0;
            anchor = 0;
            xOffset = 0;
            yOffset = 0;
            exist = false;

            std::string directory = "../";
            std::ifstream vals(directory + "/__val/" + id + ".txt");

            if (!vals.is_open()) {
                std::cerr << "Error opening file: " << directory << std::endl;
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
    };

    class moveBuffer {
    private:
    protected:
    public:
    };
}

namespace actors {
    using util::direction;
    using animation::Animation;

    //The general class for anything that moves TODO: plan Actor class
    class Actor {
    private:
    protected:
        int x = 0;//x position of the actor on the screen
        int y = 0;//y position of the actor on the screen
        double dx = 0;//x velocity of the actor
        double dy = 0;//y velocity of the actor

        direction facing = direction::LEFT;//the direction the actor is facing

        std::vector<Animation> animations;//list of this actor's set of animations

        std::vector<collision::HitBox> *activeHitBoxes;//references to the hitboxes active for the fighter
        std::vector<collision::HurtBox> *activeHurtBoxes;//references to the hurtboxes active for the fighter
        std::vector<collision::BlockBox> *activeBlockBoxes;//references to the hitboxes active for the fighter

        virtual void getHitBoxes() {

        }

        virtual void getHurtBoxes() {

        }
    public:
        virtual ~Actor() = default;

        Actor(std::vector<std::string> animList) {
            x = 0;//x position of the actor on the screen
            y = 0;//y position of the actor on the screen
            dx = 0;//x velocity of the actor
            dy = 0;//y velocity of the actor

            facing = direction::LEFT;

            activeHitBoxes = nullptr;
            activeHurtBoxes = nullptr;
            activeBlockBoxes = nullptr;

            for (int i = 0; i < animList.size(); i++) {
                animations.emplace_back(animList[i]);
            }
        }

        virtual void update() {
            x += dx;
            y += dy;
        }

        virtual void hit(collision::HitBox &box){}
    };

    //Any character TODO: plan Fighter class
    class Fighter: public Actor {
    friend class collision::CollisionBox;
    private:
        Fighter* enemy = this;
    protected:
        //general gameplay flags and values
        int hitStun = 0;//frames of hitstun left
        int blockStun = 0;//frames of blockstun left
        int knockedDown = 0;//frames of knockdown left
        int knockBackTime = 0;//the amount of time that knockback will be applied over
        int knockBackX = 0;//the amount of knockback in the x direction
        int knockBackY = 0;//the amount of knockback in the y direction
        int momentumX = 0;//the amount of momentum in the x direction
        int momentumY = 0;//the amount of momentum in the y direction
        double EXmeter = 0;//the amount of EX meter that the fighter has built up
        int hp = 0;//the current health of the fighter (in tenths of a percent)
        double gravity = 1;//the gravity of the fighter
        double armor = 0;//the armor hp equivalent of the fighter
        bool blockHigh = false;//whether the fighter is blocking high
        bool blockLow = false;//whether the fighter is blocking low
        bool landing = false;//whether the fighter is landing
        bool jumping = false;//whether the fighter is jumping
        bool crouching = false;//whether the fighter is crouching
        bool crouched = false;//whether the fighter is crouched
        bool isDead = false;//whether the fighter is K.O'd

        int combo = 0;//the number of hits in this fighter's combo
        int juggle = 0;//the number of hits


        //modifiable stats
        int maxHp = 1000;//the max hp of the fighter (in tenths of a percent)
        double speed = 5;//the speed of the fighter
        double grabMult = 1;//the damage multiplier applied to the fighter's grab hitboxes
        double meleeMult = 1;//the damage multiplier applied to the fighter's meelee hitboxes
        double weaponMult = 1;//the damage multiplier applied to the fighter's weapon hitboxes
        double projectileMult = 1;//the damage multiplier applied to the fighter's projectile hitboxes
        double maxStamina = 50;//the hp equivalent of the fighter's max stamina

        //upgrade stats
        double projectileSpeed = 1;//the speed of the fighter's projectiles
        double meterMult = 1;//the meter multiplier applied to the fighter's meter gains
        double bluntRes = 0;//resistance to blunt damage
        double sharpRes = 0;//resistance to sharp damage
        double pierceRes = 0;//resistance to sharp damage
        double magicRes = 0;//resistance to magic damage
        double energyRes = 0;//resistance to energy damage


        //ability flags
        bool rageBoost = false;//gains mult as hp decreases
        bool grabArmor = false;//gains armor during basic grab animation
        bool impactSprint = false;//replaces forward dash with a damaging sprint
        bool controlProjectiles = false;//direction held changes projectile speed
        bool longParry = false;//replaces parry with a longer window
        bool downDash = false;//adds an aerial down dash
        bool burst = false;//gains 1 burst meter
        bool launcher = false;//half circle forward performs a launcher
        bool poisonGrab = false;//grabs poison the opponent
        bool specialCanceling = false;//can cancel out of specials
        bool doubleDash = false;//gains a second aerial dash
        bool dashAttack = false;//gains a new attack out of dash
        bool groundedHitbox = false;//adds a grounded hitbox on down aerial
        bool invincibleDash = false;//gives the character an invincible dash
        bool blockBreaker = false;//gives the character increased damage on block
        bool fieryMelee = false;//adds fire to melee attacks, dealing a small amount of extra damage

    public:
        Fighter(std::vector<std::string> moveset) : Actor(moveset) {
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

        void update() override {
            if (hitStun<=0 && blockStun<=0) {
                if (landing) {

                }
                else if (jumping) {

                }
                else if (y==0) {
                    if (crouching) {

                    }
                    else if (crouched) {

                    }
                    else {

                    }
                }

            }
            else if (hitStun > 0) {

            }
            else{

            }
            x += static_cast<int>(dx);
            y += static_cast<int>(dy);
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
            using util::damageType;
            double dmg = box.getDamage(enemy);
            switch (box.dType) {
                case damageType::BLUNT:
                    dmg *= 1-bluntRes;
                    break;
                case damageType::SHARP:
                    dmg *= 1-sharpRes;
                    break;
                case damageType::PIERCE:
                    dmg *= 1-pierceRes;
                    break;
                case damageType::MAGIC:
                    dmg *= 1-magicRes;
                    break;
                case damageType::ENERGY:
                    dmg *= 1-energyRes;
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

//TODO: plan Player class
class Player {
private:
protected:
public:
};

int main() {

    //intialize necessary variables
    auto gameWindow = sf::RenderWindow(sf::VideoMode(graphics::windowSize.x,graphics::windowSize.y),"fightingCoach");//the window that the game is drawn to
    graphics::rt.create(graphics::internalRes.x, graphics::internalRes.y);

    while (gameWindow.isOpen()) {

    }
    return 0;
}