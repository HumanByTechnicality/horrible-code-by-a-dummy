/**
 *BEWARE-BE-WARNED-BE-WARY-BE-INFORMED:
 *The bugs abound in these lands. Not one place is safe. If you seek sanctuary from their
 *atrocities anywhere here, you shall be sorely disappointed. I have tried my best to
 *stave them off, but alas, they have broken through, and ere the summer sun they shall
 *consume our kingdom in its entirety. I beseech of you, do not attempt to destroy them.
 *They are endless in number and by nature impossible to track down. I bid you well, fair
 *traveler. May the bugs recoil at thine presence.
 *
 *-------- CAVETE, MONEMINI, CAVILLAMINI, CERTIORES FIERI --------
 *INSECTA IN HIS TERRIS ABUNDANT. NULLUS LOCUS TUTUS EST. SI REFUGIUM AB
 *EORUM ATROCITATIBUS USQUAM HIC QUAERIS, GRAVITER FRUSTRABERIS. QUANTUM
 *POTUI, CONATUS SUM EOS ARCERE, SED HEU, PERRUPERUNT ET ANTE SOLEM
 *AESTIVUM REGNUM NOSTRUM TOTUM CONSUMENT. TE OBSECRO, NE EOS DELERE
 *CONERIS. NUMERO INFINITI SUNT ET NATURA DIFFICILES AD INVENIENDUM. BENE
 *TIBI PRECOR, VIATOR PULCHER. INSECTA PRAESENTIAM TUAM ABHORREANT.
 *
 *    ||
 *====||===== CODE DEMONS!
 *    ||        BEGONE!
 *    ||      BEELZEBUG!
 *    ||
 *    ||
 ***/


#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>

#include "fstream"
#include "sstream"

#include "include/COLLISION.h"
#include "include/UTIL.h"
#include <iomanip>

#include "format"

// ------------------------------------------------------------
// Utility
// ------------------------------------------------------------
static float distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

// ------------------------------------------------------------
// Textbox
// ------------------------------------------------------------
class Textbox {
public:
    sf::FloatRect bounds;
    bool selected = false;
    std::string value;

    Textbox(float x, float y, float w, float h)
        : bounds(x, y, w, h) {}

    Textbox(float x, float y, float w, float h, std::string val)
        : bounds(x, y, w, h), value {val}{}

    bool contains(sf::Vector2f p) const {
        return bounds.contains(p);
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        sf::RectangleShape box;
        box.setPosition(bounds.left, bounds.top);
        box.setSize({bounds.width, bounds.height});
        box.setFillColor(selected ? sf::Color(200,200,255) : sf::Color(150,150,150));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2.f);
        window.draw(box);

        sf::Text txt;
        txt.setFont(font);
        txt.setString(value);
        txt.setCharacterSize(16);
        txt.setFillColor(sf::Color::Black);
        txt.setPosition(bounds.left + 4, bounds.top + 2);
        window.draw(txt);
    }
};

// ------------------------------------------------------------
// Camera class
// ------------------------------------------------------------
class Camera {
public:
    sf::Vector2f offset {0.f, 0.f};

    void move(float dx, float dy) {
        offset.x += dx;
        offset.y += dy;
    }

    void setOffset(float x, float y) {
        offset.x = x;
        offset.y = y;
    }

    sf::Vector2f getOffset() const {
        return offset;
    }

    sf::Vector2f apply(const sf::Vector2f& p) const {
        return { p.x - offset.x, p.y - offset.y };
    }

    sf::FloatRect apply(const sf::FloatRect& r) const {
        return { r.left - offset.x, r.top - offset.y, r.width, r.height };
    }

    sf::Vector2f toWorld(const sf::Vector2f& screen) const {
        return { screen.x + offset.x, screen.y + offset.y };
    }
};


// ------------------------------------------------------------
// Draggable Rectangle
// ------------------------------------------------------------
class DraggableRect {
public:
    float left, top, right, bottom;
    int grabbedCorner = -1;
    float grabRadius = 10.f;
    int group = 0;
    bool selected = false;
    bool active = false;

    DraggableRect(float l, float t, float r, float b)
        : left(l), top(t), right(r), bottom(b) {}

    std::vector<sf::Vector2f> corners() const {
        return {
            {left,  top},
            {right, top},
            {right, bottom},
            {left,  bottom}
        };
    }

    bool tryGrab(sf::Vector2f mousePos) {
        auto c = corners();
        for (int i = 0; i < 4; i++) {
            if (distance(mousePos, c[i]) < grabRadius) {
                grabbedCorner = i;
                return true;
            }
        }
        return false;
    }

    void release() {
        grabbedCorner = -1;
    }

    void drag(sf::Vector2f mousePos, float winW, float winH, Camera & cam) {
        if (grabbedCorner == -1) return;

        float mx = mousePos.x;
        float my = mousePos.y;
        mx = (int) (mx /8 + 0.5) * 8;
        my = (int) (my /8 + 0.5) * 8;

        switch (grabbedCorner) {
            case 0: left = mx; top = my; break;
            case 1: right = mx; top = my; break;
            case 2: right = mx; bottom = my; break;
            case 3: left = mx; bottom = my; break;
        }

        if (left > right) std::swap(left, right);
        if (top > bottom) std::swap(top, bottom);

        left   = std::max(cam.offset.x, left);
        top    = std::max(cam.offset.y, top);
        right  = std::min(cam.offset.x+winW, right);
        bottom = std::min(cam.offset.y+winH, bottom);
    }

    bool contains(sf::Vector2f p) const {
        return p.x >= left && p.x <= right &&
               p.y >= top  && p.y <= bottom;
    }

    void draw1(sf::RenderWindow& window, const sf::Font& font, const Camera& cam) {
        float width = right - left;
        float height = bottom - top;

        sf::FloatRect screenRect = cam.apply({ left, top, width, height });

        sf::RectangleShape rect;
        rect.setPosition(screenRect.left, screenRect.top);
        rect.setSize({screenRect.width, screenRect.height});
        if (active) {
            rect.setFillColor(selected ? sf::Color(255,255,0,80) : sf::Color(0,255,0,80));
            rect.setOutlineColor(sf::Color::Green);
        }
        else {
            rect.setFillColor(sf::Color(0,200,0,70));
            rect.setOutlineColor(sf::Color(0,200,0,255));
        }
        rect.setOutlineThickness(1.f);
        window.draw(rect);

        float cx = (left + right) * 0.5f;
        float cy = (top + bottom) * 0.5f;

        sf::Vector2f screenCenter = cam.apply({cx, cy});

        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(group));
        label.setCharacterSize(20);
        if (active) {
            label.setFillColor(sf::Color::Blue);
        }
        else {
            label.setFillColor(sf::Color::Black);
        }

        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
        label.setPosition(screenCenter);

        window.draw(label);
    }
    void draw2(sf::RenderWindow& window, const sf::Font& font, const Camera& cam) {
        if (active) {
            auto c = corners();
            for (int i = 0; i < 4; i++) {
                sf::CircleShape h(grabRadius);
                h.setOrigin(grabRadius, grabRadius);

                sf::Vector2f screenPos = cam.apply(c[i]);
                h.setPosition(screenPos);

                h.setFillColor(i == grabbedCorner ? sf::Color::Yellow : sf::Color::Red);
                window.draw(h);
            }
        }
        //std::cout<<group<<" "<<active<<std::endl;
    }

};

enum rectType {
    hit,
    hurt,
    push,
    RECT_TYPE_COUNT
};

// ------------------------------------------------------------
// Not Draggable Rectangle
// ------------------------------------------------------------
class DisplayRect {
public:
    float left, top, right, bottom;

    sf::Color borderColor;
    sf::Color centerColor;
    sf::Color otherBorderColor;
    sf::Color otherCenterColor;

    int group = -1;

    bool active = false;

