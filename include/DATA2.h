//
// Created by wangs on 3/19/2026.
//

#ifndef NEWGAME_DATA2_H
#define NEWGAME_DATA2_H
#include "ANIMATION.h"
#include "DATA.h"
#include "UTIL.h"
namespace data {

    /*class projectileInstructionHandler {
    public:
        std::vector<animation::transCon> conditions;
        std::vector<animation::animType> animations;
        std::vector<bool> checkOnlyOnState;
        std::vector<animation::animType> stateChanges;
        int currentState = 0;

    };*/

    //DTO for projectiles
    class ProjectileType {
    protected:
    public:
        bool explode;
        int lifeSpan;
        double gravity;
        double drag;
        sf::vector2f acceleration;


    };
}
#endif //NEWGAME_DATA2_H