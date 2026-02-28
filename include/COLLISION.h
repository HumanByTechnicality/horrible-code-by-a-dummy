//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_COLLISION_H
#define INCLUDE_COLLISION_H

#include "vector"
#include "SFML/Graphics.hpp"
#include "array"
#include "UTIL.h"
#include "DATA.h"

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
        HitBox(std::vector<std::array<int,4>> &bounds, std::vector<std::string> datLines) :  CollisionBox(bounds) {
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
#endif