    DisplayRect(rectType type, float x, float y, float w, float h, int group) {
        left = x, top = y, right = x+w, bottom = y+h;
        switch (type) {
            case hit:
                borderColor =  {255, 0, 0, 255};
                centerColor  = {255, 0, 0, 60};
                otherBorderColor =  {150, 100, 100, 255};
                otherCenterColor  = {150, 100, 100, 60};
                break;
            case hurt:
                borderColor = {0, 0, 255, 255};
                centerColor  = {0, 0, 255, 60};
                otherBorderColor =  {100, 100, 150, 255};
                otherCenterColor  = {100, 100, 150, 60};
                break;
            case push:
                borderColor = {0, 255, 255, 255};
                centerColor  = {0, 255, 255, 60};
                otherBorderColor =  {100, 150, 150, 255};
                otherCenterColor  = {100, 150, 150, 60};
                break;
        }
    }

    void draw(sf::RenderWindow& window, const Camera& cam) {
        float width = right - left;
        float height = bottom - top;

        sf::FloatRect screenRect = cam.apply({ left, top, width, height });

        sf::RectangleShape rect;
        rect.setPosition(screenRect.left, screenRect.top);
        rect.setSize({screenRect.width, screenRect.height});
        if (active) {
            rect.setFillColor(centerColor);
            rect.setOutlineColor(borderColor);
        }
        else {
            rect.setFillColor(otherCenterColor);
            rect.setOutlineColor(otherBorderColor);
        }
        rect.setOutlineThickness(1.f);
        window.draw(rect);
    }
private:
};

// ------------------------------------------------------------
// Button Class
// ------------------------------------------------------------
class Button {
public:
    sf::FloatRect bounds;
    bool toggled = false;
    std::string label;

    Button(float x, float y, float w, float h, const std::string& text)
        : bounds(x, y, w, h), label(text) {}

    bool contains(sf::Vector2f p) const {
        return bounds.contains(p);
    }

    void click() {
        toggled = !toggled;
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        sf::RectangleShape box;
        box.setPosition(bounds.left, bounds.top);
        box.setSize({bounds.width, bounds.height});
        box.setFillColor(toggled ? sf::Color(120,200,120) : sf::Color(180,180,180));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2.f);
        window.draw(box);

        sf::Text txt;
        txt.setFont(font);
        txt.setString(label);
        txt.setCharacterSize(18);
        txt.setFillColor(sf::Color::Black);

        sf::FloatRect tb = txt.getLocalBounds();
        txt.setOrigin(tb.width * 0.5f, tb.height * 0.5f);
        txt.setPosition(bounds.left + bounds.width * 0.5f,
                        bounds.top + bounds.height * 0.5f - 2);

        window.draw(txt);
    }
};

class FighterSprite {
public:
    sf::Sprite sprite;          // the visual sprite
    sf::Texture texture;        // owned texture
    sf::Vector2f worldPos {0.f, 0.f};   // world-space position

    bool loaded = false;

    bool loadFromFile(const std::string& path) {
        if (!texture.loadFromFile(path))
            return false;

        sprite.setTexture(texture);
        loaded = true;
        return true;
    }

    void setWorldPos(float x, float y) {
        worldPos = {x, y};
    }

    sf::Vector2f getWorldPos() const {
        return worldPos;
    }
    static int adjustPosition(sf::RenderWindow& window,sf::IntRect Fbounds) {
        return 700 -Fbounds.height * 8 + 80;
    }
    void draw(sf::RenderWindow& window, const Camera& cam, int frame, sf::IntRect bounds) {
        if (!loaded) return;

        sf::Vector2f screenPos = cam.apply(worldPos);
        sprite.setPosition(screenPos.x, screenPos.y );//+ adjustPosition(window,bounds));
        sprite.setTextureRect(bounds);
        window.draw(sprite);
    }
};

enum mode {
    hitbox,
    hurtbox,
    movement,
    image,
    projectile,
    MODE_COUNT
};

// ------------------------------------------------------------
// Data and parsing structures
// ------------------------------------------------------------
namespace brainHurt {
    // ------------------------------------------------------------
    // frame data struct
    // ------------------------------------------------------------
    struct FrameData{};

    // ------------------------------------------------------------
    // wrapper object for collisionBoxes that contains frame ownership
    // ------------------------------------------------------------
    template <typename BoxType>
    class BoxSet {
    public:
        BoxSet(const BoxType& box, int ID)
            : box(box), ID(ID) {}

        BoxType box;
        std::vector<int> framesActive;
        int ID = -1;

        // Frame activity
        [[nodiscard]] const std::vector<int>& getActiveFrames() const { return framesActive; }
        void setActiveFrames(const std::vector<int>& frames) { framesActive = frames; }

        void addActiveFrame(int frame) {
            if (!isActiveOnFrame(frame)) framesActive.push_back(frame);
        }

        void removeActiveFrame(int frame) {
            framesActive.erase(std::remove(framesActive.begin(), framesActive.end(), frame), framesActive.end());
        }

        [[nodiscard]] bool isActiveOnFrame(int frame) const {
            return std::find(framesActive.begin(), framesActive.end(), frame) != framesActive.end();
        }

        // Geometry passthrough
        [[nodiscard]] const std::vector<sf::Rect<double>>& getLocalBounds() const {
            return box.getLocalBounds();
        }

        void setLocalBounds(const std::vector<sf::Rect<double>>& localBounds) {
            box.setLocalBounds(localBounds);
            //std::cout<<"localBounds set"<<std::endl;
        }

        std::vector<sf::Rect<double>> getLocalBounds() {
            return box.getLocalBounds();
        }

        void addLocalRect(sf::Rect<double>& b) {
            box.addLocalRect(b);
        }

        void removeLocalRect(int b) {
            box.removeLocalRect(b);
        }


        // Serialization placeholder
        std::string getMainData() const {
            std::string damage = "damage:" + std::to_string(box.getDamage()) + "\n";
            std::string stun  = "stun:" + std::to_string(box.getHitStun()) +","+ std::to_string(box.getBlockStun()) +"\n";
            std::string kb = "knockback:"+std::to_string(box.getKnockBackX())+"x"+std::to_string(box.getKnockBackY())+","+std::to_string(box.getKnockBackXA())+"x"+std::to_string(box.getKnockBackYA())+","+std::to_string(box.getKnockBackTime())+"\n";
            std::string tp  = "type:" +  std::to_string((int)box.getHeight()) + "," +  std::to_string((int)box.getDamageType()) + "," +  std::to_string((int)box.getHitboxType()) + "," +  std::to_string((int)box.getKnockDown()) + "," +  std::to_string((int)box.isGrab());
            return damage + stun + kb + tp; // to be implemented
        }
    };

    // ------------------------------------------------------------
    // objects for holding move data during runtime
    // ------------------------------------------------------------
    struct frameData {
        std::vector<int> hitBoxSets;
        std::vector<int> hurtBoxSets;
        sf::Texture * texture{};
        sf::IntRect imageRegion;
        double gravity{};
        sf::Vector2f movement;
    };

    struct RTE {
        int numFrames;
        int order;
        std::vector<int> sets;

        [[nodiscard]] std::vector<int> getSets() const {
            //std::cout<<"sets: "<<sets[0]<<std::endl;
            return sets;
        }
    };

    class RteHandler {
    public:
        std::vector<RTE> RTEs;
        int totalLength = 0;

        [[nodiscard]] std::vector<int> getSetsOnFrame(int frame) {
            std::cout<<"getSets: start "<<frame<<std::endl;
            sortSets();
            calcTotalLength();
            if (frame >=totalLength) {
                //std::cout<<"frame greater than total length"<<std::endl;
                return {};
            }
            int idx = -1;
            int framesTotal = 0;

            while (framesTotal <= frame) {
                framesTotal += RTEs[idx].numFrames;
                idx++;
                if (idx >= RTEs.size()) {
                    std::cout<<"index greater than RTEs size"<<std::endl;
                    std::cout<<"RTE size: "<<RTEs.size()<<std::endl;
                    return {};
                }
            }

            //std::cout<<"index: "<<idx<<std::endl;

            if (idx == -1) {
                //std::cout<<"no RTE found"<<std::endl;
                return {};
            }
            std::vector<int> sets = RTEs[idx].getSets();
            std::cout<<"returned valid value: { ";
            for (int val:sets) {
                std::cout<<val<<" ";
            }
            std::cout<<"}"<<std::endl;

            return sets;
        }

