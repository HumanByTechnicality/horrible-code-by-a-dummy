//
// Created by wangs on 2/26/2026.
//

#ifndef NEWGAME_ACTORS_H
#define NEWGAME_ACTORS_H
#include <array>
#include <utility>
#include <vector>
#include "UTIL.h"
#include "ANIMATION.h"
#include "COLLISION.h"
#include "DATA.h"
#include "functional"
#include "algorithm"
#include <filesystem>
#include <iostream>
#include <windows.h>

#pragma once

namespace agents {
    class Player;
}

namespace actors {
    using util::direction;
    using animation::Animation;
    using animation::animType;

    enum status {
        S_NONE = -1,
        S_burning,
        S_poisoned,
        S_frozen,
        S_stunned,
        S_scouted,
        S_vampire,
        S_launched,
        S_time_stop,
        S_rage_mode,
        S_iron_skin,
        S_danger_zones,
        S_burned_out,
        S_positional_competence,
        S_guard_break,
        S_COUNT,
    };

    //The general class for anything that moves TODO: plan Actor class
    class Actor {
    private:
    protected:
        double x = 0;//x position of the actor on the screen
        double y = 0;//y position of the actor on the screen
        double dx = 0;//x velocity of the actor
        double dy = 0;//y velocity of the actor

        int currentID = 0;
        int currentFrame = 0;

        direction facing = direction::LEFT;//the direction the actor is facing

        std::vector<Animation> animations;//list of this actor's set of animations

        std::array<collision::HitBox*,3> *activeHitBoxes;//references to the hitboxes active for the fighter
        std::array<collision::HitBox*,12> HB = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
        std::array<collision::HurtBox*,3> *activeHurtBoxes;//references to the hurtboxes active for the fighter
        std::array<collision::HurtBox*,12> UB = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
        std::array<collision::CollisionBox*,3> *activePushBoxes;//refrences to the pushboxes active for the fighter

    public:
        const int actorId = 0;//the ID of this actor, to track owned objects

        std::array<int,3> outwardState;

        virtual ~Actor();

        Actor();

        Actor(std::vector<std::string> animList, int ID);


        virtual void update();

        virtual collision::hitType hit(collision::HitBox &box);

        virtual std::array<collision::HitBox*,12>* getHitBoxes() {
            HB = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
            auto hb = animations[currentID].getActiveHitBoxes();
            std::copy(hb->begin(), hb->begin()+3, HB.begin());
            return &HB;
        };

        virtual std::array<collision::HurtBox*,12>* getHurtBoxes() {
            UB = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
            auto hb = animations[currentID].getActiveHurtBoxes();
            std::copy(hb->begin(), hb->begin()+3, UB.begin());
            return &UB;
        };

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
    };

    Actor::~Actor() = default;

    Actor::Actor() {
    }

