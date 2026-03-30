//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_DATA_H
#define INCLUDE_DATA_H
#include "array"
#include "UTIL.h"
#include "vector"

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
        g4c = 89, g5c, g6c,

        grabthrow = 92,

        //grab reactions
        grabreaction = 93,
        throwreaction = 94,

        justblock = 95,
        blockpoise = 96,
        block = 97,


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

        ANIM_TYPE_COUNT,

        //projectile animations
        PROJECTILE_ANIM_START = 180,
        PROJECTILE_NONE,
        PROJECTILE_active_start,
        PROJECTILE_active_loop,
        PROJECTILE_explode,
        PROJECTILE_floor_bounce,
        PROJECTILE_wall_bounce,
        PROJECTILE_end,
        PROJECTILE_ANIM_END,

        PLACEABLE_ANIM_START = 190,
        PLACEABLE_NONE,
        PLACEABLE_active_start,
        PLACEABLE_active_loop,
        PLACEABLE_trigger,
        PLACEABLE_ANIM_END,



        ANIM_TYPE_END,
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

    /*the condition that must be met in an instructions list for it to progress to its next stage.
    a transition condition, if you will.*/
    enum transCon {
        NO_CONDITION = -1,
        on_end,
        on_cancel,
        on_hit,
        on_land,
        on_wall,
        CONDITION_COUNT
    };

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
        cCanceling, doubleDash, dashAttack, groundedHitbox, invincibleDash, blockBreaker,
        STAT_COUNT
    };

    //the types of modification that an upgrade can perform on a stat
    enum modifyType{ADD, SUBTRACT, MULTIPLY, DIVIDE, SET};

    //modifies stats in a fighterBuilder
    class Upgrade{
    public:
        std::string name;
        data::statID stat;
        modifyType type;
        double amount;
        int pointsUsed;
        double applyTo(double statToApply) {
            switch (type) {
                case ADD:
                    return statToApply + amount;
                case SUBTRACT:
                    return statToApply - amount;
                case MULTIPLY:
                    return statToApply * amount;
                case DIVIDE:
                    return statToApply / amount;
                case SET:
                    return amount;
            }
            return -1;
        }
        Upgrade(std::string name, data::statID stat, modifyType modify, double amount, int points) {
            this->name = std::move(name);
            this->stat = stat;
            this->amount = amount;
            this->type = modify;
            this->pointsUsed = points;
        }


    };

    struct UpgradeSet {
        std::vector<Upgrade> upgrades;
        std::vector<int> applyNum;
        int pointsUsed;
        std::array<int,4> archetypePoints;
    };
    struct Upgrades {
        //Basic upgrade modifiers
        Upgrade statsHP{"HP",maxHP,ADD,50,3};
        Upgrade statsHPNt{"HPn't",maxHP,ADD,50,3};

        Upgrade statsSpeed{"Speed",speed,ADD,0.5,3};
        Upgrade statsAirSpeed{"AirSpeed",airSpeed,ADD,0.5,3};
        Upgrade statsSpeedNt{"Speedn't",speed,SUBTRACT,0.5,-3};
        Upgrade statsAirSpeedNt{"AirSpeedn't",airSpeed,SUBTRACT,0.5,-3};

        Upgrade statsGrabMult{"Grab Pow",grabMult,ADD,0.01,3};
        Upgrade statsGrabRes{"Grab Res",grabRes,ADD,0.01,3};
        Upgrade statsGrabMultNt{"Grab Pown't",grabMult,SUBTRACT,0.01,-3};
        Upgrade statsGrabResNt{"Grab Resn't",grabRes,SUBTRACT,0.01,-3};

        Upgrade statsProjectileMult{"Projectile Pow",projectileMult,ADD,0.01,3};
        Upgrade statsProjectileRes{"Projectile Res",projectileRes,ADD,0.01,3};
        Upgrade statsProjectileMultNt{"Projectile Pown't",projectileMult,SUBTRACT,0.01,-3};
        Upgrade statsProjectileResNt{"Projectile Resn't",projectileRes,SUBTRACT,0.01,-3};

        Upgrade statsMeleeMult{"Melee Pow",meleeMult,ADD,0.01,3};
        Upgrade statsMeleeRes{"Melee Res",meleeRes,ADD,0.01,3};
        Upgrade statsMeleeMultNt{"Melee Pown't",meleeMult,SUBTRACT,0.01,3};
        Upgrade statsMeleeResNt{"Melee Resn't",meleeRes,SUBTRACT,0.01,3};

        Upgrade statsWeaponMult{"Weapon Pow", weaponMult, ADD, 0.01, 3};
        Upgrade statsWeaponRes{"Weapon Res", weaponRes, ADD, 0.01, 3};
        Upgrade statsWeaponMultNt{"Weapon Pow", weaponMult, SUBTRACT, 0.01, 3};
        Upgrade statsWeaponResNt{"Weapon Res", weaponRes, SUBTRACT, 0.01, 3};

        //Upgrade sets
        UpgradeSet rushDown{{//fast, light, and close-range
            statsSpeed, statsAirSpeed,
            statsMeleeMult,
            statsHPNt,
            },{
                1,1,
                10,
                1
            },0, {20,0,0,0}};

        UpgradeSet zoner{{//slow, defensive, and long-range
            statsSpeedNt, statsAirSpeedNt,
            statsProjectileMult,
            statsHPNt
            },{
                1,1,
                10,
                2
            },0,{0,20,0,0}};

        UpgradeSet grappler{{//slow, meaty, and close-range
            statsSpeedNt,statsAirSpeedNt,
            statsGrabMult,
            statsHP
            },{
                2,2,
                10,
                4
            },0,{0,0,20,0}};

        UpgradeSet shoto{//normal speed, frail, and mid-range
            {
                statsMeleeMult,statsProjectileMult,
                statsGrabMult, statsWeaponMult,
                statsHPNt
            },{
                2,2,
                2,2,
                1,

            }, 0, {0,0,0,0}};

        UpgradeSet hpSpec{{statsHP},{1}, 3};

        UpgradeSet speedSpec{{statsSpeed, statsAirSpeed},{3,1},3};

        UpgradeSet GrabSpec{{statsGrabMult},{5},3};

        UpgradeSet MeleeSpec{{statsGrabMult},{5},3};

        UpgradeSet ProjectileSpec{{statsGrabMult},{5},3};

        UpgradeSet WeaponSpec{{statsGrabMult},{5},3};

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
        std::vector<UpgradeSet> upgrades;
        std::vector<std::string> baseMoves;
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

    private:

        void setBaseMoves() {
            baseMoves = {
                "idle","walk1","walk2","crouch","uncrouch","crouched",
                "jump0","jump1","jump2","land",
                "dash1","dash2","dash3","dash4",
                "air0","air1","air2",
                "hit00","hit01","hit02",
                "hit10","hit11","hit12",
                "hit20","hit21","hit22",
                "jhit0","jhit1","jhit2",

            };
        }
        void setBuildStats() {
            buildStats[maxHP] = 1000;
            buildStats[speed] = 3;
            buildStats[airSpeed] = 4;
            buildStats[grabMult] = 1;
            buildStats[weaponMult] = 1;
            buildStats[meleeMult] = 1;
            buildStats[projectileMult] = 1;
            buildStats[meterMult] = 1;
            buildStats[maxStamina] = 200;
            buildStats[meleeRes] = 0;
            buildStats[projectileRes] = 0;
            buildStats[grabRes] = 0;
            buildStats[weaponRes] = 0;
            buildStats[projectileSpeed] = 1;
            for (int i = lifeSteal; i < STAT_COUNT; i++) {
                buildStats[i] = 0;
            }
        }
    };



    //DTO for Stages
    class StageBuilder {

    };
}
#endif