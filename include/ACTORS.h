//
// Created by wangs on 2/26/2026.
//

#ifndef NEWGAME_ACTORS_H
#define NEWGAME_ACTORS_H
#include <array>
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
                    return a>b;
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

        boolean equals(Fighter & other) {

        }



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

    //Any projectile TODO: plan Projectile class
    class Projectile: public Actor {
    private:
    protected:
        sf::Time lifespan;
        bool floorBounce;
        bool wallBounce;
        bool explodeOnEnd;


        sf::Time startTime;
        sf::Time currentTime;
        projectileState state;//the projectile's current state
        int currentFrame;//the current frame of the projectile's animation

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
    inline double HitBox::getDamage(actors::Fighter* own) {

        double realDamage = damage * own->getMult(bType);

        if (own->getCombo() <= 3) {
            return realDamage;
        }
        return realDamage * 1.0/(own->getCombo()-2.0);
    }

}
#endif //NEWGAME_ACTORS_H