    Actor::Actor(std::vector<std::string> animList, int ID): actorId(ID)  {
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

    void Actor::update() {
        x += dx;
        y += dy;
        outwardState[0] = x;
        outwardState[1] = y;
        outwardState[2] = static_cast<int>(facing);
        animations[currentID].updateBoxes(x,y,facing,currentFrame);
    }

    collision::hitType Actor::hit(collision::HitBox &box) {}

    class X : public Actor {
    public:
    };

    struct moveRequest {
        int moveID = -1;
        sf::Time time = sf::Time::Zero;
        util::inputType state = util::inputType::NO_INPUT;
        bool lenient = false;

        void clear() { moveID = -1; time = sf::Time::Zero;}
        bool isValid() const { return moveID != -1; }
    };

    enum projectileState {
        NULL_PROJECTILE = -1,
        spawning,
        active,
        exploding,
        wallBounce,
        floorBounce,
        ending,
        PROJECTILE_STATE_COUNT
    };

    enum projectileBehavior {
        PB_linear = 0,
        PB_arc,
        PB_accel,
        PB_float,
        PB_seek,
        PB_wavy,
    };

    class Projectile;

    class ProjectileType: public Actor {
        friend class Projectile;
    private:
    protected:
        int lifeSpan = 0;


        int maxNum = 0;
        //std::vector<Projectile*> activeProjectiles;

        bool floorBounce = false;
        bool wallBounce = false;
        bool explodeOnEnd = true;

        projectileBehavior Pb = PB_linear;

        std::array<float,4> PbParams;

        float spawnVx[5] = {0,0,0,0,0};
        float spawnVy[5] = {0,0,0,0,0};

        int id = -1;

        std::vector<std::vector<collision::HitBox>> ownedHitBoxes;

        void updateMember(Projectile* child);
    public:
        std::string filename = "";
        ProjectileType(const std::string& file, std::vector<std::string> anims):Actor(std::move(anims),0) {
            PbParams = {0,0,0,0};

            filename = file;


            std::string directory = "../";
            std::ifstream vals(directory + "__val/" + file + ".txt");
            if (!vals.is_open()) {
                //std::cout<<"error loading file: "<<file<<std::endl;
            }
            else {
                std::vector<std::string> lines;
                std::string line;
                const char *Line;

                while (std::getline(vals, line)) {
                    //set line values to lowercase
                    if (line.rfind("file:",0)!=0) {
                        std::transform(line.begin(), line .end(), line.begin(),
                        [](unsigned char c){ return std::tolower(c); });
                    }

                    auto flag = util::split(line, ':');

                    if (flag[0] == "bounce") {
                        auto dat = util::split(flag[1],' ');
                        floorBounce = stoi(dat[0]);
                        if (dat.size()>1) {
                            wallBounce = stoi(dat[1]);
                        }
                    }

                    else if (flag[0] == "id") {
                        id = stoi(flag[1]);
                    }

                    else if (flag[0] == "behavior") {
                        auto dat = util::split(flag[1],' ');
                        Pb = static_cast<projectileBehavior>(stoi(dat[0]));

                        int statnum = 0;

                        while (statnum < dat.size()-1 && statnum < 4) {
                            PbParams[statnum] = stof(dat[statnum+1]);
                            statnum++;
                        }
                    }

                    else if (flag[0] == "velocity") {
                        auto dat = util::split(flag[1],' ');
                        for (int i = 0; i<5 && i< dat.size(); i++) {
                            auto parts = util::split(dat[i],'x');
                            if (parts.size()>0) spawnVx[i] = stof(parts[0]);
                            if (parts.size()>1) spawnVy[i] = stof(parts[1]);
                        }
                    }

                    else if (flag[0] == "lifespan") {
                        lifeSpan = stoi(flag[1]);
                    }



                }
            }

            //mark all hitboxes as pertaining to a projectile
            for (auto& anim: animations) {
                anim.markProjectiles();
            }

            //set hitboxes vector
            setHB();
        }

        [[nodiscard]] float getVx(int bucket) const {
            if (bucket <0 || bucket > 4) {
                return 0xbad;
            }
            return spawnVx[bucket];
        }

        [[nodiscard]] float getVy(int bucket) const {
            if (bucket <0 || bucket > 4) {
                return 0xbad;
            }
            return spawnVy[bucket];
        }

        [[nodiscard]] int getID() const {return id;}

        [[nodiscard]] ProjectileType* getThis() {
            return this;
        }

        void setHB() {
            for (auto& anim:animations) {
                ownedHitBoxes.push_back(anim.getHitBoxes());
            }
        }

        [[nodiscard]] std::vector<std::vector<collision::HitBox>> getHB() const {
            return ownedHitBoxes;
        }
        [[nodiscard]] int getHBSize() const {
            return ownedHitBoxes.size();
        }
        [[nodiscard]] int getHBSize(int idx) const {
            return ownedHitBoxes[idx].size();
        }

        [[nodiscard]] collision::HitBox getHB(int anim, int num) const {
            return ownedHitBoxes[anim][num];
        }

        int getHbIdxOnFrame(int anim, int frame) {
            if (anim >= animations.size() || frame >= animations[anim].getLength()) {return -1;}
            return animations[currentID].getHbIdxOnFrame(frame);
        }

        ~ProjectileType() {
            std::cout << "CRITICAL: ProjectileType at " << (void*)this << " is being DESTROYED!" << std::endl;
        }

        void printThis() {
            //std::cout<<"ProjectileType at "<<this<<std::endl;
        }

    };

    //Any projectile TODO: plan Projectile class
    class Projectile: public Actor {
        friend class ProjectileType;
    private:
    protected:
        bool endAnim = false;

        int startTime = 0;
        int currentTime = 0;
        std::array<std::array<collision::HitBox,3>,6> hitBoxes;

        std::array<int,3> HbIdx;

        projectileState state = NULL_PROJECTILE;//the projectile's current state
        int owner = -1;//the projectile's owner's ID
        int currentFrame = -1;//the current frame of the projectile's animation

        ProjectileType* parent = nullptr;
        std::array<collision::HitBox*,3> activeHB;


    public:

        Projectile() {
            state = NULL_PROJECTILE;
        }

        void spawn(ProjectileType* daParent, int bucket, util::direction face) {
            state = spawning;
            dx = daParent->getVx(bucket);
            dy = daParent->getVy(bucket);
            facing = face;

            setParent(daParent);
            //std::cout << "DEBUG: Projectile updating with parent at: " << (void*)parent << std::endl;
            //daParent.activeProjectiles.push_back(this);
            currentFrame = 0;
            startTime = 0;
            currentTime = 0;
        }

        void despawn() {
            parent = nullptr;
            state = NULL_PROJECTILE;
        }

        void place(double placeX, double placeY, direction placeFace) {
            state = active;
            x = placeX;
            y = placeY;
            dx *= (int)placeFace;
            facing = placeFace;
        }

        void setParent(ProjectileType* parent) {
            hitBoxes = {
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
                collision::HitBox(),collision::HitBox(),collision::HitBox(),
            };
            for (int i = 0; i<6 && i< parent->getHBSize(); i++) {
                for (int j = 0; j<3 && j< parent->getHBSize(i); j++) {
                    hitBoxes[i][j] = parent->getHB(i,j);
                }
            }
            this->parent = parent;
        }

        void resetParent(ProjectileType* parent) {
            this->parent = parent;
        }

        void update() override {
            //does nothing because I needed different arguments. is here for consistency.
        }

        void projectileUpdate(double camX, double camY) {
            //std::cout << "DEBUG: Projectile updating with parent at: " << (void*)parent << std::endl;
            if (parent != nullptr && parent != NULL) {
                parent->updateMember(this);
                HbIdx = {-1,-1,-1};
                if (state != NULL_PROJECTILE) {
                    HbIdx[0] = parent->getHbIdxOnFrame(currentID, currentFrame);
                }
                switch (state) {
                    case active:
                        currentID = 0;
                        if (endAnim) {
                            currentFrame = 0;
                            endAnim = false;
                        }
                        //std::cout<<"active"<<std::endl;
                        //std::cout<<"zero"<<std::endl;
                        //std::cout<<"one"<<std::endl;

                        for (int i = 0;i<3; i++) {
                            if (HbIdx[i] == -1) break;
                            if (i<HbIdx.size() && HbIdx[i]>=0) {
                                hitBoxes[currentID][HbIdx[i]].placeGlobalBounds(x,y,facing);
                                activeHB[i] = &hitBoxes[currentID][HbIdx[i]];
                            }
                            else {
                                activeHB[i] = nullptr;
                            }
                        }
                        //std::cout<<"two"<<std::endl;
                        break;
                    case exploding:
                        currentID = 1;
                        if (endAnim) {
                            despawn();
                            endAnim = false;
                        }
                        //std::cout<<"exploding"<<std::endl;
                        break;
                }
                currentFrame ++;
                if (parent != nullptr && currentFrame >= parent->animations[currentID].getLength()-1) {
                    endAnim = true;
                }
            }



            currentTime ++;
            outwardState[2] = int(facing);
        }

        void doHit() {
            if (parent!= nullptr && parent->explodeOnEnd) {
                state = exploding;
                currentFrame = 0;
                currentID = 1;
            }
            else {
                despawn();
            }
        };



        const sf::Texture* getTexture(int anim) {
            return parent->getTexture(anim);
        }

        const sf::Texture* getTexture() {
            return parent->getTexture(currentID);
        }

        sf::IntRect getTextureRect(int anim, int frame) {
            return parent->getTextureRect(anim, frame);
        }

        sf::IntRect getTextureRect() {
            return parent->getTextureRect(currentID, currentFrame);
        }

        [[nodiscard]] projectileState getState() const{
            return state;
        }

        sf::Vector2f getSpritePos(double camX, double camY) {
            if (parent != nullptr) {
                float Xd;
                if (facing == direction::RIGHT) {
                    Xd = x - camX - parent->animations[currentID].getAnchor();
                }
                else {
                    Xd = x - camX + parent->animations[currentID].getAnchor();
                }
                float Yd = y-camY + parent->animations[currentID].getHeight();
                return {Xd, Yd};
            }
            return {-200,-200};
        }

        collision::HitBox* getHitBoxes(int idx) {
            return activeHB[idx];
        };

    };

    void ProjectileType::updateMember(Projectile* child) {
        switch (child->state) {
            case NULL_PROJECTILE:
            case PROJECTILE_STATE_COUNT:
            case spawning:
                break;
            case active:
                if (child->currentTime - child->startTime < lifeSpan) {
                    switch (Pb) {
                        case PB_linear:
                            child->dx = child->dx;
                            child->dy = child->dy;
                            break;
                        case PB_arc:
                            child->dy -= PbParams[0];
                            break;
                        case PB_accel:
                            child->dx += PbParams[0] * (int)child->facing;
                            child->dy += PbParams[1];
                            break;
                        case PB_float:
                            child->dx *= PbParams[0];
                            child->dy *= PbParams[1];
                            break;
                        case PB_seek:
                        case PB_wavy:
                            //std::cout<<"this behavior is not yet implemented"<<std::endl;
                            break;
                    }

                    child->y+= child->dy;
                    child->x+= child->dx;

                    if (child->y<=0) {
                        if (!floorBounce) {
                            if (explodeOnEnd) {
                                child->state = exploding;
                            }
                            else {
                                child->state = ending;
                            }
                        }
                        else {
                            child->y *= -1;
                            child->dy += -1;
                        }
                    }

                }
                else {
                    child->despawn();
                }
                break;
            case exploding:
                break;
            default:
                break;
        }
        if (child->getState() != NULL_PROJECTILE) {
            child->resetParent(this);
        }
    }


    //Any placed object TODO: plan Placeable class
    class Placeable: public Actor {
    private:
    protected:
    public:
    };

    inline const animType hitReacts[3][3] =
        {{animType::hit00, animType::hit01, animType::hit02},
        {animType::hit10, animType::hit11, animType::hit12},
        {animType::hit20, animType::hit21, animType::hit22}};

    inline const bool doesExecute[6][5] =
        //vert:hitCon, hori:hitType
        //none, clean, clean air, clash, blocked
        {{false,false,false,false,false},//none
        {false,true,true,true,true},//any
        {false,true,true,true,false},//hit
        {false,true,false,true,true},//grab
        {false,true,false,false,false},//hitgrab
        {true,false,false,false,false}//whiff
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
        int knockedDown = -1;//frames of knockdown left
        int knockDownType = 0;//type of knockdown
        int knockBackTime = 0;//the amount of time that knockback will be applied over
        int knockBackX = 0;//the amount of knockback in the x direction
        int knockBackY = 0;//the amount of knockback in the y direction
        double momentumX = 0;//the amount of momentum in the x direction
        double momentumY = 0;//the amount of momentum in the y direction
        double px = 0;
        double py = 0;
        int hp = 500;//the current health of the fighter (in tenths of a percent)
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
        bool skipnextRequect = false;
        bool gettingUp = false;
        int scouted = 0;//lowers the highest stat
        bool exhausted = false;//prevents all meter gain for the rest of the round
        bool reactiveArmor = false;// produces upgrade points upon  being hit, + hyper armor
        bool overclocked = false; // allows cancelling out of all moves


        direction walking = direction::NONE;
        direction lastWalking = direction::NONE;

        int combo = 0;//the number of hits in this fighter's combo
        int juggle = 0;//the number of hits on the opposing fighter since they've landed
        int framesSinceHit;//number of frames since the fighter was last hit
        animType hitReaction = animType::NONE;

        int meter = 0;
        int maxMeter = 80;

        //animation shit
        bool endActive = false;//whether the current active animation is ending
        bool endPassive = false;//whether the current passive animation is ending
        bool frontOverride = false;
        int activeFrame = 0;//the frame of the active animation
        int passiveFrame = 0;//the frame of the passive animation
        animType activeAnimation = animType::NONE;//the type of the active animation
        animType passiveAnimation = animType::NONE;//the type of the passive animation
        animType lastPassiveAnimation = animType::NONE;//the type of the passive animation on the last frame
        animType currentAnimation = animType::NONE;//the type of the current animation
        animType lastAnimation = animType::NONE;//the type of the last animation
        collision::hitType hasHit = collision::H_none;//the nature of this animation's hits
        int passiveOrd = 0;
        int activeOrd = 0;
        int activeID = -1;//the ID of the active animation
        int passiveID = 1;//the ID of the passive animation
        int lastID = 0;//the ID of the last animation
        int currentIndex = 0;//the index of the current animation

        //modifiable stats
        int maxHp = 1500;//the max hp of the fighter (in tenths of a percent)
        double speed = 3;//the speed of the fighter
        double airspeed = 4;//the airspeed of the fighter
        double grabMult = 1;//the damage multiplier applied to the fighter's grab hitboxes
        double meleeMult = 1;//the damage multiplier applied to the fighter's meelee hitboxes
        double weaponMult = 1;//the damage multiplier applied to the fighter's weapon hitboxes
        double projectileMult = 1;//the damage multiplier applied to the fighter's projectile hitboxes
        double meterMult = 1;//the meter multiplier applied to the fighter's meter gains
        double maxStamina = 200;//the hp equivalent of the fighter's max stamina
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

        bool rageMode = false;
        bool ironSkin = false;
        bool dangerZones = false;
        bool overclock = false;
        bool vampiric = false;
        bool superReflect = false;
        bool scouting = false;

        moveRequest currentRequest;//the current move request pending
        bool callForQueueClear = false;//flag that signals the player object to clear its inputs queue

        std::vector<ProjectileType> projectileTypes;
        std::array<Projectile, 16> projectiles;

        std::array<int,12> projectileOwners = {0,0,0,0,0,0,0,0,0,0,0,0};
        //std::array<int, 16> projectileActives = {};//the fighter's active projectiles


        collision::CollisionBox * activePushBox = nullptr;
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
            px = 0;
            py = 0;
            momentumX = 0;
            momentumY = 0;
            currentFrame = passiveFrame;
            currentID = passiveID;
            hasHit = collision::H_none;
            if (!inputsList.empty() && inputsList[0]>3) {
                crouched = false;
            }
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
            bool cancel = false;
            switch (hasHit){
                case collision::H_clean:
                case collision::H_clean_air:
                    if (animations[moveID].getWeight()>animations[currentID].getWeight()) {
                        cancel = true;
                        //std::cout<<"CANCEL:"<<moveID<< " from " <<currentID<<std::endl;
                    }
                    break;
                default:
                    break;
            }

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
            if (currentFrame > animations[currentID].getCancelFrame() && moveID != currentID) {
                cancel = true;
                //std::cout<<"CANCEL:"<<moveID<< " from " <<currentID<<std::endl;
            }

            bool enoughMeter = (animations[moveID].getMeterUsed() * 10) <= meter;
            if (meter<0) {
                enoughMeter = true;
            }

            bool state = ((y<=0) == (startState == util::G));
            //std::cout<<animations[moveID].getActive()<<" "<<state<<" "<<enoughMeter<<std::endl;
            return moveID >=0 && state && true && (actionable || cancel);
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
            if (skipnextRequect) {
                skipnextRequect = false;
                return;
            }
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
                if (!gettingUp && (hitStun >0||knockedDown >-1)&&(int)Type>(int)animation::grabthrow) {

                }
                else if (inputChecks[i] == data::inputRefs[Type].size() && (!forceToIdle ||
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
            bool noExc = false;
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
                        landing = false;
                        //make unactionable
                        actionable = false;

                        //reset active parameters to start new animation
                        activeAnimation = animations[currentRequest.moveID].getAnimType();
                        activeID = currentRequest.moveID;
                        activeFrame = 0;
                        animations[activeID].reactivate();

                        meter -= animations[currentRequest.moveID].getMeterUsed() * 10;

                        //clear inputs queue
                        callForQueueClear = true;

                        //set momentum
                        px = dx* animations[currentRequest.moveID].getMomentum();
                        py = dy* animations[currentRequest.moveID].getMomentum();

                        //make it so that the animation has not hit yet.
                        hasHit = collision::H_none;

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
                                else {
                                    noExc = true;
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
                    if (!noExc) {
                        /*currentAnimation = animations[currentRequest.moveID].getAnimType();
                        currentID = currentRequest.moveID;
                        currentFrame = 0;
                        */
                        animations[currentRequest.moveID].reactivate();
                        currentRequest = {};
                        meter -= animations[currentRequest.moveID].getMeterUsed() * 10;
                    }
                }
            }
        }

    public:
        Fighter() : Actor(), currentRequest(3) {
            hitStun = 0;
            blockStun = 0;
            knockedDown = -1;
            knockBackTime = 0;
            knockBackX = 0;
            knockBackY = 0;
            momentumX = 0;
            momentumY = 0;
            meter = 0;
            maxMeter = 80;
            hp = 0;
        }


        Fighter(std::vector<std::string> moveset, int id = 0) : Actor(moveset, id), currentRequest(3){
            hitStun = 0;
            blockStun = 0;
            knockedDown = -1;
            knockBackTime = 0;
            knockBackX = 0;
            knockBackY = 0;
            momentumX = 0;
            momentumY = 0;
            meter = 0;
            maxMeter = 80;
            hp = maxHp;
            std::sort(animations.begin(),animations.end(),
                              [](const animation::Animation& a, const animation::Animation& b) {
                                  return static_cast<int>(a.getInputID()*100-a.getOrdinality()) > static_cast<int>(b.getInputID()*100-b.getOrdinality());
                              }
            );

            for (const auto& anim: animations) {
                auto files = anim.getProjectileNames();
                for (const auto& file: files) {
                    bool broken = false;
                    for (auto type: projectileTypes) {
                        if (type.filename == file) {
                            broken = true;
                            break;
                        }
                    }
                    if (!broken) {
                        projectileTypes.push_back(ProjectileType(file,{file+"_active",file+"_explode"}));
                    }
                }
            };

        }

        Fighter(data::FighterBuilder &fb, int id = 0) : Actor(fb.moveFiles, id), currentRequest(3){

        }

        void takeInput(std::vector<int> &inputs, const sf::Clock &clock) {
            cullDupeInputs(inputs);
            checkForAction(clock);
            executeAction(clock);
        }

        void update() override {
            passiveOrd = 0;
            if (hitStun<=0 && blockStun<=0 && knockedDown == -1) {
                //passiveID = 0;
                if (landing) {
                    landing = true;
                    dx = 0;
                    y = 0;
                    dy = 0;
                    px = 0;
                    py = 0;
                    actionable = false;

                    if (endPassive) {
                        if (!animations[currentID].getActive()) {
                            landing = false;
                            actionable = true;
                        }
                        passiveFrame = 0;
                    }

                    if (endActive) {
                        landing = false;
                        actionable = true;
                        endActiveAnim();
                    }
                }

                else if (jumping) {
                    actionable = false;
                    //std::cout<<"jumping: "<<y<<" "<<dy<<" "<<py<<std::endl;
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
                        dx = airspeed * static_cast<int>(facing) * ((passiveAnimation == animation::jump0)?-1:(passiveAnimation == animation::jump2)?1:0);

                    }
                }


                else if (y > 0) {
                    //std::cout <<"in the air: "<<y<<" "<< dy<<std::endl;
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
                        endPassiveAnim();
                        int id = animations[currentID].getLandAnim();
                        if (id == 02100) {
                            endActiveAnim();
                            passiveAnimation = animType::land;
                            passiveFrame = 0;
                        }
                        else {
                            int iter = (id-animations[currentID].getID()) * 0.1;
                            currentID += iter;
                            activeID = currentID;
                            activeFrame = 0;
                            activeAnimation = animations[activeID].getAnimType();
                            actionable = false;
                            landing = true;
                            endActive = false;
                        }
                        y = 0;
                        dy = 0;
                        //std::cout<<"landing start: "<<y<<" "<<dy<<std::endl;

                    }

                }


                else if (y<=0) {
                    y = 0;
                    //std::cout<<"walking: "<< static_cast<int>(walking)<<std::endl;
                    if (crouching) {
                        dx = 0;
                        passiveAnimation = animType::crouch;
                        walking = direction::NONE;
                        crouching = true;
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
                        crouched = true;
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
            else if (knockedDown >0 ) {
                if (framesSinceHit == 0) {
                    px = knockBackX;
                    py = knockBackY;
                }
                if (landing) {
                    py = 0;
                    dy = 0;
                    y = 0;
                    if (knockDownType == 0) {
                        activeAnimation = animType::lightLand;

                    }
                    else  {
                        activeAnimation = animType::heavyLand;
                    }
                    if (activeID > -1 && currentFrame >= animations[activeID].length-2) {
                        landing = false;
                        endActiveAnim();
                        activeAnimation = animType::knocked;
                    }
                    if (knockedDown>0) {
                        knockedDown--;
                    }
                }
                else if (y>0) {
                    landing = false;
                    activeAnimation = animType::knockDown;
                    py-=gravity;


                    if (dy+py>2) {
                        activeFrame = 4;
                    }
                    else if (dy+py > 0) {
                        activeFrame = 6;
                    }
                    else if (dy+py>-2) {
                        activeFrame = 8;
                    }
                    else {
                        activeFrame = 10;
                    }
                    if (y+dy+py <= 0) {
                        landing = true;
                        activeFrame = 0;
                        endActiveAnim();
                        activeAnimation = static_cast<animType>(animType::lightLand + knockDownType);
                    }
                }

                else if (y==0) {
                    activeAnimation = animType::knocked;
                    if (activeID > -1 && currentFrame >= animations[activeID].length-2) {
                        activeFrame = 0;
                    }
                    if (abs(px)>0) {
                        px -= util::sign(dx+px);
                    }
                    if (knockedDown>0) {
                        knockedDown--;
                    }
                    if (knockedDown == 0) {
                        dx = 0; px = 0;
                        dy = 0; py = 0;
                        activeFrame = 0;
                        activeAnimation = animType::getupG1;
                    }

                }

                for (int i = 0; i< animations.size(); i++) {
                    if (animations[i].getAnimType() == activeAnimation) {
                        activeID = i;
                        break;
                    }
                }
                framesSinceHit = 1000;
                //std::cout<<landing<<" "<<(int)(activeAnimation)<<" "<<activeFrame<<std::endl;
            }
            else if (knockedDown == 0) {
                activeAnimation = animType::getupG1;
                if (activeID > -1 && currentFrame >= animations[activeID].length-2) {
                    passiveAnimation = animType::idle;
                    passiveFrame = 0;
                    passiveOrd = 0;
                    y = 0;
                    landing = false;
                    activeFrame = 0;
                    activeID = -1;
                    activeAnimation = animType::NONE;
                    knockedDown = -1;
                    knockDownType = 0;
                    hitStun = 0;
                    actionable = true;
                }

            }
            else if (hitStun > 0) {
                actionable = false;

                if (y<=0) {
                    if (knockBackTime >0) {
                        knockBackTime--;
                        px = knockBackX;
                        py = knockBackY;
                    }
                    else {
                        knockBackTime = 0;
                        px = 0;
                        py = 0;
                    }

                }
                else {
                    knockBackTime = 0;
                    if (framesSinceHit == 0) {
                        px = knockBackX;
                        py = knockBackY;
                    }
                    else {
                        py -= gravity;
                    }
                    if (y+dy+py<=0 &&hitStun > 0) {
                        landing = true;
                        knockDownType = 0;
                        knockedDown = 40;
                        activeFrame = 0;
                        hitStun = 0;
                    }

                }
                if (hitReaction == animType::NONE) {
                    hitStun = 0;
                    activeFrame = 6;
                }
                else {
                    activeAnimation = hitReaction;
                    for (int i = 0; i< animations.size(); i++) {
                        if (animations[i].getAnimType() == hitReaction) {
                            activeID = i;
                            break;
                        }
                    }
                    if (framesSinceHit <=2) {
                        activeFrame = 2;
                    }
                    else if (framesSinceHit < hitStun * 2) {
                        activeFrame = 4;
                    }
                    else {
                        activeFrame = 6;
                    }
                }

                if (hitStun >0) {
                    hitStun --;
                }
                framesSinceHit++;
                //std::cout<<knockedDown<<std::endl;

                if (hitStun<=0) {
                    actionable = true;
                    endActiveAnim();
                    hitReaction = animType::NONE;
                    passiveFrame = 0;
                }
            }
            else if (blockStun > 0){

            }
            if (lastPassiveAnimation != passiveAnimation) {
                passiveFrame = 0;
            }
            for (int i = 0; i< animations.size(); i++) {
                if (animations[i].getAnimType()==passiveAnimation && animations[i].getOrdinality() == passiveOrd) {
                    passiveID = i;
                    //std::cout<<"passiveId: "<<passiveID<<std::endl;

                }
                /*if (animations[i].getAnimType()==activeAnimation) {
                    passiveID = i;
                    //std::cout<<"passiveId: "<<passiveID<<std::endl;

                }*/
            }

            passiveFrame++;
            if (activeID > -1) {
                activeFrame++;
                currentFrame = activeFrame;
                currentID = activeID;
                dx = animations[activeID].getXMove(currentFrame) * static_cast<int>(facing) + px;
                dy = animations[activeID].getYMove(currentFrame) + py;
                for (auto& spawn:animations[activeID].getSpawns(currentFrame)) {
                    bool broken = false;
                    for (auto &pr:projectiles) {
                        if (pr.getState() == NULL_PROJECTILE) {
                            for (int id = 0; id<projectileTypes.size(); id++) {
                                if (projectileTypes[id].getID() == spawn.projId) {
                                    //std::cout<<"projectile spawned: "<<id<<std::endl;
                                    pr.spawn(&projectileTypes[id],0,facing);
                                    pr.place(x+spawn.x * int(facing), y+spawn.y, facing );
                                    broken = true;
                                    break;
                                }
                            }
                            if (broken) {
                                break;
                            }
                        }
                    }
                }

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
            outwardState[0] = (int)x;
            outwardState[1] = (int)y;
            outwardState[2] = static_cast<int>(facing);

            if (projectileTypes.size() > 0) {
                projectileTypes.at(0).printThis();
            }

            for (auto & pr: projectiles) {
                if ((pr.getState() == active || pr.getState() == exploding) && pr.getState() != NULL_PROJECTILE) {
                    pr.projectileUpdate(0,0);
                }
            }

            animations[currentID].updateBoxes(x,y,facing,currentFrame);

            activeHitBoxes = animations[currentID].getActiveHitBoxes();
            activeHurtBoxes = animations[currentID].getActiveHurtBoxes();
            activePushBox = animations[currentID].getActivePushBox();

            lastPassiveAnimation = passiveAnimation;

            //std::cout<<currentFrame<<" "<<currentID<<" "<<activeID<<"||";
            /*catch (std::out_of_range &f) {
            }catch (std::bad_array_new_length &g) {
            }*/
        }

        void updateProjectiles(int frame);

        std::array<collision::HitBox*,12>* getHitBoxes() override{
            HB = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
            projectileOwners = {-1,-1,-1};
            auto hb = animations[currentID].getActiveHitBoxes();
            std::copy(hb->begin(), hb->begin()+3, HB.begin());
            int next = 3;
            for (int i = 0; i<9; i++) {
                if (projectiles[i].getState() == active) {
                    HB[next] = projectiles[i].getHitBoxes(0);
                    projectileOwners[next] = i;
                    next++;
                }
            }
            return &HB;
        };

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
                default:
                    return 0;
            }
        }
        int doHit(collision::HitBox* box, int id, collision::hitType nature = collision::H_blocked) {
            int weight = animations[currentID].getWeight();
            if (!box->isProjectile()) {
                hasHit = nature;
            }
            if (projectileOwners[id] == -1) {
                animations[currentID].doHit(box);
                if (doesExecute[static_cast<int>(box->getTriggerCon())][nature]){
                    activeFrame = 0;
                    activeAnimation = static_cast<animType>(box->getTriggerType()*0.01);
                    activeID+=(box->getTriggerType()%100)*0.1 - animations[currentID].getOrdinality();
                    animations[activeID].reactivate();
                    currentRequest = {};
                    hasHit = collision::H_none;
                }
            }
            else {
                projectiles[projectileOwners[id]].doHit();
            }

            if (nature == collision::H_blocked) {
                return -1;
            }
            meter ++;
            if (meter > maxMeter) {
                meter = maxMeter;
            }
            std::cout<<"Meter: "<<meter<<std::endl;
            return weight;


        }

        //applies the proper values to the fighter on hit
        collision::hitType hit(collision::HitBox &box) override {
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
                    knockBackX = box.knockBackXA *-1*static_cast<int>(box.facing);
                    knockBackY = box.knockBackYA;
                }
                else {
                    knockBackTime = box.knockBackTime;
                    knockBackX = box.knockBackX *-1*static_cast<int>(box.facing);
                    knockBackY = box.knockBackY;
                }

                if (blockHigh || blockLow) {
                    blockStun = box.blockStun;
                }
                else {
                    hitStun = box.hitStun;
                    framesSinceHit = 0;
                }

                int h = static_cast<int>(box.getHeight());
                int w = static_cast<int>(box.getWeight());
                hitReaction = hitReacts[h][w];
                activeAnimation = hitReaction;
                for (int i = 0; i< animations.size(); i++) {
                    if (animations[i].getAnimType() == hitReaction) {
                        activeID = i;
                        break;
                    }
                }
                currentFrame = 2;

                currentID = activeID;
            }

            bool blocking = false;
            if (blocking) {
                return collision::H_blocked;
            }

            return collision::H_clean;

        }

