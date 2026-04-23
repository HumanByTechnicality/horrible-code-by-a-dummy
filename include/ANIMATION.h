//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_ANIMATION_H
#define INCLUDE_ANIMATION_H
#include "UTIL.h"
#include "GRAPHICS.h"
#include <SFML/Graphics.hpp>
#include "string"
#include "vector"
#include "array"
#include "COLLISION.h"
#include "DATA.h"
#include "fstream"
#include "iostream"
#include "random"


namespace animation {
    struct TextureWrapper {
        std::string fileID;
        sf::Texture texture;
    };

    inline sf::Texture NullTexture;
    inline bool nullTextureLoaded = NullTexture.loadFromFile("../__pic/NULLTEXTURE.png");
    inline std::vector<TextureWrapper> textures = {TextureWrapper{}};

    struct spawnInfo {
        int projId;
        float x;
        float y;
    };

    class Animation {
    protected:
        int width; //the width of one frame
        int height; //the height of one frame
        int anchor; //the x anchor of one frame
        int xOffset; //the offset from x=0 of the top left corner of the first frame
        int yOffset; //the offset from y=0 of the top left corner of the first frame

        bool exist; //whether this animation has been properly initialized
        int ID = 0;
        int wrapperIndex = 0;

        bool active = false;


        animType animType;

        double momentumCarry = 0;

        std::vector<bool> frontFrames;//the set of frames where the animation will always be shown in behind
        std::vector<collision::HitBox> hitBoxes; //the set of hitboxes associated with this animation
        std::vector<collision::HurtBox> hurtBoxes; //the set of hurtboxes associated with this animation
        std::vector<collision::CollisionBox> pushBoxes; //the set of push boxes associated with this animation
        std::vector<std::vector<int> > hurtBoxOrder;
        std::vector<std::vector<int> > hitBoxOrder;
        std::vector<int> pushBoxOrder;
        std::array<collision::HitBox *, 3> activeHitBoxes = {nullptr, nullptr, nullptr};
        std::array<collision::HurtBox *, 3> activeHurtBoxes = {nullptr, nullptr, nullptr};
        collision::CollisionBox * activePushBox = nullptr;

        std::vector<std::vector<int>> cancels;//the ids of animations which can be cancelled into on any given frame

        std::vector<int> frameorder; //the order of frames in this animation, by layer

        std::vector<std::array<double, 2> > movements; //da movements
        std::vector<double> gravity; //when and how hard gravity applies to the move
        sf::Texture *texture; //the texture associated with this animation

        std::vector<std::vector<int>> projectileFrames;
        std::vector<std::string> projectileFiles;

        std::vector<spawnInfo> spawns;

        int weight = 0;


        int landAnim = 02100;
    public:
        std::vector<graphics::layerID> layers; //the layers covered by the animation
        int numLayers; //the number of layers the animation covers
        int frameNum; //the number of unique frames in an animation
        int length; //the length of the animation

