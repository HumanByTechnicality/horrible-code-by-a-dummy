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

        // --- Existence ---
        [[nodiscard]] bool exists() const { return exist; }
        void setExists(bool v) { exist = v; }

        // --- Facing ---
        [[nodiscard]] util::direction getFacing() const { return facing; }
        void setFacing(util::direction d) { facing = d; }

        // --- Bounds (local) ---
        [[nodiscard]] const std::vector<sf::Rect<double>>& getLocalBounds() const { return localBounds; }
        void setLocalBounds(const std::vector<sf::Rect<double>>& b) { localBounds = b; }

        // --- Bounds (global) ---
        [[nodiscard]] const std::vector<sf::Rect<double>>& getGlobalBounds() const { return globalBounds; }
        void setGlobalBounds(const std::vector<sf::Rect<double>>& b) { globalBounds = b; }

        // --- Single-rect access ---
        [[nodiscard]] sf::Rect<double> getLocalRect(int i) const { return localBounds[i]; }
        void setLocalRect(int i, const sf::Rect<double>& r) { localBounds[i] = r; }

        [[nodiscard]] sf::Rect<double> getGlobalRect(int i) const { return globalBounds[i]; }
        void setGlobalRect(int i, const sf::Rect<double>& r) { globalBounds[i] = r; }

        // --- Count ---
        [[nodiscard]] int getRectCount() const { return static_cast<int>(localBounds.size()); }
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

        HitBox(std::vector<std::array<int,4>> &bounds, std::array<int,14> dat) :  CollisionBox(bounds) {
            id = dat[0];
            damage = dat[1];
            knockBackX = dat[2];
            knockBackY = dat[3];
            knockBackXA = dat[4];
            knockBackYA = dat[5];
            knockBackTime = dat[6];
            hitStun = dat[7];
            blockStun = dat[8];
            bHeight = static_cast<height>(dat[9]);
            dType = static_cast<damageType>(dat[10]);
            bType = static_cast<hitboxType>(dat[11]);
            knockDown = dat[12];
            grab = dat[13];
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

        // --- ID ---
        [[nodiscard]] int getID() const { return id; }
        void setID(int v) { id = v; }

        // --- Damage ---
        [[nodiscard]] double getDamage() const { return damage; }
        void setDamage(double v) { damage = v; }

        // --- Grab ---
        [[nodiscard]] bool isGrab() const { return grab; }
        void setGrab(bool v) { grab = v; }

        // --- Knockback ---
        [[nodiscard]] int getKnockBackX() const { return knockBackX; }
        void setKnockBackX(int v) { knockBackX = v; }

        [[nodiscard]] int getKnockBackY() const { return knockBackY; }
        void setKnockBackY(int v) { knockBackY = v; }

        [[nodiscard]] int getKnockBackXA() const { return knockBackXA; }
        void setKnockBackXA(int v) { knockBackXA = v; }

        [[nodiscard]] int getKnockBackYA() const { return knockBackYA; }
        void setKnockBackYA(int v) { knockBackYA = v; }

        [[nodiscard]] int getKnockBackTime() const { return knockBackTime; }
        void setKnockBackTime(int v) { knockBackTime = v; }

        [[nodiscard]] bool getKnockDown() const { return knockDown; }
        void setKnockDown(bool v) { knockDown = v; }

        // --- Stun ---
        [[nodiscard]] int getHitStun() const { return hitStun; }
        void setHitStun(int v) { hitStun = v; }

        [[nodiscard]] int getBlockStun() const { return blockStun; }
        void setBlockStun(int v) { blockStun = v; }

        // --- Types ---
        [[nodiscard]] util::damageType getDamageType() const { return dType; }
        void setDamageType(util::damageType t) { dType = t; }

        [[nodiscard]] util::hitboxType getHitboxType() const { return bType; }
        void setHitboxType(util::hitboxType t) { bType = t; }

        [[nodiscard]] util::height getHeight() const { return bHeight; }
        void setHeight(util::height h) { bHeight = h; }

        // --- Trigger animation ---
        [[nodiscard]] animation::animType getTriggerType() const { return trigger; }
        void setTriggerType(animation::animType t) { trigger = t; }

        int getTriggerID() const { return triggerID; }
        void setTriggerID(int v) { triggerID = v; }

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

        [[nodiscard]] hurtboxType getType() const { return bType; }
        void setType(hurtboxType t) { bType = t; }

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