        collision::hitType hit(collision::HitBox* box) {
            using util::hitboxType;
            double dmg = box->getDamage(enemy);

            if (hitStun > 0) {
                combo++;
            }
            else {
                combo = 1;
                std::cout<<"combo broken, ";
            }
            std::cout<<"combo: " <<combo<< " hitstun: " <<hitStun<<std::endl;

            switch (box->bType) {
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
            if (hp<0){
                hp = 0;
            }

            if (armor <=0 ||true) {
                if (y+dy>0) {
                    knockBackX = box->knockBackXA *static_cast<int>(box->facing);
                    knockBackY = box->knockBackYA;
                }
                else {
                    knockBackTime = box->knockBackTime;
                    knockBackX = box->knockBackX *static_cast<int>(box->facing);
                    knockBackY = box->knockBackY;
                }


                if (blockHigh || blockLow) {
                    blockStun = box->blockStun;
                }
                else {
                    //std::cout<<hitStun<<" ";
                    hitStun = box->hitStun;
                    //std::cout<<hitStun<<std::endl;
                    framesSinceHit = 0;
                }

                if (y<=0 &&( box->knockDown<=1 || box->knockDown==4)) {
                    int h = static_cast<int>(box->getHeight());
                    int w = static_cast<int>(box->getWeight());
                    hitReaction = hitReacts[h][w];
                    knockDownType = 0;
                    knockedDown = -1;
                }
                else if (box->knockDown<=1 ) {
                    hitReaction = animType::jhit0;
                    knockDownType = 0;
                    knockedDown = -1;
                }
                else if (box->knockDown==collision::knockDownType::KN_heavy_AA) {
                    hitReaction = animType::knockDown;
                    knockDownType = 1;
                    knockedDown = 50;
                }
                else {
                    hitReaction = animType::knockDown;
                    knockedDown = 50;
                    dx = 2 * (int)box->facing;
                    dy = 6;
                }

                activeAnimation = hitReaction;
                activeID = 0;
                for (int i = 0; i< animations.size(); i++) {
                    if (animations[i].getAnimType() == hitReaction) {
                        activeID = i;
                        break;
                    }
                }
                currentFrame = 2;

                currentID = activeID;
            }

            if (hp<0) {
                hp = 0;
            }

            bool blocking = false;
            if (blocking) {
                return collision::H_blocked;
            }

            return collision::H_clean;



        }

        void setX(int newX) {
            x = newX;
        }

        direction getFacing() {
            return facing;
        }

        sf::Vector2f getSpritePos(double camX, double camY) {
            float Xd;
            if (facing == direction::RIGHT) {
                Xd = x - camX - animations[currentID].getAnchor();
            }
            else {
                Xd = x - camX + animations[currentID].getAnchor();
            }
            float Yd = y-camY + animations[currentID].getHeight();
            return {Xd, Yd};
        }

        std::array<std::vector<sf::Rect<double>>,2> getBoxes() {

            std::vector<sf::Rect<double>> Ubs;

            if (activeHurtBoxes != nullptr) {
                //std::cout<<"nonnullptr Ubs"<<std::endl;
                for (collision::HurtBox* b : *activeHurtBoxes) {
                    if (b != nullptr) {
                        auto rects = b->getGlobalBounds();
                        for (sf::Rect<double> r : rects)
                            Ubs.emplace_back(r.left * 4.0f, r.top * 4.0f, r.width * 4.0f, r.height * 4.0f);
                    }
                }
            }

            //std::cout<<Ubs.size()<<std::endl;

            std::vector<sf::Rect<double>> Hbs;

            if (activeHitBoxes!= nullptr) {
                //std::cout<<"nonnullptr Hbs"<<std::endl;
                for (collision::HitBox* b : *activeHitBoxes) {
                    if (b!= nullptr) {
                        auto rects = b->getGlobalBounds();
                        for (sf::Rect<double> r : rects)
                            Hbs.emplace_back(r.left * 4.0f, r.top * 4.0f, r.width * 4.0f, r.height * 4.0f);
                    }
                }
            }
            //std::cout<<Hbs.size()<<std::endl;

            /*if (currentAnimation == animType::g5a) {
                return{Ubs, Hbs};
            }*/
            return {Ubs, Hbs};
        }

        bool checkForClearCall() {
            return callForQueueClear;
        }

        void clearClearCall() {
            callForQueueClear = false;
        }

        boolean equals(Fighter & other) {

        }

        int getHitStunAndDown() {
            if (knockedDown>0) {
                return knockedDown;
            }
            return hitStun;
        }

        void turnTo(direction facing) {
            if (facing == this->facing) return;
            if (actionable && (y-dy<=0) && !landing && !jumping) {
                this->facing = facing;
            }
        }

        int renderFront() {
            return static_cast<int>(facing) + 3*(activeID!=-1) - 6 * (hitStun>0) - 12 * frontOverride;
        }

        std::vector<Projectile*> getProjectiles() {
            std::vector<Projectile*> projectiles;
            for (int i = 0; i < this->projectiles.size(); i++) {
                if (this->projectiles[i].getState() == active || this->projectiles[i].getState() == exploding) {
                    projectiles.push_back(&this->projectiles[i]);
                    //std::cout<<"pr "<<i<<" "<<projectiles[i]->getSpritePos(0,0).x<<" "<<projectiles[i]->getSpritePos(0,0).y;
                }
            }
            //std::cout<<std::endl;
            return projectiles;
        }

        double getHealthProp() {
            return double(hp)/maxHp;
        }

        bool getMeterFx() {
            return animations[currentID].getMeterFx();
        }

        int getMeter() {
            return meter;
        }

    };

}
namespace collision {