        Animation(std::string animId) {
            //initialize default values (remove "garbage")
            landAnim = 02100;
            int parseType = 0;
            width = 0;
            height = 0;
            anchor = 0;
            xOffset = 0;
            yOffset = 0;
            momentumCarry = 0;
            exist = false;
            active = false;
            length = 0;

            int currentID = -1;

            int frameNum = 0;
            // temp hitbox data fields
            int damage = 0;
            int knockBackX = 0, knockBackY = 0;
            int knockBackXA = 0, knockBackYA = 0;
            int knockBackTime = 0;
            int hitStun = 0, blockStun = 0;
            int hHeight = 0, dType = 0, bType = 0,hWeight = 0;
            int knockDown = 0, grab = 0;
            int triggerType = 0, triggerID = 00000;

            TextureWrapper texture;

            std::vector<std::array<int, 4> > localBounds; // temp for pos: lines


            frameorder.reserve(400);
            hitBoxes.reserve(400);
            hurtBoxes.reserve(400);

            std::string directory = "../";
            std::ifstream vals(directory + "__val/" + animId + ".txt");

            if (!vals.is_open()) {
                std::cerr << "Error opening file: " << directory << std::endl;
            } else {
                std::vector<std::string> lines;
                std::string line;
                const char *Line;

                while (std::getline(vals, line)) {
                    //set line values to lowercase
                    if (line.rfind("file:",0)!=0) {
                        std::transform(line.begin(), line .end(), line.begin(),
                        [](unsigned char c){ return std::tolower(c); });
                    }

                    //separate flag from data on each line
                    std::vector<std::string> flag = util::split(line, ':');

                    //sets image file and texture that the image draws from
                    if (parseType == 0) {
                        if (flag[0] == "file") {
                            bool existing = false;
                            for (int i = 0; i < textures.size(); i++) {
                                if (flag[1] == textures[i].fileID) {
                                    existing = true;
                                    wrapperIndex = i;
                                }
                            }
                            if (!existing) {
                                sf::Texture textureA;
                                textureA.loadFromFile(directory + "__pic/" + flag[1]);
                                textures.push_back(TextureWrapper{flag[1], textureA});
                                wrapperIndex = textures.size() - 1;
                            }
                        }

                        if (flag[0] == "active") {
                            active = stoi(flag[1]);
                        }
                        if (flag[0] == "weight") {
                            weight = stoi(flag[1]);
                        }

                        // sets frameNum
                        if (flag[0] == "framenum") {
                            frameNum = std::stoi(flag[1]);
                        }

                        if (flag[0] == "momentumcarry") {
                            momentumCarry = std::stod(flag[1]);
                        }

                        //sets frameOrder
                        else if (flag[0] == "frameorder") {
                            std::vector<std::string> data = util::split(flag[1], ' ');
                            for (int i = 0; i < data.size(); i++) {
                                std::vector<std::string> data2 = util::split(data[i], 'x');
                                if (data2.size() > 1) {
                                    int numFrames = std::stoi(data2[0]);
                                    for (int j = 0; j < numFrames; j++) {
                                        frameorder.push_back(std::stoi(data2[1]));
                                        //std::cout << std::stoi(data2[1]) << std::endl;
                                        movements.push_back({0, 0});
                                        gravity.push_back(0.0);
                                        projectileFrames.push_back({});
                                        length++;
                                    }
                                }
                            }
                        }
                        else if (flag[0] == "landanim") {
                            landAnim = stoi(flag[1]);
                        }
                        //sets frameOrder
                        else if (flag[0] == "gravity") {
                            int num = 0;
                            std::vector<std::string> data = util::split(flag[1], ' ');
                            for (int i = 0; i < data.size(); i++) {
                                std::vector<std::string> data2 = util::split(data[i], 'x');
                                if (data2.size() > 1) {
                                    int numFrames = std::stoi(data2[0]);
                                    for (int j = 0; j < numFrames; j++) {
                                        gravity[num] = std::stod(data2[1]);
                                        //std::cout << std::stoi(data2[1]) << std::endl;
                                        num++;
                                    }
                                }
                            }
                        } else if (flag[0] == "framesize") {
                            std::vector<std::string> data = util::split(flag[1], 'x');
                            width = std::stoi(data[0]);
                            height = std::stoi(data[1]);
                        } else if (flag[0] == "id") {
                            ID = std::stoi(flag[1]);
                            animType = static_cast<animation::animType>(ID / 100);
                        } else if (flag[0] == "frameanchor") {
                            std::vector<std::string> data = util::split(flag[1], 'x');
                            anchor = std::stoi(data[0]);
                        } else if (flag[0] == "movement") {
                            int num = 0;
                            std::vector<std::string> data = util::split(flag[1], ' ');
                            for (int i = 0; i < data.size(); i++) {
                                std::vector<std::string> data2 = util::split(data[i], 'x');
                                int numFrames = std::stoi(data2[0]);
                                if (data2.size() > 1) {
                                    std::vector<std::string> data3 = util::split(data2[1], ',');
                                    //std::cout<<numFrames<<std::endl;
                                    if (data3.size() > 1) {
                                        for (int k = 0; k < numFrames; k++) {
                                            movements[num][0] = stod(data3[0]);
                                            movements[num][1] = stod(data3[1]);
                                            //std::cout<<"added movement: "<< movements[num][0]<<" "<<num<<std::endl;
                                            num++;
                                        }
                                    }
                                }
                            }
                        }
                        if (flag[0] == "hitboxorder") {
                            // Example: "5x0,1 3x2 -1x-1"
                            std::vector<std::string> tokens = util::split(flag[1], ' ');

                            for (auto &t: tokens) {
                                if (t.empty()) continue;

                                auto parts = util::split(t, 'x');
                                if (parts.size() != 2) continue;

                                int duration = std::stoi(parts[0]);
                                std::string boxList = parts[1];

                                // Sentinel: -1x-1 means "no hitboxes for all frames"
                                if (duration == -1 && boxList == "-1") {
                                    hitBoxOrder.resize(frameorder.size());
                                    for (auto &v: hitBoxOrder) v.clear();
                                    continue;
                                }

                                // Parse comma-separated box indices
                                std::vector<int> activeSets;
                                auto indices = util::split(boxList, ',');
                                for (auto &idx: indices) {
                                    if (!idx.empty())
                                        activeSets.push_back(std::stoi(idx));
                                    if (std::stoi(idx) == -1) {
                                        activeSets = {};
                                        break;
                                    }
                                }

                                // Expand RLE
                                for (int i = 0; i < duration; i++) {
                                    hitBoxOrder.push_back(activeSets);
                                }
                            }

                            // Clamp to total frames
                        }

                        if (flag[0] == "hurtboxorder") {
                            // Example: "30x0,2"
                            std::vector<std::string> tokens = util::split(flag[1], ' ');

                            for (auto &t: tokens) {
                                if (t.empty()) continue;

                                auto parts = util::split(t, 'x');
                                if (parts.size() != 2) continue;

                                int duration = std::stoi(parts[0]);
                                std::string boxList = parts[1];

                                // Sentinel: -1x-1 means "no hurtboxes"
                                if (duration == -1 && boxList == "-1") {
                                    hurtBoxOrder.resize(frameorder.size());
                                    for (auto &v: hurtBoxOrder) v.clear();
                                    continue;
                                }

                                // Parse comma-separated box indices
                                std::vector<int> activeSets;
                                auto indices = util::split(boxList, ',');
                                for (auto &idx: indices) {
                                    if (!idx.empty())
                                        activeSets.push_back(std::stoi(idx));
                                }

                                // Expand RLE
                                for (int i = 0; i < duration; i++) {
                                    hurtBoxOrder.push_back(activeSets);
                                }
                            }

                            // Clamp to total frames
                        }

                        if (flag[0] == "pushboxorder") {
                            // Example: "30x0,2"
                            std::vector<std::string> tokens = util::split(flag[1], ' ');

                            for (auto &t: tokens) {
                                if (t.empty()) continue;

                                auto parts = util::split(t, 'x');
                                if (parts.size() != 2) continue;

                                int duration = std::stoi(parts[0]);
                                std::string boxList = parts[1];

                                // Sentinel: -1x-1 means "no hurtboxes"
                                if (duration == -1 && boxList == "-1") {
                                    hurtBoxOrder.resize(frameorder.size());
                                    for (auto &v: hurtBoxOrder) v.clear();
                                    continue;
                                }

                                // Expand RLE
                                for (int i = 0; i < duration; i++) {
                                    pushBoxOrder.push_back(stoi(boxList));
                                }

                            }

                            // Clamp to total frames
                            pushBoxOrder.resize(frameorder.size(),-1);
                        }

                        if (flag[0] == "offset") {
                            auto parts = util::split(flag[1], 'x');
                            if (parts.size() >= 2) {
                                xOffset = std::stoi(parts[0]);
                                yOffset = std::stoi(parts[1]);
                            }
                        }

                        if (flag[0] == "projectiles") {
                            std::vector<std::string> tokens = util::split(flag[1], ' ');
                            projectileFiles = tokens;
                        }

                        if (flag[0] == "projectilepos") {
                            std::vector<std::string> tokens = util::split(flag[1], ' ');
                            float x = 0;
                            float y = 0;
                            int id = 0;
                            for (auto &t: tokens) {
                                auto parts = util::split(t, ',');
                                if (parts.size() >= 1) id = std::stoi(parts[0]);
                                if (parts.size() >= 2) x = std::stof(parts[1]);
                                if (parts.size() >= 3) y = std::stof(parts[2]);
                                spawns.emplace_back(id,x,y);
                            }
                        }

                        if (flag[0] == "spawnframes") {
                            std::vector<std::string> tokens = util::split(flag[1], ' ');
                            int frame = 0;
                            int id = 0;
                            for (auto &t: tokens) {
                                auto parts = util::split(t, ',');
                                if (parts.size() >= 1) frame = std::stoi(parts[0]);
                                if (parts.size() >= 2) id = std::stof(parts[1]);
                                projectileFrames[frame].push_back(id);
                            }
                        }

                        if (flag[0] == "hitbox_start") {
                            parseType = 1;
                            currentID++;
                            localBounds.clear();

                            // reset hitbox data to defaults
                            damage = 0;
                            knockBackX = knockBackY = 0;
                            knockBackXA = knockBackYA = 0;
                            knockBackTime = 0;
                            hitStun = blockStun = 0;
                            hHeight = dType = bType = 0;
                            knockDown = grab = 0;
                            hWeight = 0;
                            triggerID = 0;
                            triggerType = 0;
                        }

                        if (flag[0] == "hurtbox_start") {
                            parseType = 2;
                            currentID++;
                            localBounds.clear();
                        }

                        if (flag[0] == "pushbox_start") {
                            parseType = 3;
                            currentID = 0;
                            localBounds.clear();
                        }


                    } else if (parseType == 1) {
                        if (flag[0] == "hitbox_end") {
                            parseType = 0;
                            continue;
                        }
                        if (flag[0] == "pos") {
                            // pos:-13x45,26x45
                            localBounds.clear();
                            auto parts = util::split(flag[1], ',');
                            for (int i = 0; i < parts.size() - 1; i += 2) {
                                auto xy = util::split(parts[i], 'x');
                                auto wh = util::split(parts[i + 1], 'x');
                                if (xy.size() == 2 && wh.size() == 2) {
                                    int x = std::stoi(xy[0]);
                                    int y = std::stoi(xy[1]);
                                    int width = std::stoi(wh[0]);
                                    int height = std::stoi(wh[1]);
                                    localBounds.emplace_back(std::array<int, 4>{x, y, width, height});
                                }
                            }
                        }
                        if (flag[0] == "damage") {
                            damage = std::stoi(flag[1]);
                        }

                        if (flag[0] == "knockback") {
                            // knockback:2x3,1x2,5
                            // (kbX x kbY), (kbXA x kbYA), time
                            auto parts = util::split(flag[1], ',');
                            if (parts.size() >= 1) {
                                auto a = util::split(parts[0], 'x');
                                if (a.size() == 2) {
                                    knockBackX = std::stoi(a[0]);
                                    knockBackY = std::stoi(a[1]);
                                }
                            }
                            if (parts.size() >= 2) {
                                auto b = util::split(parts[1], 'x');
                                if (b.size() == 2) {
                                    knockBackXA = std::stoi(b[0]);
                                    knockBackYA = std::stoi(b[1]);
                                }
                            }
                            if (parts.size() >= 3) {
                                knockBackTime = std::stoi(parts[2]);
                            }
                        }

                        if (flag[0] == "stun") {
                            // stun:4,3  -> hitStun, blockStun
                            auto parts = util::split(flag[1], ',');
                            if (parts.size() >= 1) hitStun = std::stoi(parts[0]);
                            if (parts.size() >= 2) blockStun = std::stoi(parts[1]);
                        }

                        if (flag[0] == "type") {
                            // type:height,weight,boxType,damageType,knockdown,grab
                            auto parts = util::split(flag[1], ',');
                            if (parts.size() >= 1) hHeight = std::stoi(parts[0]);
                            if (parts.size() >= 2) hWeight= std::stoi(parts[1]);
                            if (parts.size() >= 3) bType = std::stoi(parts[2]);
                            if (parts.size() >= 4) dType = std::stoi(parts[3]);
                            if (parts.size() >= 5) knockDown = std::stoi(parts[4]);
                            if (parts.size() >= 6) grab = std::stoi(parts[5]);
                        }

                        if (flag[0] == "boxid") {
                            currentID = stoi(flag[1]);
                        }

                        if (flag[0] == "trigger") {
                            auto parts = util::split(flag[1], ',');
                            if (parts.size() >= 1) triggerType = std::stoi(parts[0]);
                            if (parts.size() >= 2) triggerID = std::stoi(parts[1]);
                        }

                        if (flag[0].rfind("save", 0) == 0) {
                            // build dat array for new HitBox ctor
                            std::array<int, 17> dat{};
                            dat[0] = currentID;
                            dat[1] = damage;
                            dat[2] = knockBackX;
                            dat[3] = knockBackY;
                            dat[4] = knockBackXA;
                            dat[5] = knockBackYA;
                            dat[6] = knockBackTime;
                            dat[7] = hitStun;
                            dat[8] = blockStun;
                            dat[9] = hHeight;
                            dat[10] = dType;
                            dat[11] = bType;
                            dat[12] = knockDown;
                            dat[13] = grab;
                            dat[14] = hWeight;
                            dat[15] = triggerType;
                            dat[16] = triggerID;

                            collision::HitBox hb(localBounds, dat);
                            hitBoxes.push_back(hb);
                            localBounds.clear();
                        }


                    } else if (parseType == 2) {
                        if (flag[0] == "hurtbox_end") {
                            parseType = 0;
                            continue;
                        }

                        if (flag[0] == "pos") {
                            // pos:-13x45,26x45
                            localBounds.clear();
                            auto parts = util::split(flag[1], ',');
                            for (int i = 0; i < parts.size() - 1; i += 2) {
                                auto xy = util::split(parts[i], 'x');
                                auto wh = util::split(parts[i + 1], 'x');
                                if (xy.size() == 2 && wh.size() == 2) {
                                    int x = std::stoi(xy[0]);
                                    int y = std::stoi(xy[1]);
                                    int width = std::stoi(wh[0]);
                                    int height = std::stoi(wh[1]);
                                    localBounds.emplace_back(std::array<int, 4>{x, y, width, height});
                                }
                            }
                        }

                        if (flag[0] == "type") {
                            // type:hurt  (you can map this to different hurtboxType later)
                        }

                        if (flag[0] == "boxid") {
                            currentID = stoi(flag[1]);
                        }

                        if (flag[0].rfind("save", 0) == 0) {
                            collision::HurtBox hb(localBounds, collision::hurtboxType::HURTBOX);
                            hurtBoxes.push_back(hb);
                            localBounds.clear();
                        }
                    } else if (parseType == 3) {
                        if (flag[0] == "hurtbox_end") {
                            parseType = 0;
                            continue;
                        }

                        if (flag[0] == "pos") {
                            // pos:-13x45,26x45
                            localBounds.clear();
                            auto parts = util::split(flag[1], ',');
                            for (int i = 0; i < parts.size() - 1; i += 2) {
                                auto xy = util::split(parts[i], 'x');
                                auto wh = util::split(parts[i + 1], 'x');
                                if (xy.size() == 2 && wh.size() == 2) {
                                    int x = std::stoi(xy[0]);
                                    int y = std::stoi(xy[1]);
                                    int width = std::stoi(wh[0]);
                                    int height = std::stoi(wh[1]);
                                    localBounds.emplace_back(std::array<int, 4>{x, y, width, height});
                                }
                            }
                        }
                        if (flag[0] == "boxid") {
                            currentID = stoi(flag[1]);
                        }
                        if (flag[0].rfind("save", 0) == 0) {
                            collision::CollisionBox hb(localBounds);
                            pushBoxes.push_back(hb);
                            localBounds.clear();
                        }
                    }
                }
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

        ~Animation() {
        }

        [[nodiscard]] const sf::Texture *getTexture() const {
            return &textures.at(wrapperIndex).texture;
        }

        [[nodiscard]] sf::IntRect getFrame(int frame) const {
            return {xOffset, yOffset + height * frameorder[frame], width, height};
        }

        bool operator<(const Animation &other) const {
            return ID < other.ID;
        }

        bool operator>(const Animation &other) const {
            return ID > other.ID;
        }

        bool operator==(const Animation &other) const {
            return ID == other.ID;
        }

        [[nodiscard]] int getAnchor() const {
            return anchor;
        }

        [[nodiscard]] std::vector<std::string> getProjectileNames() const {
            return projectileFiles;
        }

        [[nodiscard]] int getWidth() const {
            return width;
        }

        [[nodiscard]] int getHeight() {
            return height;
        }

        [[nodiscard]] bool getActive() const {
            return active;
        }

        [[nodiscard]] enum animType getAnimType() const {
            return animType;
        }

        [[nodiscard]] double getMomentum() const {
            return momentumCarry;
        }

        [[nodiscard]] double getGravity(int frame) const {
            return gravity[frame];
        }

        [[nodiscard]] int getID() const {
            return ID;
        }

        [[nodiscard]] int getInputID() const {
            return ID / 100;
        }

        [[nodiscard]] int getOrdinality() const {
            return ID % 100 / 10;
        }

        [[nodiscard]] int getVariant() const {
            return ID % 10;
        }

        [[nodiscard]] double getXMove(int frame) const {
            return movements[frame][0];
        }

        [[nodiscard]] double getYMove(int frame) const {
            return movements[frame][1];
        }

        [[nodiscard]] double getXOffset(int frame) const {
        }

        void markProjectiles() {
            for (auto& Hb : hitBoxes) {
                Hb.setProjectile(true);
            }
        }

        void unmarkProjectiles() {
            for (auto& Hb : hitBoxes) {
                Hb.setProjectile(false);
            }
        }


        int getLength() {
            return frameorder.size();
        }

        void updateBoxes(int x, int y, util::direction facing, int frame) {
            //std::cout<<"update boxes: "<<frame<<std::endl;
            activeHitBoxes = {nullptr, nullptr, nullptr};
            if (frame >= length) {
                return;
            }
            try {
                //std::cout<<"hitBoxes:"<<std::endl;
                for (int i = 0; i < activeHitBoxes.size(); i++) {
                    //std::cout<<i<<std::endl;
                    if (i < hitBoxOrder.at(frame).size() && hitBoxOrder.at(frame).at(i) < hitBoxes.size()) {
                        //std::cout<<" "<<hitBoxes.size()<<" ";
                        activeHitBoxes[i] = &hitBoxes.at(hitBoxOrder.at(frame).at(i));
                        //std::cout<<"hitbox "<<i<<std::endl;
                        activeHitBoxes.at(i)->placeGlobalBounds(x, y, facing);
                    } else {
                        //std::cout<<"missing:"<<std::endl;
                    }
                }
            } catch (std::out_of_range &e) {
                activeHitBoxes = {nullptr, nullptr, nullptr};
            }
            //std::cout<<": done1"<<std::endl;
            /*activeHurtBoxes = {nullptr,nullptr,nullptr};
            try {
                //std::cout<<"hurtBoxes:"<<std::endl;
                for (int i = 0; i< activeHurtBoxes.size(); i++) {
                    if (i<hurtBoxOrder.at(frame).size()&& hurtBoxOrder.at(frame).at(i)<hurtBoxes.size()) {
                        activeHurtBoxes.at(i) = &hurtBoxes.at(hurtBoxOrder.at(frame).at(i));
                        activeHurtBoxes.at(i)->placeGlobalBounds(x,y,facing);
                        //std::cout<<"eh";
                    }
                    else {
                        //std::cout<<"eb";
                    }
                }
            } catch (std::out_of_range& e){
                //std::cout<<"el";
                activeHurtBoxes = {nullptr,nullptr,nullptr};
                //std::cout<<"ec";

            }
            //std::cout<<": done2"<<std::endl;*/
            activeHurtBoxes = {nullptr, nullptr, nullptr};
            try {
                //std::cout<<"hurtBoxes:"<<std::endl;
                for (int i = 0; i < activeHurtBoxes.size(); i++) {
                    //std::cout<<i<<std::endl;
                    //std::cout<<"numBoxes: "<<hurtBoxes.size()<<std::endl;
                    if (i < hurtBoxOrder.at(frame).size() && hurtBoxOrder.at(frame).at(i) < hurtBoxes.size()) {
                        activeHurtBoxes[i] = &hurtBoxes.at(hurtBoxOrder.at(frame).at(i));
                        //std::cout<<"hurtbox "<<i<<std::endl;
                        activeHurtBoxes.at(i)->placeGlobalBounds(x, y, facing);

                    } else {
                        //std::cout<<"missing:"<<std::endl;
                    }
                }
            } catch (std::out_of_range &e) {
                activeHurtBoxes = {nullptr, nullptr, nullptr};
            }

            /*activePushBox = nullptr;
            try {
                if (pushBoxOrder.at(frame)<pushBoxes.size() && pushBoxes.size()>0) {
                    activePushBox = &pushBoxes.at(pushBoxOrder.at(frame));
                }
            }catch (std::out_of_range &e) {
                activePushBox = nullptr;
            }*/
        }

        [[nodiscard]] std::vector<spawnInfo> getSpawns(int frame) const {
            std::vector<spawnInfo> spawnOnFrame;
            for (int s: projectileFrames[frame]) {
                spawnOnFrame.push_back(spawns[s]);
            }
            return spawnOnFrame;
        }

        [[nodiscard]] std::vector<std::string> getProjectileTypes(int frame) const {
            return projectileFiles;
        }

        [[nodiscard]] std::array<collision::HitBox *, 3> *getActiveHitBoxes() {
            //std::cout<<"get hit boxes"<<std::endl;
            return &activeHitBoxes;
        }

        [[nodiscard]] std::array<collision::HurtBox *, 3> *getActiveHurtBoxes() {
            //std::cout<<"get hurt boxes"<<std::endl;
            return &activeHurtBoxes;
        }

        [[nodiscard]] collision::CollisionBox * getActivePushBox() {
            return activePushBox;
        }

        [[nodiscard]] std::vector<int> getCancels(int frame) const {
            return cancels[frame];
        }

        [[nodiscard]] int getLandAnim() const {
            return landAnim;
        }

        [[nodiscard]] int getWeight() const {
            return weight;
        }

        void reactivate() {
            for (auto& hb:hitBoxes) {
                hb.setActive(true);
            }
        }

        void doHit(collision::HitBox* box) {
            int id = box->getID();
            for (auto& hb:hitBoxes) {
                //std::cout<<"check inactive"<<std::endl;
                //std::cout<<id<<" "<<hb.getID()<<std::endl;
                if (hb.getID() == id) {
                    hb.setActive(false);
                    //std::cout<<"set inactive"<<std::endl;
                }
            }

        }


        std::vector<int> getHbIdxOnFrame(int frame) {
            return hitBoxOrder[frame];
        }

        std::vector<collision::HitBox> getHitBoxes() {
            return hitBoxes;
        }
    };

    class moveBuffer {
    private:
    protected:
    public:
    };

    enum spawnBehavior {
        Sb_none = -1,
        Sb_randCircle,//random magnitude and direction normalized (Vmax, Vmin, EMPTY)
        Sb_randCone,//random magnitude and direction normalized within a cone (Vmax
        Sb_static,//fixed magnitude and direction
    };

    enum moveBehavior {
        Mb_none = -1,
        Mb_straight,
        Mb_accelerateLine,
        Mb_acceleratePlateau,
        Mb_decelerate,
        Mb_flutter_down,
        Mb_flutter_up,
        Mb_arc,
    };

    enum scaleBehavior {
        Lb_none = -1,
        Lb_const,
        Lb_linear,//(start, slope)
        Lb_inverse,
        Lb_exp,
        Lb_bump,
    };

    enum despawnBehavior {
        Tb_blip = -1,//all particles disappear immediately
        Tb_linear,//straight line down after start time
        Tb_decay,//exponential decay after start time
        Tb_cliff,//starts to drop slowly, then drops off a cliff
    };

    std::array<int,4> getTexturePosFromID(int ID) {
        if (ID <0 || ID>39) return {0,0,0,0};
        return {ID%5 * 8, ID/5 * 8, 8, 8};
    }


    const int numDirectionalBuckets = 32;
    const int numVelocityBuckets = 16;
    const int numFrameBuckets = 256;
    const int numScaleBuckets = 16;
    const int numLifespanBuckets = 32;
    const int numTextureBuckets = 16;


    class particleType {
    private:
        int maxLifeSpan;


        spawnBehavior SB = Sb_none;
        moveBehavior MB = Mb_none;
        scaleBehavior LB = Lb_none;
        despawnBehavior TB = Tb_blip;

        std::array<float,4> sParam = {0.f,0.f,0.f,0.f};//spawn parameters, different meanings depending on spawn behavior
        std::array<float,4> mParam = {0.f,0.f,0.f,0.f};//movement parameters, different meanings depending on movement behavior
        std::array<float,4> lParam = {0.f,0.f,0.f,0.f};//scale parameters, different meaning depending on scale behavior
        std::array<float,4> tParam = {0.f,0.f,0.f,0.f};//lifespan parameters, different depending upon despawn behavior
        std::array<int,numTextureBuckets> xParam = {};//texture parameters, represents the ids of the particle texture sections

    public:
        float velocityBuckets[2][numFrameBuckets][numDirectionalBuckets][numVelocityBuckets];

        float scaleBuckets[numFrameBuckets][numScaleBuckets];

        int lifespanBuckets[numLifespanBuckets];

        int textureBuckets[numTextureBuckets][4];

        particleType(
            spawnBehavior sb, std::array<float,4> sParam,
            moveBehavior mb, std::array<float,4> mParam,
            scaleBehavior lb, std::array<float,4> lParam,
            despawnBehavior tb, std::array<float,4> tParam,
            std::array<int,16> xParam ) {

            SB = sb;
            MB = mb;
            LB = lb;
            TB = tb;

            this->sParam = sParam;
            this->mParam = mParam;
            this->lParam = lParam;
            this->tParam = tParam;
            this->xParam = xParam;

            makeBuckets();


        }

        [[nodiscard]] float getVx(int bucketD, int bucketV, int frame) const {
            return velocityBuckets[0][frame][bucketD][bucketV];
        }
        [[nodiscard]] float getVy(int bucketD, int bucketV, int frame) const {
            return velocityBuckets[1][frame][bucketD][bucketV];
        }
        [[nodiscard]] bool checkDespawn(int bucketT, int frame) const {
            return lifespanBuckets[bucketT]<frame;
        }
        [[nodiscard]] float getScale(int bucketS, int frame) const {
            return scaleBuckets[frame][bucketS];
        }

        void makeBuckets() {
            float interval1;
            float interval2;
            float prevval;
            int idx;
            switch (SB) {
                case Sb_static:
                    for (int i = 0; i < numDirectionalBuckets; i++) {
                        for (int j = 0; j < numVelocityBuckets; j++) {
                            velocityBuckets[0][0][i][j] = sParam[0];
                            velocityBuckets[1][0][i][j] = sParam[1];
                        }
                    }
                    break;
                case Sb_randCircle:
                    //[0] Vmax, [1] Vmin
                    for (int i = 0; i < numDirectionalBuckets; i++) {
                        float dir = i * 2 * std::numbers::pi / numDirectionalBuckets;
                        for (int j = 0; j < numVelocityBuckets; j++) {
                            float mag = sParam[1] + (sParam[0] - sParam[1]) / (numVelocityBuckets - 1) * j;
                            velocityBuckets[0][0][i][j] = mag * std::cos(dir);
                            velocityBuckets[1][0][i][j] = mag * std::sin(dir);
                        }
                    }
                    break;
                case Sb_randCone:
                    //[0] Vmax, [1] Vmin, [2] Tmax, [3] Tmin
                    for (int i = 0; i < numDirectionalBuckets; i++) {
                        float dir = (sParam[3] + (sParam[2] - sParam[3]) / (numDirectionalBuckets - 1) * i)/180 * std::numbers::pi;
                        for (int j = 0; j < numVelocityBuckets; j++) {
                            float mag = sParam[1] + (sParam[0] - sParam[1]) / (numVelocityBuckets - 1) * j;
                            velocityBuckets[0][0][i][j] = mag * std::cos(dir);
                            velocityBuckets[1][0][i][j] = mag * std::sin(dir);
                        }
                    }
                    break;
                default:
                    break;
            }

            switch (MB) {
                case Mb_straight:
                    for (int h = 0; h<2;h++) {
                        for (int i = 1; i < numFrameBuckets; i++) {
                            for (int j = 0; j < numDirectionalBuckets; j++) {
                                for (int k = 0; k < numVelocityBuckets; k++) {
                                    velocityBuckets[h][i][j][k] = velocityBuckets[h][0][j][k];
                                }
                            }
                        }
                    }
                    break;
                case Mb_accelerateLine:
                    for (int h = 0; h<2;h++) {
                        for (int i = 1; i < numFrameBuckets; i++) {
                            for (int j = 0; j < numDirectionalBuckets; j++) {
                                for (int k = 0; k < numVelocityBuckets; k++) {
                                    velocityBuckets[h][i][j][k] = velocityBuckets[h][i-1][j][k] + mParam[0];
                                }
                            }
                        }
                    }
                    break;
                case Mb_acceleratePlateau:
                    //[0] a, [1] b, in the equation dv/dt = b(a-v)^2/a
                    for (int i = 1; i < numFrameBuckets; i++) {
                        for (int j = 0; j < numDirectionalBuckets; j++) {
                            for (int k = 0; k < numVelocityBuckets; k++) {
                                auto vTotal = sqrt(std::pow(velocityBuckets[0][i-1][j][k],2.0)+std::pow(velocityBuckets[1][i-1][j][k],2.0));
                                auto vNew = mParam[1] * (mParam[0]-vTotal)/mParam[0];
                                auto theta = std::atan2(velocityBuckets[1][i-1][j][k],velocityBuckets[0][i-1][j][k]);
                                velocityBuckets[0][i][j][k] = vNew * cos(theta);
                                velocityBuckets[1][i][j][k] = vNew * sin(theta);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }

            switch (LB) {
                case Lb_linear:

                    break;
                default:
                    for (int i = 0; i<numScaleBuckets; i++ ) {
                        for (int j = 0; j< numFrameBuckets; j++) {
                            scaleBuckets[i][j] = lParam[0];
                        }
                    }
                    break;

            }

            switch (TB) {
                case Tb_blip:
                    for (int i = 0; i<numLifespanBuckets; i++ ) {
                        lifespanBuckets[i] = tParam[0];
                    }
                    break;
                default:
                    for (int i = 0; i<numLifespanBuckets; i++ ) {
                        lifespanBuckets[i] = tParam[0];
                    }
                    break;
            }

            for (int i = 0; i< numDirectionalBuckets; i++) {
                for (int j = 0; j < numVelocityBuckets; j++) {}
            }
        }
    };

    enum particleStatus {
        P_open = -1,
        P_spawning = 0,
        P_spawned = 1,
        P_despawning = 2,
        P_state_count = 3,
    };

    const int MAX_PARTICLES = 1024;

    class ParticleHandler {
    private:

        std::array<int, 67> largeBucketRand;
        std::array<int, 61> smallBucketRand;
        int smallRandIdx = 0;
        int largeRandIdx = 0;

        std::vector<particleType> particleTypes;

        std::array<int, MAX_PARTICLES> typeNums;//indexes of the particle types spawned

        std::array<particleStatus, MAX_PARTICLES> statuses;//current status of each particle; if open, no particle exists

        std::array<float, MAX_PARTICLES> VxVals;//x-velocity values of each particle
        std::array<float, MAX_PARTICLES> VyVals;//y-velocity values of each particle

        std::array<float, MAX_PARTICLES> xVals;//x values of each particle
        std::array<float, MAX_PARTICLES> yVals;//y values of each particle

        std::array<float, MAX_PARTICLES> scaleVals;//scale values of each particle

        std::array<int, MAX_PARTICLES> fVals;//frame counters of each particle

        std::array<int, MAX_PARTICLES> dBuckets;//directional buckets per-projectile
        std::array<int, MAX_PARTICLES> vBuckets;//velocity buckets per-projectile
        std::array<int, MAX_PARTICLES> lBuckets;//scale buckets per-projectile
        std::array<int, MAX_PARTICLES> tBuckets;//despawn buckets per-projectile
        std::array<int, MAX_PARTICLES> xBuckets;//texture buckets per-projectile
    public:

        ParticleHandler() {
            std::mt19937 rng(std::random_device{}());

            std::uniform_int_distribution<int> smallDist(0,15);
            std::uniform_int_distribution<int> largeDist(0,31);
            //initializes default values from an array
            for (auto& stat: statuses) {
                stat = P_open;
            }

            for (int& rand:smallBucketRand) {
                rand = smallDist(rng);
            }

            for (int& rand:largeBucketRand) {
                rand = largeDist(rng);
            }
        }

        boolean spawnParticles(int PT, int num, float x, float y) {
            int totalNum = 0;
            for (int i = 0; i<MAX_PARTICLES; i++) {
                if (statuses[i] == P_open) {
                    fVals[i] = 0;
                    xVals[i] = x;
                    yVals[i] = y;

                    auto dBuck = largeBucketRand[largeRandIdx];
                    if (++largeRandIdx == 67) largeRandIdx = 0;


                    auto tBuck = largeBucketRand[largeRandIdx];
                    if (++largeRandIdx == 67) largeRandIdx = 0;


                    auto vBuck = smallBucketRand[smallRandIdx];
                    if (++smallRandIdx == 61) smallRandIdx = 0;


                    auto lBuck = smallBucketRand[smallRandIdx];
                    if (++smallRandIdx == 61) smallRandIdx = 0;

                    dBuckets[i] = dBuck;
                    vBuckets[i] = vBuck;
                    lBuckets[i] = lBuck;
                    tBuckets[i] = tBuck;

                    VxVals[i] = particleTypes[PT].velocityBuckets[0][0][dBuck][vBuck];
                    VyVals[i] = particleTypes[PT].velocityBuckets[1][0][dBuck][vBuck];

                    typeNums[i] = PT;
                    statuses[i] = P_spawned;

                    totalNum++;
                    if (totalNum >= num) return true;
                }
            }
            return false;
        }

        void update() {
            //iterates through the arrays one by one to update values simultaneously
            for (int i = 0; i<MAX_PARTICLES; i++) {
                if (statuses[i] == P_open) {
                    continue;
                }

                auto& type = particleTypes[typeNums[i]];

                int f = ++fVals[i];

                int t = tBuckets[i];
                if (f >= type.lifespanBuckets[t]) {
                    statuses[i] = P_open;
                    continue;
                }

                int d = dBuckets[i];
                int v = vBuckets[i];
                int s = lBuckets[i];

                float vx = VxVals[i] = type.velocityBuckets[0][f][d][v];
                float vy = VyVals[i] = type.velocityBuckets[1][f][d][v];

                scaleVals[i] = type.scaleBuckets[f][s];

                xVals[i] += vx;
                yVals[i] += vy;
            }
        }

        void addParticle(particleType P) {
            particleTypes.push_back(P);
        }
    };


}
#endif
