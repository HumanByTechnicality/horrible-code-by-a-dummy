//
// Created by wangs on 2/20/2026.

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


//