    //returns the amount of damage dealt by a hitbox before resistance is applied
    double HitBox::getDamage(actors::Fighter* own) {

        double realDamage = damage * own->getMult(bType);

        if (own->getCombo() <= 10) {
            return realDamage;
        }
        return realDamage * 1.0/(own->getCombo()-10.0);
    }

    struct hitData {
        HitBox* box;
        int prio;
        int id;
        sf::Rect<double> inteRECTion;//Yes, that is a pun. Shut up. You know what it means.
    };

    std::array<hitData,2> hitCheck(std::array<actors::Fighter*,2> fs) {
        std::array<HitBox*,2> box = {nullptr,nullptr};
        auto prio = std::array{-1,-1};
        auto rects = std::array{sf::Rect<double>(),sf::Rect<double>()};

        auto id = std::array{-1,-1};

        auto hitboxes = std::array{*fs[0]->getHitBoxes(),*fs[1]->getHitBoxes()};
        auto hurtboxes = std::array{*fs[1]->getHurtBoxes(),*fs[0]->getHurtBoxes()};
        //int maxPrio = -1;
        for (int i = 0; i<=1; i++) {
            int k = 0;
            for (HitBox* box1 : hitboxes[i]) {
                if (box1!=nullptr) {
                    if (box1->getActive()) {
                        for (HurtBox* box2 : hurtboxes[i]) {
                            if (box2!=nullptr) {
                                auto coll = box1->intersects(box2, rects[i]);
                                if (coll.value) {
                                    box[i] = box1;
                                    id[i] = k;
                                    break;
                                }
                            }
                        }
                    }
                }
                k++;
            }
        }

        return std::array{hitData{box[0],0,id[0], rects[0]},hitData{box[1],0,id[1],rects[1]}};
    }


}
#endif //NEWGAME_ACTORS_H