        void sortSets() {
            std::sort(RTEs.begin(), RTEs.end(), [](const RTE& a, const RTE& b){return a.order>b.order;});
        }

        void addSet(int set) {

        }

        int calcTotalLength() {
            totalLength = 0;
            for (const RTE& r:RTEs) {
                totalLength+=r.numFrames;
            }
            std::cout<<"totalLength: "<<totalLength<<std::endl;
            return totalLength;
        }

        RteHandler(std::vector<RTE>& Rte) {
            for (RTE r:Rte) {
                RTEs.push_back(r);
            }

            calcTotalLength();
        }

        RteHandler() {

        }

    };

    struct movementData {
        sf::Vector2f movement;
        int ID;

        bool operator==(const movementData & movement_data) const {return (movement == movement_data.movement);};
    };

    struct gravData {
        double gravity;
        int ID;

        bool operator==(const gravData & grav_data) const = default;
    };

    struct moveData{
        std::vector<BoxSet<collision::HitBox>> hitBoxes;
        std::vector<BoxSet<collision::HurtBox>> hurtBoxes;
        std::vector<std::vector<int>> hitBoxOrder;
        std::vector<std::vector<int>> hurtBoxOrder;
        std::vector<int> imageOrder;
        std::vector<movementData> movement;//all unique sets of movement
        int xAnchor;
        std::vector<gravData> gravity;//all unique sets of gravity
        sf::Texture * texture;

        int xOffset, yOffset;
        int Fwidth, Fheight;
        int frameNum;

        //stored as std::string so we don't have to do anything risky
        std::string id;
        std::string imgFile;

        //Run-Time Encoding handlers for:
        RteHandler HbH;//Hitboxes
        RteHandler UbH;//Hurtboxes
        RteHandler MvH;//Movement
        RteHandler GvH;//Gravity
        RteHandler ImH;//Images


        frameData getFrameData(int f) {
            sf::IntRect imageRegion = {xOffset, yOffset + Fheight * imageOrder[f], Fwidth, Fheight};
            return {hitBoxOrder[f],hurtBoxOrder[f],texture, imageRegion,gravity[f].gravity,movement[f].movement};
        }
        ~moveData() {
            delete texture;
        }

        void printHitBoxes() {
            for (BoxSet<collision::HitBox>& hitBox : hitBoxes) {
                std::vector<sf::Rect<double>> bounds = hitBox.getLocalBounds();
                for (sf::Rect<double> rect : bounds) {
                    std::cout<<rect.left<<" "<<rect.top<<" "<<rect.width<<" "<<rect.height<<std::endl;
                }
                std::cout <<"---------------"<<std::endl;
            }
        }

        std::vector<int> getActiveHitBoxes(int frame) {
            return HbH.getSetsOnFrame(frame);
        }
        std::vector<int> getActiveHurtBoxes(int frame) {
            return UbH.getSetsOnFrame(frame);
        }


        void initRTEs() {
            UbH.RTEs.clear();
            int num = 0;
            int id = 0;

            std::vector<int> lastVal;
            if (hurtBoxOrder.size() != 0) {
                lastVal = hurtBoxOrder[0];
            }

            std::vector<int> lastSet;
            for (std::vector<int>&set :hurtBoxOrder) {

                if (set == lastVal) {
                    num ++;
                }
                else {
                    UbH.RTEs.emplace_back(num, id,lastVal);
                    std::cout<<"added hurtBox set: "<<num<<" "<<id<<" ";
                    for (int val:lastVal) {
                        std::cout<<val<<" ";
                    }
                    std::cout<<std::endl;
                    num = 0;
                    lastVal = set;
                    id++;
                }
                lastSet = std::move(set);
            }
            UbH.RTEs.emplace_back(num, id,lastVal);
            std::cout<<"added hurtBox set: "<<num<<" "<<id<<" { ";
            for (int val:lastVal) {
                std::cout<<val<<" ";
            }
            std::cout<<"}"<<std::endl;
            HbH.RTEs.clear();
            num = 0;
            id = 0;

            if (hitBoxOrder.size() != 0) {
                lastVal = hitBoxOrder[0];
            }


            for (std::vector<int>&set :hitBoxOrder) {
                if (set == lastVal) {
                    num ++;
                }
                else {
                    HbH.RTEs.emplace_back(num, id,set);
                    num = 0;
                    lastVal = set;
                    id++;
                }
            }

            HbH.RTEs.emplace_back(num, id,lastVal);

            num = 0;
            id = 0;
            MvH.RTEs.clear();

            movementData lastMd;
            if (movement.size() != 0) {
                lastMd= movement[0];
            }

            for (movementData& vec: movement) {
                if (vec == lastMd) {
                    num ++;
                    std::cout<<lastMd.movement.x<<" "<<num<<std::endl;
                }
                else {
                    MvH.RTEs.emplace_back(num, id, std::vector{vec.ID});
                    num = 0;
                    id ++;
                    lastMd = vec;

                }
            }
            MvH.RTEs.emplace_back(num, id, std::vector{lastMd.ID});
            std::cout<<MvH.RTEs[0].numFrames<<std::endl;



            num = 0;
            id = 0;
            GvH.RTEs.clear();

            gravData lastGv;
            if (gravity.size() != 0) {
                lastGv = gravity[0];
            }

            for (gravData& gv: gravity) {
                if (gv == lastGv) {
                    num ++;
                }
                else {
                    GvH.RTEs.emplace_back(num, id, std::vector{gv.ID});
                    num = 0;
                    lastGv = gv;
                    id ++;
                }
            }
            GvH.RTEs.emplace_back(num, id, std::vector{lastGv.ID});
        }

    };

    // ------------------------------------------------------------
    // parser object
    // ------------------------------------------------------------
    class Parser {
        std::string fileName;
        std::vector<std::string> fileLines;

    public:
        Parser(std::string filename) : fileName(filename) {
            std::ifstream vals = std::ifstream(filename);
            //std::cout<<vals.is_open()<<std::endl;
            if (vals.is_open()) {
                std::string line;
                while (std::getline(vals, line)) {
                    if (line.rfind("file:",0)!=0) {
                        std::transform(line.begin(), line .end(), line.begin(),
                        [](unsigned char c){ return std::tolower(c); });
                    }

                    fileLines.push_back(line);
                    std::cout<<line<<std::endl;
                }
            }
            else {
                //std::cout<<"failed to open "<<filename<<std::endl;
            }
        }

