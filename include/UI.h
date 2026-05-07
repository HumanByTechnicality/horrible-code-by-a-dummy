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

    enum class imgBehavior {
        image_static = -1,
        image_hovered,
        image_with_state,
        image_animate,
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
        int imgState;

        bool selected = false;//whether the slider is currently selected
        bool writesInState = false;

        int numStates = 0;//total number of unique states
        int state = 0;//current state (0 to numStates-1)
        int lastState = 0;

        int* readVal = nullptr;//value read from to update display
        int* writeVal = nullptr;//value modified by the button when pressed

        pressFunction fP = pressFunction::NONE;
        releaseFunction fR = releaseFunction::NONE;
        imgBehavior iB = imgBehavior::image_hovered;
        indicatorType type = indicatorType::on_off;

        std::vector<int> stateMap;
    protected:
    public:
        Button(sf::IntRect size, sf::IntRect img, int states, std::vector<int> map, int imgBehavior, int* write =nullptr, int* read = nullptr) {
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
            else writeVal = write;

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
                case pressFunction::set:
                    state = 0;
                    writeValue();
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
                /*
                *writeVal = stateMap[state];
                std::cout<<this<<" wrote the value "<<stateMap[state]<<" to the address "<<writeVal<<std::endl;*/
            }
            imgState = getImageFrame();
            lastState = state;
        }

        int getImageFrame() {
            switch (iB) {
                case (imgBehavior::image_hovered):
                    return (int) hovered;
                case (imgBehavior::image_with_state):
                    return stateMap[state];
                default:
                    return 0;
            }

        };

        sf::IntRect getTextureRect() {
            return {0, imgH*imgState,imgW, imgH};
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

        void writeValue() {
            *(writeVal) = stateMap[state];
            std::cout <<"wrote value "<< stateMap[state]<<" to tag at "<<writeVal<<": "<<*writeVal<<std::endl;


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

    class TextField : public sf::Drawable , public sf::Transformable {
        public:
    };

    enum class textAlignment {
        LEFT = -1,
        MIDDLE = 0,
        RIGHT = 0,
    };

    class TextIndicator {
    private:
        std::vector<std::string> text;
        int numStates = 0;
        std::vector<int> stateMap;

        sf::Color textColor = sf::Color::White;
        sf::Color backgroundColor = sf::Color::Black;

        sf::Font *font = nullptr;

        sf::Vector2f textOffset = sf::Vector2f(0, 0);
        sf::Vector2f position = sf::Vector2f(0, 0);
        sf::Vector2f size = {0, 0};

        int fontSize = 7;

        bool wrapping = true;

        int *read;

        int state = 0;

        sf::Text drawable;



    public:
        TextIndicator(std::vector<std::string> text,
                      sf::FloatRect locale,
                      sf::Font *font,
                      int numStates,
                      std::vector<int> stateMap,
                      int *read,
                      int fontSize) {
            this->text = text;
            this->numStates = numStates;
            this->stateMap = stateMap;
            this->read = read;
            position = locale.getPosition();
            size = locale.getSize();
            this->fontSize = fontSize;
            this->font = font;
        }

        void updateDrawable() {
            if (!font) return;
            if (read == nullptr) return;
            state = * read;

            // Update text from state/read
            drawable.setFont(*font);
            drawable.setString(getCurrentText());
            drawable.setCharacterSize(fontSize);
            drawable.setFillColor(textColor);

            // Positioning logic (simple version)
            drawable.setPosition(
                position.x + textOffset.x,
                position.y + textOffset.y
            );
        }

        // -------------------------
        // Getters
        // -------------------------

        const std::vector<std::string> &getText() const { return text; }
        int getNumStates() const { return numStates; }
        const std::vector<int> &getStateMap() const { return stateMap; }

        sf::Color getTextColor() const { return textColor; }
        sf::Color getBackgroundColor() const { return backgroundColor; }

        sf::Font *getFont() const { return font; }

        sf::Vector2f getTextOffset() const { return textOffset; }
        sf::Vector2f getPosition() const { return position; }
        sf::Vector2f getSize() const { return size; }

        int getFontSize() const { return fontSize; }
        bool getWrapping() const { return wrapping; }

        int *getReadPtr() const { return read; }
        int getState() const { return state; }

        // -------------------------
        // Setters
        // -------------------------

        void setText(const std::vector<std::string> &t) { text = t; }
        void setNumStates(int n) { numStates = n; }
        void setStateMap(const std::vector<int> &sm) { stateMap = sm; }

        void setTextColor(const sf::Color &c) { textColor = c; }
        void setBackgroundColor(const sf::Color &c) { backgroundColor = c; }

        void setFont(sf::Font *f) { font = f; }

        void setTextOffset(const sf::Vector2f &off) { textOffset = off; }
        void setPosition(const sf::Vector2f &pos) { position = pos; }
        void setSize(const sf::Vector2f &s) { size = s; }

        void setFontSize(int fs) { fontSize = fs; }
        void setWrapping(bool w) { wrapping = w; }

        void setReadPtr(int *r) { read = r; }
        void setState(int s) { state = s; }

        std::string getCurrentText() const {
            // Check if state is valid
            if (state < 0 || state >= static_cast<int>(stateMap.size()))
                return "";

            int mappedIndex = stateMap[state];

            // Check if mapped index is valid for text vector
            if (mappedIndex < 0 || mappedIndex >= static_cast<int>(text.size()))
                return "";

            return text[mappedIndex];
        }

        const sf::Text& getDrawable() const {
            return drawable;
        }
    };


}


namespace actors {
    class Actor;
    class Fighter;
}
#endif //NEWGAME_UI_H