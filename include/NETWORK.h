//
// Created by wangs on 4/26/2026.
//

#ifndef NEWGAME_NETWORK_H
#define NEWGAME_NETWORK_H
#include "UTIL.h"
#include "ANIMATION.h"
#include "ACTORS.h"
#include "DATA.h"
namespace network {
    using util::direction;
    using animation::Animation;
    using animation::animType;



    struct ActorSnapShot {
        double x;
        double y;
        double dx;
        double dy;

        direction facing;

        int currentFrame;
        int currentID;


        actors::Actor* actor;
    };

    struct ProjectileSnapShot : ActorSnapShot {

        int activeFrame;
        int passiveFrame;
        int passiveID;
        int activeID;

        actors::projectileState state;
        actors::ProjectileType* parent;
    };

    struct FighterSnapShot : ActorSnapShot {
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
        double EXmeter = 0;//the amount of EX meter that the fighter has built up
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

        int combo = 0;//the number of hits in this fighter's combo
        int juggle = 0;//the number of hits on the opposing fighter since they've landed
        int framesSinceHit;//number of frames since the fighter was last hit
        animType hitReaction = animType::NONE;

        //animation shit
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

        int rageMode = 0;
        int ironSkin = 0;
        int dangerZones = 0;
        int overclock = 0;
        int vampiric = 0;
        int superReflect = 0;
        int scouting = 0;

        actors::moveRequest currentRequest;//the current move request pending
        bool callForQueueClear = false;//flag that signals the player object to clear its inputs queue

    };


    class simulationPart {};

}

#endif //NEWGAME_NETWORK_H