        moveData parseMoveData() {
            int parsetype = 0; // 0 = normal, 1 = hitbox, 2 = hurtbox, 3 = projectile
            std::string id = "00000";
            std::string imgFile = "idle.png";

            std::vector<BoxSet<collision::HitBox> > hitBoxes;
            std::vector<BoxSet<collision::HurtBox> > hurtBoxes;

            std::vector<std::vector<int> > hitBoxOrder;
            std::vector<std::vector<int> > hurtBoxOrder;

            std::vector<double> gravity;
            std::vector<sf::Vector2f> movement;

            int width = 0, height = 0;
            int xoffset = 0, yoffset = 0;

            int xAnchor = 0, yAnchor = 0;

            std::vector<int> imageOrder;
            sf::Texture *texture = nullptr;

            std::vector<std::array<int, 4> > localBounds; // temp for pos: lines
            int currentID = -1;

            int frameNum = 0;
            // temp hitbox data fields
            int damage = 0;
            int knockBackX = 0, knockBackY = 0;
            int knockBackXA = 0, knockBackYA = 0;
            int knockBackTime = 0;
            int hitStun = 0, blockStun = 0;
            int hHeight = 0, dType = 0, bType = 0;
            int knockDown = 0, grab = 0;

            for (std::string line: fileLines) {
                if (line.empty()) continue;

                auto flag = util::split(line, ':');
                if (flag.empty()) continue;

                // -------------------------------
                // NORMAL PARSING
                // -------------------------------
                if (parsetype == 0) {
                    if (flag[0] == "id") {
                        // move id, not used for individual boxes
                        id = flag[1];
                    }

                    if (flag[0] == "file") {
                        imgFile = flag[1];
                        texture = new sf::Texture();
                        texture->loadFromFile("../__pic/"+flag[1]);
                    }

                    if (flag[0] == "framenum") {
                        frameNum = std::stoi(flag[1]);
                    }

                    if (flag[0] == "frameorder") {
                        // example: "5x0 2x1 4x2 6x3"
                        auto tokens = util::split(flag[1], ' ');
                        for (auto &t: tokens) {
                            if (t.empty()) continue;
                            auto parts = util::split(t, 'x');
                            if (parts.size() != 2) continue;
                            int count = std::stoi(parts[0]);
                            int idx = std::stoi(parts[1]);
                            for (int i = 0; i < count; ++i)
                                imageOrder.push_back(idx);
                        }
                    }

                    if (flag[0] == "frameanchor") {
                        auto tokens = util::split(flag[1], 'x');
                        xAnchor = std::stoi(tokens[0]);
                    }

                    if (flag[0] == "movement") {
                        // example: "movement:0x0,0"
                        //std::cout<<"parsing movement: ";
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            //std::cout<<"num: "<< data2[0]<<" ";
                            int numFrames = std::stoi(data2[0]);
                            if (data2.size()>1) {
                                std::vector<std::string> data3 = util::split(data2[1], ',');
                                //std::cout<<numFrames<<std::endl;
                                if (data3.size()>1) {
                                    for (int k = 0; k< numFrames; k++) {
                                        //std::cout<<"mov: "<< data3[0]<<" "<<data3[1];
                                        movement[num].x = stod(data3[0]);
                                        movement[num].y = stod(data3[1]);
                                        //std::cout<<"added movement: "<< movements[num][0]<<" "<<num<<std::endl;
                                        num++;
                                    }
                                }
                            }
                            //std::cout<<std::endl;
                        }
                    }

                    if (flag[0] == "momentumcarry") {
                        // store if needed
                    }

                    if (flag[0] == "gravity") {
                        // example: "gravity:19x1"
                        int num = 0;
                        std::vector<std::string> data = util::split(flag[1], ' ');
                        for (int i = 0; i< data.size(); i++) {
                            std::vector<std::string> data2 = util::split(data[i], 'x');
                            if (data2.size()>1) {
                                int numFrames = std::stoi(data2[0]);
                                for (int j = 0; j< numFrames; j++) {
                                    if (data2.size()>1) {
                                        gravity.push_back(std::stod(data2[1]));
                                        //std::cout << std::stod(data2[1]) << std::endl;
                                        num++;
                                    }
                                }
                            }
                        }
                    }

                    if (flag[0] == "framesize") {
                        // framesize:54x65
                        auto parts = util::split(flag[1], 'x');
                        if (parts.size() == 2) {
                            width = std::stoi(parts[0]);
                            height = std::stoi(parts[1]);
                        }
                    }

                    if (flag[0] == "offset") {
                        // offset:0x0
                        auto parts = util::split(flag[1], 'x');
                        if (parts.size() == 2) {
                            xoffset = std::stoi(parts[0]);
                            yoffset = std::stoi(parts[1]);
                        }
                    }

                    if (flag[0] == "hitboxorder") {
                        // Example: "5x0,1 3x2 -1x-1"
                        std::vector<std::string> tokens = util::split(flag[1], ' ');

                        for (auto& t : tokens) {
                            if (t.empty()) continue;

                            auto parts = util::split(t, 'x');
                            if (parts.size() != 2) continue;

                            int duration = std::stoi(parts[0]);
                            std::string boxList = parts[1];

                            // Sentinel: -1x-1 means "no hitboxes for all frames"
                            if (duration == -1 && boxList == "-1") {
                                hitBoxOrder.resize(imageOrder.size());
                                for (auto& v : hitBoxOrder) v.clear();
                                continue;
                            }

                            // Parse comma-separated box indices
                            std::vector<int> activeSets;
                            auto indices = util::split(boxList, ',');
                            for (auto& idx : indices) {
                                if (!idx.empty())
                                    activeSets.push_back(std::stoi(idx));
                            }

                            // Expand RLE
                            for (int i = 0; i < duration; i++) {
                                hitBoxOrder.push_back(activeSets);
                            }
                        }

                        // Clamp to total frames
                        if (hitBoxOrder.size() > imageOrder.size())
                            hitBoxOrder.resize(imageOrder.size());
                    }

                    if (flag[0] == "hurtboxorder") {
                        // Example: "30x0,2"
                        std::vector<std::string> tokens = util::split(flag[1], ' ');

                        for (auto& t : tokens) {
                            if (t.empty()) continue;

                            auto parts = util::split(t, 'x');
                            if (parts.size() != 2) continue;

                            int duration = std::stoi(parts[0]);
                            std::string boxList = parts[1];

                            // Sentinel: -1x-1 means "no hurtboxes"
                            if (duration == -1 && boxList == "-1") {
                                hurtBoxOrder.resize(imageOrder.size());
                                for (auto& v : hurtBoxOrder) v.clear();
                                continue;
                            }


                            // Parse comma-separated box indices
                            std::vector<int> activeSets;
                            auto indices = util::split(boxList, ',');
                            for (auto& idx : indices) {
                                if (!idx.empty())
                                    activeSets.push_back(std::stoi(idx));
                                if (stoi(idx) == -1) {
                                    activeSets.clear();
                                    break;
                                }
                            }

                            // Expand RLE
                            for (int i = 0; i < duration; i++) {
                                hurtBoxOrder.push_back(activeSets);
                            }
                        }

                        // Clamp to total frames
                        if (hurtBoxOrder.size() > imageOrder.size())
                            hurtBoxOrder.resize(imageOrder.size());
                    }

                    if (flag[0] == "hitbox_start") {
                        parsetype = 1;
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
                    }

                    if (flag[0] == "hurtbox_start") {
                        parsetype = 2;
                        currentID++;
                        localBounds.clear();
                    }

                    if (flag[0] == "projectile_start") {
                        parsetype = 3;
                    }
                    //std::cout<<flag[0]<<std::endl;
                }

                // -------------------------------
                // HITBOX PARSING
                // -------------------------------
                else if (parsetype == 1) {
                    if (flag[0] == "hitbox_end") {
                        parsetype = 0;
                        continue;
                    }

                    if (flag[0] == "pos") {
                        // pos:-13x45,26x45
                        localBounds.clear();
                        auto parts = util::split(flag[1], ',');
                        for (int i = 0; i< parts.size()-1; i+=2) {
                            auto xy = util::split(parts[i], 'x');
                            auto wh  = util::split(parts[i+1], 'x');
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
                        // type:height,boxType,damageType,knockdown,grab
                        auto parts = util::split(flag[1], ',');
                        if (parts.size() >= 1) hHeight = std::stoi(parts[0]);
                        if (parts.size() >= 2) bType = std::stoi(parts[1]);
                        if (parts.size() >= 3) dType = std::stoi(parts[2]);
                        if (parts.size() >= 4) knockDown = std::stoi(parts[3]);
                        if (parts.size() >= 5) grab = std::stoi(parts[4]);
                    }

                    if (flag[0] == "id") {
                        currentID = stoi(flag[1]);
                    }

                    if (flag[0].rfind("save", 0) == 0) {
                        // build dat array for new HitBox ctor
                        std::array<int, 14> dat{};
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

                        collision::HitBox hb(localBounds, dat);
                        BoxSet<collision::HitBox> set(hb, currentID);
                        hitBoxes.push_back(set);
                        localBounds.clear();
                    }
                }

                // -------------------------------
                // HURTBOX PARSING
                // -------------------------------
                else if (parsetype == 2) {
                    if (flag[0] == "hurtbox_end") {
                        parsetype = 0;
                        continue;
                    }

                    if (flag[0] == "pos") {
                        // pos:-13x45,26x45
                        localBounds.clear();
                        auto parts = util::split(flag[1], ',');
                        for (int i = 0; i< parts.size()-1; i+=2) {
                            auto xy = util::split(parts[i], 'x');
                            auto wh  = util::split(parts[i+1], 'x');
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

                    if (flag[0] == "id") {
                        currentID = stoi(flag[1]);
                    }

                    if (flag[0].rfind("save", 0) == 0) {
                        collision::HurtBox hb(localBounds, collision::hurtboxType::HURTBOX);
                        BoxSet<collision::HurtBox> set(hb, currentID);
                        hurtBoxes.push_back(set);
                        localBounds.clear();
                    }
                }

                // -------------------------------
                // PROJECTILE PARSING (stub)
                // -------------------------------
                else if (parsetype == 3) {
                    if (flag[0] == "PROJECTILE_END") {
                        parsetype = 0;
                    }
                }
            }

            while (movement.size()<imageOrder.size()) {
                movement.emplace_back(0,0);
            }
            while (gravity.size()<imageOrder.size()) {
                gravity.push_back(0.f);
            }

            moveData md;
            md.id = id;
            md.imgFile = imgFile;
            md.frameNum = frameNum;
            md.hitBoxes = std::move(hitBoxes);
            md.hurtBoxes = std::move(hurtBoxes);
            md.hitBoxOrder = std::move(hitBoxOrder);
            md.hurtBoxOrder = std::move(hurtBoxOrder);
            md.imageOrder = std::move(imageOrder);
            for (int i = 0; i< movement.size(); i++) {
                md.movement.push_back({movement[i],i});
            }
            for (int i = 0; i< gravity.size(); i++) {
                md.gravity.push_back({gravity[i],i});
            }
            md.texture = texture;
            md.xAnchor = xAnchor;
            md.xOffset = xoffset;
            md.yOffset = yoffset;
            md.Fwidth = width;
            md.Fheight = height;
            md.initRTEs();

            return md;
        }

    };

    // ------------------------------------------------------------
    // writer object
    // ------------------------------------------------------------
    class Writer {
    public:
        std::string file;
        std::ofstream out;

        Writer (std::string fileName) {
            file = fileName;
            //out.open(file.c_str());
        }

        void writeData(const moveData& md) {
            std::ostringstream movDat;
            movDat << "id:" << md.id << "\n";
            movDat << "file:" << md.imgFile << "\n";
            movDat << "\n//main-line animation\n";
            movDat << "frameNum:"<<md.frameNum<<"\n";

            std::ostringstream hitbox;
            hitbox << "HITBOX_START:\n";

            // HITBOXES
            for (const BoxSet<collision::HitBox>& box : md.hitBoxes) {
                const auto& bounds = box.getLocalBounds();
                if (!bounds.empty()) {

                    bool first = true;
                    for (const auto& rect : bounds) {
                        if (!first) hitbox << ",";
                        hitbox << "pos:" << rect.left << "x" << rect.top
                               << "," << rect.width << "x" << rect.height;
                        first = false;
                    }

                    hitbox << "\n";
                    hitbox << "id:" << box.ID << "\n";
                    hitbox << box.getMainData() << "\n";
                    hitbox << "SAVE:-------\n";
                }
            }

            hitbox << "HITBOX_END\n\n";
            hitbox << "hitboxorder:\n\n";

            // HURTBOXES
            std::ostringstream hurtbox;
            hurtbox << "HURTBOX_START:\n";

            for (const BoxSet<collision::HurtBox>& box : md.hurtBoxes) {
                const auto& bounds = box.getLocalBounds();
                if (!bounds.empty()) {

                    bool first = true;
                    for (const auto& rect : bounds) {
                        if (!first) hurtbox << ",";
                        hurtbox << "pos:" << rect.left << "x" << rect.top
                                << "," << rect.width << "x" << rect.height;
                        first = false;
                    }

                    hurtbox << "\n";
                    hurtbox << "id:" << box.ID << "\n";
                    hurtbox << "SAVE:-------\n";
                }
            }

            // OUTPUT
            std::cout << hitbox.str() << hurtbox.str() << std::endl;
        }
    };
}


// ------------------------------------------------------------
// Editor Class
// ------------------------------------------------------------
class Editor {

public:
    brainHurt::Writer w = brainHurt::Writer("noFile");
    int heightOffset = 700;
    Editor(float winW, float winH, const sf::Font& font)
        : winW(winW), winH(winH), font(font)
    {
        initTextboxGrid();
        timelineY = grid[0][0].bounds.top - 60.f;
        initModeButtons();
        cam.setOffset(-winW/2,0);

        framDat = movDat.getFrameData(6);

        if (!fighterSprite.loadFromFile("../__pic/idle.png")) {
            std::cerr << "Failed to load fighter sprite\n";
        }

        // Set initial world-space position of the fighter
        fighterSprite.setWorldPos(0 ,heightOffset -movDat.Fheight * 8);

        // Optionally center the sprite on its origin
        if (fighterSprite.loaded) {
            sf::FloatRect b = fighterSprite.sprite.getLocalBounds();
            fighterSprite.sprite.setOrigin(movDat.xAnchor,0);
            fighterSprite.sprite.setScale(8,8);
        }

        int num = movDat.hitBoxes.size();
        while (movDat.hitBoxes.size()<10) {
            movDat.hitBoxes.emplace_back(collision::HitBox(), num);
            num++;
        }
        num = movDat.hurtBoxes.size();
        while (movDat.hurtBoxes.size()<10) {
            movDat.hurtBoxes.emplace_back(collision::HurtBox(), num);
            num++;
        }

        rects.clear();
        for (int i = 0; i<10; i++) {
            if (currentMode == hurtbox) {
                makeBoxRects(movDat.hurtBoxes[i]);
            }
            if (currentMode == hitbox) {
                makeBoxRects(movDat.hitBoxes[i]);
            }
        }
        makeDisplayRects();


        //w.writeData(movDat);

    }

