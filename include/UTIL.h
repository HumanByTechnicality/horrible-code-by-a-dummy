//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_UTIL_H
#define INCLUDE_UTIL_H
#include "stdexcept"
#include "string"
#include "vector"

//contains all functions necessary for the game but not included by the other namespaces, such as camera movement and game states
namespace util {

    class Camera {
    private:
    protected:
    public:
        double x;
        double y;
        double zoom;
    };

    //function to shift each element of an array over by one, overloaded for arrays of char, int and float.
    inline bool shift(char list[], int size, int increment = 1, bool cycle=false){
        increment = increment %size;
        char list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = 'x';
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<size;i++) {
                if (list2[i] == 'x') {
                    list2[i] = list[((i-increment)+size)%size];
                }
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to 'x' when value is deleted
    inline bool shift(int list[], int size, int increment = 1, bool cycle=false){
        increment = increment %size;
        int list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = -1;
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<startI;i++) {
                list2[i] = list[(i-increment)%size];
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to -1 when value is deleted
    inline bool shift(double list[], int size, int increment = 1, bool cycle=false) {
        increment = increment %size;
        double list2[size];
        int startI = increment * (increment > 0);
        int endI = size -1 + increment + (increment < 0);
        for (int i = 0; i < size; i++) {
            list2[i] = -1;
        }
        for (int i = startI; i<endI;i++) {
            list2[i] = list[i-increment];
        }
        if (cycle) {
            for (int i = 0; i<startI;i++) {
                list2[i] = list[(i-increment)%size];
            }
        }
        for (int i = 0; i<size; i++) {
            list[i] = list2[i];
        }
        return true;
    }//defaults to -1.0 when value is deleted

    //function to split a string at all instances of a delimiter
    inline std::vector<std::string> split(std::string &s, char delimiter = ' ') {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = s.find(delimiter);

        while (end != std::string::npos) {
            tokens.push_back(s.substr(start, end - start));
            start = end + 1;
            end = s.find(delimiter, start);
        }
        tokens.push_back(s.substr(start)); // Add the last token

        return tokens;
    }

    template <typename T>
    class circularBuffer {
    private:
        std::vector<T> buffer;
        int headIdx = 0;
        int tailIdx = 0;
        int capacity = 0;
    public:
        circularBuffer(int size) {
            capacity = size;
            buffer.assign(capacity + 1, T{});
        }
        T read() {
            T b = buffer[tailIdx];
            tailIdx = (tailIdx + 1) %(capacity + 1);
            return b;
        }

        void write(T value) {
            buffer[headIdx] = value;
            headIdx = (headIdx + 1) %(capacity + 1);
        }

        void writeWithOverWrite(T value) {
            buffer[headIdx] = value;
            headIdx = (headIdx + 1) %(capacity + 1);
            if (headIdx == tailIdx) {
                tailIdx = (tailIdx + 1) %(capacity + 1);
            }
        }

        T peek(int idx) {
            int count = (headIdx - tailIdx + (capacity + 1)) % (capacity + 1);

            if (idx < 0 || idx >= count) {
                throw std::out_of_range("Index out of buffer bounds");
            }

            return buffer[(tailIdx + idx) % (capacity + 1)];
        }

        T peekLast() {
            if (isEmpty()) {
                throw std::out_of_range("Index out of buffer bounds");
            }

            return buffer[(headIdx + capacity) % (capacity + 1)];
        }

        std::vector<T> peekFull() {
            std::vector<T> vect;
            int current = tailIdx;

            vect.reserve(capacity);
            for (;current!=headIdx; current = (current + 1) % (capacity + 1)) {
                vect.push_back(buffer[current]);
            }
            return vect;
        }
        std::vector<T> peekFullBack() {
            std::vector<T> vect;
            int current = (headIdx + capacity) % (capacity + 1);

            vect.reserve(capacity);
            for (;current!=tailIdx; current = (current + capacity) % (capacity + 1)) {
                vect.push_back(buffer[current]);
            }
            return vect;
        }

        bool isFull() {
            return (headIdx + 1) % (capacity + 1) == tailIdx;
        }

        bool isEmpty() {
            return  headIdx == tailIdx;
        }

        bool clear() {
            while (!isEmpty()) {
                read();
            }
            return true;
        }


    };

    inline bool endsWith(const std::vector<int>& set, const std::vector<int>& subset) {
        if (subset.size() > set.size()) {
            return false;
        }

        return std::equal(subset.begin(), subset.end(), set.end() - subset.size());
    }

    inline bool endsWithFromIndex(const std::vector<int>& set, const std::vector<int>& subset, int startIndex) {
        if (subset.size() > set.size() - startIndex + 1 || subset.size() - startIndex <= 0) {
            return false;
        }

        //std::cout<<subset[startIndex]<<" "<<set[set.size() - startIndex + 1]<<std::endl;
        return std::equal(subset.begin() + startIndex, subset.end(), set.end() - subset.size() + startIndex);
    }



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


}
#endif
