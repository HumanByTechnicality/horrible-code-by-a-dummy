//
// Created by wangs on 2/26/2026.
//
#include "ANIMATION.h";
#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#ifndef NEWGAME_UI_H
#define NEWGAME_UI_H
#pragma once
namespace ui {
    enum class pressFunction {
        NONE = -1,
        increment_loop = 0,
        decrement_loop = 1,
        increment_limit = 2,
        decrement_limit = 3,
        toggle = 4,
        drag = 5,
        set = 6

    };

    enum class releaseFunction {
        NONE = -1,
        increment_loop = 0,
        decrement_loop = 1,
        increment_limit = 2,
        decrement_limit = 3,
        toggle = 4,
        set = 5
    };

    enum class indicatorType {
        on_off = 0,
        continuous_RIGHT = 1,
        continuous_UP = 2,
        continuous_DOWN = 3,
        continuous_LEFT = 4,
        discrete_RIGHT = 5,
        discrete_UP = 6,
        discrete_DOWN = 7,
        discrete_LEFT = 8,
        discrete_IMAGE = 9,
        counter = 10
    };

    //any ui element that can be selected TODO:plan Button class
    class Button {
    private:
        int x = 0;//x position of top left corner
        int y = 0;//y position of top left corner
        int width = 0;//total width
        int height = 0;//total height

        int hovered = false;

        int tagVal = 0;

        int imgX;
        int imgY;
        int imgW;
        int imgH;

        bool selected = false;//whether the slider is currently selected

        int numStates = 0;//total number of unique states
        int state = 0;//current state (0 to numStates-1)
        int lastState = 0;

        int* readVal = nullptr;//value read from to update display
        int* writeVal = nullptr;//value modified by the button when pressed

        pressFunction fP = pressFunction::NONE;
        releaseFunction fR = releaseFunction::NONE;
        indicatorType type = indicatorType::on_off;

        std::vector<int> stateMap;
    protected:
    public:
        Button(sf::IntRect size, sf::IntRect img, int states, std::vector<int> map, int* write =nullptr, int* read = nullptr) {
            x = size.left;
            y = size.top;
            width = size.width;
            height = size.height;
            imgX = img.left;
            imgY = img.top;
            imgW = img.width;
            imgH = img.height;

            numStates = states;
            state = 0;
            for (int i = 0; i<states; i++) {
                if (i<map.size()) stateMap.push_back(map[i]);
                else stateMap.push_back(0xDAF7A55);
            }



            if (read == nullptr) readVal = &this->hovered;
            else readVal = read;

            if (write == nullptr) writeVal = &this->tagVal;
            else readVal = read;

        }
        bool checkHover(int xC, int yC) {
            hovered =(xC>x && xC<x+width && yC>y && yC<y+height);
            return hovered;
        }

        void drag (int xC, int yC) {
            switch (type) {
                case indicatorType::discrete_UP:
                case indicatorType::continuous_UP:
                    state = (double)(y+height-yC)/height * numStates;
                    break;
                case indicatorType::discrete_DOWN:
                case indicatorType::continuous_DOWN:
                    state = (double)(yC-y)/height * numStates;
                    break;
                case indicatorType::discrete_LEFT:
                case indicatorType::continuous_LEFT:
                    state = (double)(x+width-xC)/height * numStates;
                    break;
                case indicatorType::discrete_RIGHT:
                case indicatorType::continuous_RIGHT:
                    state = (double)(xC-x)/height * numStates;
                    break;
                default:
                    break;
            }

            if (state < 0 ) {
                state = 0;
            }

            if (state >= numStates) {
                state = numStates-1;
            }
        }

        void press(int xC, int yC) {
            switch (fP) {
                case pressFunction::NONE:
                    break;
                case pressFunction::increment_loop:
                    state ++;
                    if (state >= numStates) {
                        state = 0;
                    }
                    break;
                case pressFunction::increment_limit:
                    state ++;
                    if (state >= numStates) {
                        state = numStates;
                    }
                    break;
                case pressFunction::decrement_loop:
                    state --;
                    if (state < 0) {
                        state = numStates-1;
                    }
                    break;
                case pressFunction::decrement_limit:
                    state --;
                    if (state < 0) {
                        state = 0;
                    }
                    break;
                case pressFunction::toggle:
                    state = !state;
                    break;
                case pressFunction::drag:
                    drag(xC, yC);
                    break;

            }
            selected = true;
        }
        void release(int x, int y) {
            selected = false;
        }

        void reset() {
            state = 0;
        }
        void update(int yC = 0, int xC = 0) {
            if (selected && fP == pressFunction::drag) {
                drag(xC, yC);

            }
            if (lastState != state) {
                *writeVal = stateMap[state];
                std::cout<<this<<" wrote the value "<<stateMap[state]<<" to the address "<<writeVal<<std::endl;
            }
            lastState = state;
        }

        sf::IntRect getTextureRect() {
            return {0, imgH*hovered,imgW, imgH};
        }

        int getRealImgX() {
            return (x + imgX) * graphics::scale;
        }
        int getRealImgY() {
            return (y + imgY) * graphics::scale;
        }

        void setHover(bool hover) {
            this->hovered = hover;
        }

        void setWrite(int* newWrite) {
            writeVal = newWrite;
        }

        void setBehavior(pressFunction pf, releaseFunction rf) {
            fP = pf;
            fR = rf;
        }


    };

    //any ui element that changes according to a state TODO:plan Indicator class
    class Indicator {
    private:
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;

        int numStates = 0;
        int state = 0;

        int* readVal = nullptr;

        indicatorType type = indicatorType::on_off;
    protected:
    public:
        void press(int x, int y) {

        }
        void release(int x, int y) {

        }
        void update() {

        }
    };

}


namespace actors {
    class Actor;
    class Fighter;
}
#endif //NEWGAME_UI_H