    void handleMousePress(sf::Vector2f mouse, sf::Mouse::Button button) {
        if (button == sf::Mouse::Right) {
            createRect(cam.toWorld(mouse));
            return;
        }

        if (button == sf::Mouse::Left) {
            handleLeftClick(mouse);
        }
    }

    void handleMouseRelease(sf::Mouse::Button button) {
        if (button == sf::Mouse::Left && activeRect) {
            activeRect->release();
            activeRect = nullptr;
        }
        if (button == sf::Mouse::Left) {
            draggingTimeline = false;
        }

    }

    void handleKeyPress(sf::Keyboard::Key key) {
        handleTextboxTyping(key);
        handleRectangleKeys(key);
        handleArrows(key);
        if (key == sf::Keyboard::P) {
            rects.clear();
            for (int i = 9; i>=0; i--) {
                if (currentMode == hitbox) {
                    makeBoxRects(movDat.hitBoxes[i]);
                }
                else {
                    makeBoxRects(movDat.hurtBoxes[i]);
                }
            }
        }
    }

    void updateLocalBounds() {
        if (lastMode == hitbox) {
            std::array<std::vector<sf::Rect<double>>,10> newHitBounds;
            for (int i =0; i < rects.size(); i++) {
                newHitBounds[rects[i].group].push_back(static_cast<sf::Rect<double>>(
                sf::Rect{std::round(rects[i].left/8),
                         std::round(-(rects[i].top-heightOffset+80)/8),
                        std::round((rects[i].right - rects[i].left)/8),
                        std::round((rects[i].bottom - rects[i].top)/8)}));
            }
            for (int i = 0; i<10; i++) {
                movDat.hitBoxes[i].setLocalBounds(newHitBounds[i]);
            }
        }
        else if (lastMode == hurtbox) {
            std::array<std::vector<sf::Rect<double>>,10> newHurtBounds;
            for (int i =0; i < rects.size(); i++) {
                newHurtBounds[rects[i].group].push_back(static_cast<sf::Rect<double>>(
                        sf::Rect{std::round(rects[i].left/8),
                                 std::round(-(rects[i].top-heightOffset+80)/8),
                                std::round((rects[i].right - rects[i].left)/8),
                                std::round((rects[i].bottom - rects[i].top)/8)}));
            }
            for (int i = 0; i<10; i++) {
                movDat.hurtBoxes[i].setLocalBounds(newHurtBounds[i]);
            }
        }
        //w.writeData(movDat);

    }


