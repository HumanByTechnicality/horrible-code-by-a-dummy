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



namespace animation {

    struct TextureWrapper {
        std::string fileID;
        sf::Texture texture;
    };

    inline sf::Texture NullTexture;
    inline bool nullTextureLoaded = NullTexture.loadFromFile("../__pic/NULLTEXTURE.png");
    inline std::vector<TextureWrapper> textures = {TextureWrapper{}};

    class Animation {
    protected:
        std::vector<collision::HitBox> hitBoxes;//the set of hitboxes associated with this animation
        std::vector<collision::HurtBox> hurtBoxes;//the set of hurtboxes associated with this animation

        sf::Texture* texture;//the texture associated with this animation

        std::vector<int> frameorder;//the order of frames in this animation, by layer

        std::vector<std::array<double,2>> movements;//da movements
        std::vector<double> gravity;//when and how hard gravity applies to the move

        int width;//the width of one frame
        int height;//the height of one frame
        int anchor;//the x anchor of one frame
        int xOffset;//the offset from x=0 of the top left corner of the first frame
        int yOffset;//the offset from y=0 of the top left corner of the first frame

        bool exist;//whether this animation has been properly initialized
        int ID = 0;
        int wrapperIndex = 0;

        bool active = false;

        animType animType;

        double momentumCarry = 0;
    public:
        std::vector<graphics::layerID> layers;//the layers covered by the animation
        int numLayers;//the number of layers the animation covers
        int frameNum;//the number of unique frames in an animation
        int length;//the length of the animation

        Animation(std::string id) {
            //initialize default values (remove "garbage")
            width = 0;
            height = 0;
            anchor = 0;
            xOffset = 0;
            yOffset = 0;
            momentumCarry = 0;
            exist = false;
            active = false;
            length = 0;
            TextureWrapper texture;

            frameorder.reserve(400);
            hitBoxes.reserve(400);
            hurtBoxes.reserve(400);

            std::string directory = "../";
            std::ifstream vals(directory + "__val/" + id + ".txt");

            if (!vals.is_open()) {
                std::cerr << "Error opening file: " << directory << std::endl;
            }

            else {
                std::vector<std::string> lines;
                std::string line;
                const char*Line;

                while (std::getline(vals, line)) {
                    //set line values to lowercase
                    Line = line.c_str();
                    for (int i = 0; Line[i] != '\0'; i++) {
                        std::tolower(Line[i]);
                    }
                    line = std::string(Line);

                    //separate flag from data on each line
                    std::vector<std::string> flag = util::split(line, ':');

                    //sets image file and texture that the image draws from
                    if (flag[0] == "file") {
                        bool existing = false;
                        for (int i = 0; i< textures.size(); i++) {
                            if (flag[1]==textures[i].fileID) {
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
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            if (data2.size()>1) {
                                int numFrames = std::stoi(data2[0]);
                                for (int j = 0; j< numFrames; j++) {
                                    frameorder.push_back(std::stoi(data2[1]));
                                    //std::cout << std::stoi(data2[1]) << std::endl;
                                    movements.push_back({0,0});
                                    gravity.push_back(0.0);
                                    length++;
                                }
                            }
                        }
                    }

                    //sets frameOrder
                    else if (flag[0] == "gravity") {
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            if (data2.size()>1) {
                                int numFrames = std::stoi(data2[0]);
                                for (int j = 0; j< numFrames; j++) {
                                    gravity[num] = std::stod(data2[1]);
                                    std::cout << std::stoi(data2[1]) << std::endl;
                                    num++;
                                }
                            }
                        }
                    }

                    else if (flag[0] == "framesize") {
                        std::vector<std::string> data = util::split(flag[1], 'x');
                        width = std::stoi(data[0]);
                        height = std::stoi(data[1]);
                    }
                    else if (flag[0] == "id") {
                        ID = std::stoi(flag[1]);
                        animType =static_cast<animation::animType>(ID/100);
                    }

                    else if (flag[0] == "frameanchor") {
                        std::vector<std::string> data = util::split(flag[1], 'x');
                        anchor = std::stoi(data[0]);
                    }

                    else if (flag[0] == "movement") {
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            int numFrames = std::stoi(data2[0]);
                            if (data2.size()>1) {
                                std::vector<std::string> data3 = util::split(data2[1], ',');
                                //std::cout<<numFrames<<std::endl;
                                if (data3.size()>1) {
                                    for (int k = 0; k< numFrames; k++) {
                                        movements[num][0] = stod(data3[0]);
                                        movements[num][1] = stod(data3[1]);
                                        //std::cout<<"added movement: "<< movements[num][0]<<" "<<num<<std::endl;
                                        num++;
                                    }
                                }
                            }
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

        ~Animation() {}

       [[nodiscard]] const sf::Texture* getTexture() const{
            return &textures.at(wrapperIndex).texture;
        }

        [[nodiscard]] sf::IntRect getFrame(int frame) const{
            return {xOffset, yOffset + height * frameorder[frame], width, height};
        }

        bool operator<(const Animation& other) const {
            return ID < other.ID;
        }

        bool operator>(const Animation& other) const {
            return ID > other.ID;
        }

        bool operator==(const Animation& other) const {
            return ID == other.ID;
        }

        [[nodiscard]] int getAnchor() const{
            return anchor;
        }

        [[nodiscard]] int getWidth() const{
            return width;
        }
        [[nodiscard]] int getHeight() {
            return height;
        }

        [[nodiscard]] bool getActive() const{
            return active;
        }

        [[nodiscard]] enum animType getAnimType() const{
            return animType;
        }

        [[nodiscard]] double getMomentum() const{
            return momentumCarry;
        }

        [[nodiscard]] double getGravity(int frame) const {
            return gravity[frame];
        }

        [[nodiscard]] int getID() const {
            return ID;
        }
        [[nodiscard]] int getInputID() const {
            return ID/100;
        }
        [[nodiscard]] int getOrdinality() const {
            return ID%100/10;
        }
        [[nodiscard]] int getVariant() const {
            return ID%10;
        }

        [[nodiscard]] double getXMove(int frame) const {
            return movements[frame][0];
        }

        [[nodiscard]] double getYMove(int frame) const {
            return movements[frame][1];
        }

    };

    class moveBuffer {
    private:
    protected:
    public:
    };
}
#endif

