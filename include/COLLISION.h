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

    inline std::vector<std::array<int,4>> nullBounds = {};

    struct BoolAndRect {
        bool value;
        sf::Rect<double> rect;
    };

    enum hitType {
        H_none = 0,
        H_clean,
        H_clean_air,
        H_clash,
        H_blocked
    };

    enum hitCon {
        CON_none = 0,
        CON_any,
        CON_hit,
        CON_grab,
        CON_hitgrab,
        CON_whiff,
    };


    //a set of rectangles which defines a collider TODO: write CollisionBox class
    class CollisionBox{
    private:
    protected:

        bool exist = false;
        std::vector<sf::Rect<double>> localBounds = {};//the bounds of the collision box with respect to some origin
        std::vector<sf::Rect<double>> globalBounds = {};//the bounds of the collision box with respect to the stage

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
            localBounds.clear();
            globalBounds.clear();
            for (int i = 0; i<bounds.size(); i++) {
                localBounds.push_back(bounds[i]);
                globalBounds.push_back(bounds[i]);
            }
        }

        //changes globalBounds to account for the position of the origin TODO: write move method
        void move(int x, int y, bool direction){}

        //returns whether this collision box intersects with another collision box TODO: write intersects method
        bool intersects(CollisionBox &other) const{
            if (this->exist && other.exist) {
                for (const auto & globalBound : this->globalBounds) {
                    for (const auto & j : other.globalBounds) {
                        if (globalBound.intersects(j)) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }


        bool intersects(CollisionBox *other) const {
            if (this->exist && other->exist) {
                for (const auto & globalBound : this->globalBounds) {
                    for (const auto & j : other->globalBounds) {
                        if (globalBound.intersects(j)) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        BoolAndRect intersects(CollisionBox *other, sf::Rect<double>& collisionRegion) {
            if (this->exist && other->exist) {
                for (const auto & globalBound : this->globalBounds) {
                    for (const auto & j : other->globalBounds) {
                        if (globalBound.intersects(j,collisionRegion)) {
                            return {true,collisionRegion};
                        }
                    }
                }
            }
            return{ false,sf::Rect<double>(0,0,0,0)};
        }

        // --- Existence ---
        [[nodiscard]] bool exists() const { return exist; }
        void setExists(bool v) { exist = v; }

        // --- Facing ---
        [[nodiscard]] util::direction getFacing() const { return facing; }
        void setFacing(util::direction d) { facing = d; }

        // --- Bounds (local) ---
        [[nodiscard]] const std::vector<sf::Rect<double>>& getLocalBounds() const { return localBounds; }
        void setLocalBounds(const std::vector<sf::Rect<double>>& b) {
            localBounds.clear();
            for (sf::Rect<double> a : b) {
                localBounds.push_back(a);
            }
        }

        // --- Bounds (global) ---
        [[nodiscard]] const std::vector<sf::Rect<double>>& getGlobalBounds() const { return globalBounds; }
        void setGlobalBounds(const std::vector<sf::Rect<double>>& b) { globalBounds = b; }

        // --- Single-rect access ---
        [[nodiscard]] sf::Rect<double> getLocalRect(int i) const { return localBounds[i]; }
        void setLocalRect(int i, const sf::Rect<double>& r) { localBounds[i] = r; }

        [[nodiscard]] sf::Rect<double> getGlobalRect(int i) const { return globalBounds[i]; }
        void setGlobalRect(int i, const sf::Rect<double>& r) { globalBounds[i] = r; }

        // --- Single-rect add or remove ---
        void addLocalRect(const sf::Rect<double>& r) { localBounds.push_back(r); }
        void removeLocalRect(int i) { localBounds.erase(localBounds.begin()+i); }

        // --- Count ---
        [[nodiscard]] int getRectCount() const { return static_cast<int>(localBounds.size()); }

        void placeGlobalBounds(double x, double y, util::direction face) {
            if (exist == 1) {
                globalBounds = localBounds;
                facing = face;
                if ((int) face == -1) {
                    for (int i = 0; i< localBounds.size(); i++) {
                        globalBounds[i].left = x - localBounds[i].left - localBounds[i].width;
                        globalBounds[i].top = y + localBounds[i].top - localBounds[i].height + 1;
                    }
                } else {
                    for (int i = 0; i< localBounds.size(); i++) {
                        globalBounds[i].left = x + localBounds[i].left;
                        globalBounds[i].top = y + localBounds[i].top - localBounds[i].height + 1;
                    }
                }
                for (int i = 0; i< localBounds.size(); i++) {
                    globalBounds[i].width = std::abs(localBounds[i].width);
                    globalBounds[i].height = std::abs(localBounds[i].height);
                    //std::cout<<globalBounds[i].getPosition().x<<","<<globalBounds[i].getPosition().y<<" "<<globalBounds[i].getSize().x<<","<<globalBounds[i].getSize().y<<std::endl;
                }
            }
        }
    };

    enum knockDownType {
        KN_none = 0,
        KN_light,
        KN_heavy,
        KN_heavy_bounce,
        KN_heavy_AA,
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
        int knockDown;//whether this hitbox applies knockdown on hit
        bool isActive = true;

        int id;//the id of this hitbox

        int hitStun;//the amount of stun applied on a vulnerable target
        int blockStun;//the amount of stun applied on a blocking target

        damageType dType;//the type of damage this hitbox deals
        hitboxType bType;//the type of attack associated with this hitbox
        height bHeight;//the height of this hitbox
        util::weight bWeight;//the weight of the hitbox

        hitCon triggerCon;

        int trigger;//ID of the animation triggered on hit

        bool projectile = false;
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

        HitBox(std::vector<std::array<int,4>> &bounds, std::array<int,17> dat) :  CollisionBox(bounds) {
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
            bWeight = static_cast<util::weight>(dat[14]);
            triggerCon = static_cast<hitCon>(dat[15]);
            trigger = dat[16];
            exist = true;

        }

        HitBox(): CollisionBox(nullBounds) {
            exist = false;
        }

        HitBox(int id): CollisionBox(nullBounds) {
            this->id = id;
            exist = true;
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

        [[nodiscard]] util::weight getWeight() const { return bWeight; }
        void setWeight(util::weight h) { bWeight = h; }

        // --- Trigger animation ---

        [[nodiscard]] int getTriggerCon() const { return triggerCon; }

        [[nodiscard]] int getTriggerType() const { return trigger; }
        void setTrigger(int t) { trigger = t; }

        void setDirection(util::direction newFace){ facing = newFace;}

        void setActive(bool state) {isActive = state;}
        [[nodiscard]] bool getActive() const {return isActive;}

        void setProjectile(bool state) {projectile = state;}
        [[nodiscard]] bool isProjectile() const {return projectile;}
    };

    //TODO: plan HurtBox class
    class HurtBox: public CollisionBox {
    private:
        int id;
    protected:
    public:
        hurtboxType bType;

        HurtBox():
        CollisionBox(nullBounds), bType(hurtboxType::NONE) {
            exist = false;
        }

        HurtBox(int id):CollisionBox(nullBounds), bType(hurtboxType::NONE) {
            exist = false;
            this->id = id;
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
    inline bool intersects(CollisionBox &c1, CollisionBox &c2) {
        return c1.intersects(c2);
    }

    inline std::vector<HitBox> NULLHITBOXES = {};
    inline std::vector<HurtBox> NULLHURTBOXES = {};

    //TODO: ensure hitboxes are sorted in order of decreasing priority before calling this function
    /*inline HitBox* hitCheck(std::vector<HitBox> &HiB, std::vector<HurtBox> &HuB) {
        for (int i = 0; i<HiB.size(); i++) {
            for (int j = 0; j<HuB.size(); j++) {
                if (HiB[i].intersects(HuB[j])) {
                    return &HiB[i];
                }
            }
        }
        return &NULLHITBOXES[0];
    }*/

}
#endif