    void update(sf::RenderWindow& window) {
        if (currentFrame != lastFrame || currentMode != lastMode) {
            updateLocalBounds();

            rects.clear();
            for (int i = 0; i<10; i++) {
                if (currentMode == hitbox) {
                    makeBoxRects(movDat.hitBoxes[i]);
                }
                else if (currentMode == hurtbox) {
                    makeBoxRects(movDat.hurtBoxes[i]);
                }
            }
            makeDisplayRects();

            writeTextboxGrid();
        }
        setActives();

        totalFrames = movDat.imageOrder.size();
        framDat = movDat.getFrameData(currentFrame);

        sf::Vector2f mausu = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        sf::Vector2f m = cam.toWorld(mausu);

        if (activeRect) {
            activeRect->drag(sf::Vector2f(m.x, m.y), winW, winH, cam);
        }
        if (draggingTimeline) {
            float mx = static_cast<float>(mausu.x);

            float cellW = (winW - 24 - (totalFrames - 1) * 2) / totalFrames;;
            float xStart = 12.f;

            int newFrame = (mx - xStart) / (cellW + 2);
            if (newFrame < 0) newFrame = 0;
            if (newFrame >= totalFrames) newFrame = totalFrames - 1;

            currentFrame = newFrame;

            float my = static_cast<float>(m.y);

            float cellH = 24.f;
            float y0 = timelineY - 12.f;
            float y1 = timelineY + cellH;

            if (my < y0 || my > y1) {
                draggingTimeline = false;
            }

        }
        lastFrame = currentFrame;
        lastMode = currentMode;
        //movDat.printHitBoxes();
    }


    void draw(sf::RenderWindow& window) {
        fighterSprite.draw(window, cam, currentFrame, framDat.imageRegion);

        for (auto& r: staticRects)
            r.draw(window, cam);
        for (auto& r : rects)
            r.draw1(window, font, cam);
        for (auto& r : rects)
            r.draw2(window, font, cam);

        drawTimeline(window);

        for (auto& b : modeButtons)
            b.draw(window, font);

        for (auto& row : grid)
            for (auto& tb : row)
                tb.draw(window, font);

        drawCameraOffsetOverlay(window);
    }

private:
    brainHurt::Parser p =  brainHurt::Parser("../__val/idle.txt");
    brainHurt::moveData movDat = p.parseMoveData();
    brainHurt::frameData framDat;
    FighterSprite fighterSprite;

    Camera cam;

    float winW, winH;
    const sf::Font& font;

    std::vector<DisplayRect> staticRects;
    std::vector<DraggableRect> rects;
    DraggableRect* activeRect = nullptr;

    std::vector<std::vector<Textbox>> grid;
    Textbox* activeTextbox = nullptr;

    mode currentMode = hitbox;
    mode lastMode = hitbox;
    std::vector<Button> modeButtons;

    int currentFrame = 0;
    int lastFrame = 0;
    int totalFrames = 60; // default, can be changed
    float timelineY;
    bool draggingTimeline = false;



    // ------------------------------------------------------------
    // Grid Initialization
    // ------------------------------------------------------------
    void initTextboxGrid() {
        const int cols = 45;
        const int rows = 5;
        const float tbW = 32.f;
        const float tbH = 24.f;
        const float pad = 3.f;

        float gridHeight = rows * (tbH + pad) - pad;
        float startX = 12.f;
        float startY = winH - gridHeight - 20.f;

        grid.resize(rows, std::vector<Textbox>(cols, Textbox(0,0,0,0, "")));

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                float x = startX + c * (tbW + pad);
                float y = startY + r * (tbH + pad);
                grid[r][c] = Textbox(x, y, tbW, tbH,"");
            }
        }
    }

    void initTextboxGrid(int rw, int cl) {
        const int cols = cl;
        const int rows = rw;
        const float tbW = 32.f;
        const float tbH = 24.f;
        const float pad = 3.f;

        float gridHeight = rows * (tbH + pad) - pad;
        float startX = 12.f;
        float startY = winH - gridHeight - 20.f;

        grid.resize(rows, std::vector<Textbox>(cols, Textbox(0,0,0,0, "")));

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                float x = startX + c * (tbW + pad);
                float y = startY + r * (tbH + pad);
                grid[r][c] = Textbox(x, y, tbW, tbH,"");
            }
        }
    }

    // ------------------------------------------------------------
    // Button Initialization
    // ------------------------------------------------------------
    void initModeButtons() {
        const float buttonH = 24.f;
        const float extraPad = 12.f;

        // Determine grid top Y
        float gridTopY = grid[0][0].bounds.top;

        float y = gridTopY - buttonH - extraPad;

        // Labels for each mode
        std::vector<std::string> labels = {
            "Hitbox", "Hurtbox", "Movement", "Image", "Projectile"
        };

        int count = labels.size();
        modeButtons.clear();
        modeButtons.reserve(count);

        float totalWidth = winW;
        float buttonW = totalWidth / count;

        for (int i = 0; i < count; i++) {
            float x = i * buttonW;
            modeButtons.emplace_back(x + 4, y, buttonW - 8, buttonH, labels[i]);
        }

        // Default: first mode active
        modeButtons[0].toggled = true;
        currentMode = hitbox;
    }

    // ------------------------------------------------------------
    // Rectangle Logic
    // ------------------------------------------------------------
    void createRect(sf::Vector2f mouse) {
        float w = 120.f;
        float h = 80.f;

        rects.emplace_back(mouse.x, mouse.y, mouse.x + w, mouse.y + h);
        sf::Rect<double> re = static_cast<sf::Rect<double>>(
                    sf::Rect<int>{static_cast<int>(mouse.x/8 + 0.5),
                             static_cast<int>(mouse.y/8 + 0.5),
                            static_cast<int>(w/8 + 0.5),
                            static_cast<int>(h/8 + 0.5)});
        if (currentMode == hitbox) {
            movDat.hitBoxes[0].addLocalRect(re);
        }
        if (currentMode == hurtbox) {
            movDat.hurtBoxes[0].addLocalRect(re);
        }
        std::cout << 0 <<" "<<re.top<<" "<<re.left<<" "<<re.width<<" "<<re.height<<std::endl;

        for (auto& r : rects) r.selected = false;
        rects.back().selected = true;
    }


    void makeBoxRects(brainHurt::BoxSet<collision::HitBox> set) {
        for (auto& r : set.getLocalBounds()) {
            float realTop = (heightOffset) - 80 -r.top *8;
            rects.emplace_back(r.left * 8, realTop,(r.left+r.width)*8,realTop+(r.height)*8);
            rects.back().group = set.ID;
        }
    }

    void makeBoxRects(brainHurt::BoxSet<collision::HurtBox> set) {
        for (auto& r : set.getLocalBounds()) {
            float realTop = (heightOffset) - 80 -r.top *8;
            rects.emplace_back(r.left * 8,realTop,(r.left+r.width)*8,realTop + (r.height)*8);
            rects.back().group = set.ID;
        }
    }

    void makeDisplayRects() {
        staticRects.clear();
        if (currentMode != hurtbox) {
            for (int i = 0; i<10; i++) {
                auto set = movDat.hurtBoxes[i];
                for (auto r : set.getLocalBounds()) {
                    float realTop = (heightOffset) - 80 -r.top *8;
                    staticRects.emplace_back(hurt, r.left * 8,realTop,(r.width)*8 + 1,(r.height)*8 + 1,set.ID);
                }
            }
        }
        if (currentMode != hitbox) {
            for (int i = 0; i<10; i++) {
                auto set = movDat.hitBoxes[i];
                for (auto r : set.getLocalBounds()) {
                    float realTop = (heightOffset) - 80 -r.top *8;
                    staticRects.emplace_back(hit, r.left * 8,realTop,(r.width)*8,(r.height)*8,set.ID);
                }
            }
        }
    }

    void setActives() {
        if (currentMode == hitbox) {
            for (DraggableRect & r1: rects) {
                r1.active = false;
                for (int id: movDat.getActiveHitBoxes(currentFrame)) {
                    if (id == r1.group) {
                        r1.active = true;
                        //std::cout<<"Rect made active: hitbox "<<r.group<<std::endl;
                        break;
                    }
                }
            }
            for (DisplayRect & r2: staticRects) {
                r2.active = false;
                for (int id: movDat.getActiveHurtBoxes(currentFrame)) {
                    if (id == r2.group) {
                        r2.active = true;
                        //std::cout<<"Rect made active: hurtbox "<<r.group<<std::endl;
                        break;
                    }
                }
            }
        }
        else if (currentMode == hurtbox) {
            for (DraggableRect & r3: rects) {
                //std::cout<<"group: "<<r.group<<" ";
                r3.active = false;
                for (int id: movDat.getActiveHurtBoxes(currentFrame)) {
                    if (id == r3.group) {
                        r3.active = true;
                        //std::cout<<"Rect made active: hurtbox "<<r.group<<std::endl;
                        break;
                    }
                }
                //std::cout<<std::endl;
            }
            for (DisplayRect & r4: staticRects) {
                r4.active = false;
                for (int id: movDat.getActiveHitBoxes(currentFrame)) {
                    if (id == r4.group) {
                        r4.active = true;
                        //std::cout<<"Rect made active: hitbox "<<r.group<<std::endl;
                        break;
                    }
                }
            }
        }
    }

    //clears all values in the textbox grid
    void clearTextboxGrid() {
        for (int i = 0; i<grid.size(); i++) {
            for (int j = 0; j<grid[i].size(); j++) {
                grid[i][j].value = "";
            }
        }
    }

    //writes the correct values into the textbox grid according to the current mode of the editor
    void writeTextboxGrid() {
        if (currentMode == hitbox) {//in hitbox mode, read hitbox order
            initTextboxGrid(5,45);
            for (int i = 0; i<movDat.HbH.RTEs.size() && i<grid[0].size(); i++) {

                //write frame number value
                grid[0][i].value = std::to_string(movDat.HbH.RTEs[i].numFrames);

                //write sets active
                for (int j = 1; j<grid.size() && j<= movDat.HbH.RTEs[i].sets.size(); j++) {
                    grid[j][i].value = std::to_string(movDat.HbH.RTEs[i].sets[j-1]);
                }
            }
        }
        else if (currentMode == hurtbox) {//in hurtbox mode, read hurtbox order
            initTextboxGrid(5, 45);
            for (int i = 0; i<movDat.UbH.RTEs.size() && i<grid[0].size(); i++) {

                //write frame number value
                grid[0][i].value = std::to_string(movDat.UbH.RTEs[i].numFrames);

                //write sets active
                for (int j = 1; j<grid.size() && j<= movDat.UbH.RTEs[i].sets.size(); j++) {
                    grid[j][i].value = std::to_string(movDat.UbH.RTEs[i].sets[j-1]);
                }
            }
        }
        else if (currentMode == movement) {//in movement mode, read movement data
            initTextboxGrid(5,30);

            for (int i = 0; i<movDat.MvH.RTEs.size() && i<grid.size(); i++) {

                //write frame number value for movement
                grid[0][i].value = std::to_string(movDat.MvH.RTEs[i].numFrames);

                //write movement values
                if (movDat.MvH.RTEs[i].sets.size() > 0) {
                    grid[1][i].value = std::format("{:.2f}",movDat.movement[movDat.MvH.RTEs[i].sets[0]].movement.x);
                    grid[2][i].value = std::format("{:.2f}",movDat.movement[movDat.MvH.RTEs[i].sets[0]].movement.y);
                }
                else {
                    grid[1][i].value = "";
                    grid[2][i].value = "";
                }


                //write frame number value for gravity
                grid[3][i].value = std::to_string(movDat.GvH.RTEs[i].numFrames);

                //write gravity value
                if (movDat.GvH.RTEs[i].sets.size() > 0) {
                    grid[4][i].value = std::format("{:.2f}",movDat.gravity[movDat.GvH.RTEs[i].sets[0]].gravity);
                }
                else {
                    grid[4][i].value = "";
                }

            }
        }
        else if (currentMode == image) {
            initTextboxGrid(5,45);
            for (int i = 0; i<movDat.ImH.RTEs.size() && i<grid.size(); i++) {
                grid[0][i].value = std::to_string(movDat.ImH.RTEs[i].numFrames);
                grid[1][i].value = std::to_string(movDat.ImH.RTEs[i].sets[0]);
            }

        }
    }

    //reads all values in the textbox grid and maps them to RTEs
    void readTextboxGrid() {
        if (currentMode != lastMode) return;

        if (currentMode == hitbox) {//write hitbox data:
            for (int i = 0; i<grid[0].size(); i++) {

                try {

                    //check for a valid number of frames
                    movDat.HbH.RTEs[i].numFrames = std::stoi(grid[0][i].value);

                    //parse for box sets
                    movDat.HbH.RTEs[i].sets.clear();
                    for (int j = 1;j<grid.size();j++) {
                        try {
                            int temp = std::stoi(grid[j][i].value);
                            if (temp >=0 && temp < 10) {
                                movDat.HbH.RTEs[i].sets.push_back(temp);
                            }
                        }
                        catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                            continue;
                        }
                    }

                } catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                    continue;
                } catch (const std::out_of_range& e) {
                    int num;
                    try {
                        num = std::stoi(grid[1][i].value);
                    }catch (std::invalid_argument& f){
                        num = 0;
                    }

                    movDat.HbH.RTEs.push_back(brainHurt::RTE{std::stoi(grid[0][i].value), i, {num}});
                }

            }
        }
        else if (currentMode == hurtbox) {
            int curr = 0;
            for (int i = 0; i<grid[0].size(); i++) {

                try {

                    //check for a valid number of frames
                    movDat.UbH.RTEs.at(i).numFrames = std::stoi(grid[0][i].value);

                    //parse for box sets
                    movDat.UbH.RTEs.at(i).sets.clear();
                    for (int j = 1;j<grid.size();j++) {
                        curr = j;
                        try {
                            int temp = std::stoi(grid[j][i].value);
                            if (temp >=0 && temp < 10) {
                                movDat.UbH.RTEs.at(i).sets.push_back(temp);
                            }
                        }
                        catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                            continue;
                        }
                    }

                } catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                    continue;
                } catch (const std::out_of_range& e) {
                    int num;
                    try {
                        num = std::stoi(grid[1][i].value);
                    }catch (std::invalid_argument& f){
                        num = 0;
                    }

                    movDat.UbH.RTEs.push_back(brainHurt::RTE{std::stoi(grid[0][i].value), i, {num}});
                }

            }
        }
        else if (currentMode == movement) {
            movDat.movement.clear();
            movDat.gravity.clear();
            for (int i = 0; i<grid.size(); i++) {
                float movX = 0;
                float movY = 0;

                try {//check for movement number
                    //check for a valid number of frames
                    movDat.MvH.RTEs[i].numFrames = std::stoi(grid[0][i].value);
                } catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                    continue;
                } catch (const std::out_of_range& e) {
                    movDat.MvH.RTEs.push_back(brainHurt::RTE{std::stoi(grid[0][i].value)});
                }

                try {//check for X
                    movX = std::stof(grid[1][i].value);
                }catch (const std::invalid_argument& e) {}

                try {//check for Y
                    movY = std::stof(grid[2][i].value);
                }catch (const std::invalid_argument& e) {}
                movDat.movement.push_back({{movX,movY},i});
                movDat.MvH.RTEs[i].sets.resize(1,i);
            }
            for (int i = 0; i<grid.size(); i++) {
                double grav = 0;

                try {//check for grav number
                    //check for a valid number of frames
                    movDat.GvH.RTEs[i].numFrames = std::stoi(grid[3][i].value);
                } catch (const std::invalid_argument& e) {//skip loop iteration if string is invalid
                    continue;
                }

                try {//check for gravity
                    grav = std::stof(grid[4][i].value);
                }catch (const std::invalid_argument& e) {}

                movDat.gravity.push_back({grav,i});
                movDat.GvH.RTEs[i].sets.resize(1,i);
            }
        }
        else if (currentMode == image) {

        }
    }



    void drawTimeline(sf::RenderWindow& window) {
        float cellW = (winW - 24 - (totalFrames - 1) * 2) / totalFrames;
        float cellH = 24.f;
        float xStart = 12.f;

        for (int i = 0; i < totalFrames; i++) {
            sf::RectangleShape cell({cellW, cellH});
            cell.setPosition(xStart + i * (cellW + 2), timelineY);

            if (i == currentFrame)
                cell.setFillColor(sf::Color(255, 255, 255));
            else
                cell.setFillColor(sf::Color(100, 120, 160));

            cell.setOutlineColor(sf::Color::Black);
            cell.setOutlineThickness(1.f);

            window.draw(cell);
        }
    }

    void handleLeftClick(sf::Vector2f mouseLocal) {
        bool clickedSomething = false;

        // Timeline scrubber click or drag start
        float cellH = 24; // whatever height you chosexxx
        float cellW = (winW - 24 - (totalFrames - 1) * 2) / totalFrames;// computed dynamically
        float xStart = 12.f;
        sf::Vector2f mouse = mouseLocal;
        sf::Vector2f mouseWorld = cam.toWorld(mouseLocal);

        if (mouse.y >= timelineY && mouse.y <= timelineY + cellH) {
            for (int i = 0; i < totalFrames; i++) {
                float x = xStart + i * (cellW + 2);
                if (mouse.x >= x && mouse.x <= x + cellW) {
                    currentFrame = i;
                    draggingTimeline = true;   // <-- start dragging
                    return;
                }
            }
        }



        // Mode buttons
        for (int i = 0; i < modeButtons.size(); i++) {
            if (modeButtons[i].contains(mouse)) {

                // Reset all buttons
                for (auto& b : modeButtons)
                    b.toggled = false;

                // Toggle only this one
                modeButtons[i].toggled = true;

                // Update current mode
                currentMode = static_cast<mode>(i);

                return; // Do not interact with textboxes or rectangles
            }
        }

        // Textboxes
        activeTextbox = nullptr;
        for (auto& row : grid) {
            for (auto& tb : row) {
                if (tb.contains(mouse)) {
                    tb.selected = true;
                    activeTextbox = &tb;
                    clickedSomething = true;
                } else {
                    tb.selected = false;
                }
            }
        }

        // Rectangles
        for (int i = rects.size() - 1; i >= 0; i--) {
            if (rects[i].active&&rects[i].tryGrab(mouseWorld)) {
                activeRect = &rects[i];
                for (auto& r : rects) r.selected = false;
                rects[i].selected = true;
                clickedSomething = true;
                break;
            }
        }

        for (int i = rects.size() - 1; i >= 0; i--) {
            if (rects[i].active&&rects[i].contains(mouseWorld)) {
                for (auto& r : rects) r.selected = false;
                rects[i].selected = true;
                clickedSomething = true;
                break;
            }
        }

        if (!clickedSomething) {
            for (auto& r : rects) r.selected = false;
        }
    }

    void handleRectangleKeys(sf::Keyboard::Key key) {
        int newGroup = -1;

        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9)
            newGroup = key - sf::Keyboard::Num0;

        if (newGroup != -1) {
            for (auto& r : rects)
                if (r.selected)
                    r.group = newGroup;
        }

        if (key == sf::Keyboard::Backspace || key == sf::Keyboard::Delete) {
            rects.erase(
                std::remove_if(rects.begin(), rects.end(),
                    [](const DraggableRect& r) { return r.selected; }),
                rects.end()
            );
            activeRect = nullptr;
        }
    }

    // ------------------------------------------------------------
    // Textbox Logic
    // ------------------------------------------------------------
    void handleTextboxTyping(sf::Keyboard::Key key) {
        if (!activeTextbox) return;

        if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
            activeTextbox->value.push_back(char('0' + (key - sf::Keyboard::Num0)));
            if (activeTextbox->value.length() >3) activeTextbox->value.erase(0);
        }

        if (key == sf::Keyboard::Backspace && !activeTextbox->value.empty()) {
            activeTextbox->value.pop_back();
        }

        if (key == sf::Keyboard::Enter && !activeTextbox->value.empty()) {
            readTextboxGrid();
            writeTextboxGrid();
        }
    }

    void handleArrows(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Left)
            cam.setOffset(cam.getOffset().x -8, cam.getOffset().y);
        if (key == sf::Keyboard::Right)
            cam.setOffset(cam.getOffset().x +8, cam.getOffset().y);
        if (key == sf::Keyboard::Down)
            cam.setOffset(cam.getOffset().x, cam.getOffset().y -8);
        if (key == sf::Keyboard::Up)
            cam.setOffset(cam.getOffset().x, cam.getOffset().y +8);
        if (key == sf::Keyboard::BackSlash)
            cam.setOffset(0,0);
    }

    void drawCameraOffsetOverlay(sf::RenderWindow& window) {
        sf::Text label;
        label.setFont(font);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::White);

        // Format: Offset: (x, y)
        label.setString(
            "Offset: (" +
            std::to_string(static_cast<int>(cam.getOffset().x/8)) + ", " +
            std::to_string(static_cast<int>(cam.getOffset().y/8)) + ") "+
            std::to_string(currentFrame) + " " + std::to_string(currentMode)
        );

        // Measure text so we can right‑align it
        sf::FloatRect bounds = label.getLocalBounds();

        float x = window.getSize().x - bounds.width - 12;
        float y = 10;

        label.setPosition(x, y);
        window.draw(label);
    }

};

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main() {
    sf::Texture texture;
    texture.loadFromFile("../__pic/editorBack.png");
    sf::Sprite sprite = sf::Sprite(texture);
    sprite.setScale(8.f, 8.f);
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Editor");

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    Editor* editor = new Editor(1600, 900, font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
                if (editor != nullptr){delete editor; editor = nullptr;}
            }

            sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);

            if (event.type == sf::Event::MouseButtonPressed)
                editor->handleMousePress(mouse, event.mouseButton.button);

            if (event.type == sf::Event::MouseButtonReleased)
                editor->handleMouseRelease(event.mouseButton.button);

            if (event.type == sf::Event::KeyPressed)
                editor->handleKeyPress(event.key.code);
        }

        editor->update(window);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        editor->draw(window);
        window.display();
    }

    if (editor != nullptr) delete editor;
    return 0;
}