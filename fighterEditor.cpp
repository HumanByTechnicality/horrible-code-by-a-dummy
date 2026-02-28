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
 *
 ***/


#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>

#include "fstream"

#include "include/COLLISION.h"
#include "include/UTIL.h"

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

    void drag(sf::Vector2f mousePos, float winW, float winH) {
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

        left   = std::max(0.f, left);
        top    = std::max(0.f, top);
        right  = std::min(winW, right);
        bottom = std::min(winH, bottom);
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
        rect.setFillColor(selected ? sf::Color(255,255,0,80) : sf::Color(0,255,0,80));
        rect.setOutlineColor(sf::Color::Green);
        rect.setOutlineThickness(1.f);
        window.draw(rect);

        float cx = (left + right) * 0.5f;
        float cy = (top + bottom) * 0.5f;

        sf::Vector2f screenCenter = cam.apply({cx, cy});

        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(group));
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::Blue);

        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
        label.setPosition(screenCenter);

        window.draw(label);
    }
    void draw2(sf::RenderWindow& window, const sf::Font& font, const Camera& cam) {
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

    void draw(sf::RenderWindow& window, const Camera& cam, int frame) {
        if (!loaded) return;

        sf::Vector2f screenPos = cam.apply(worldPos);
        sprite.setPosition(screenPos);
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
        }

        // Serialization placeholder
        std::string getMainData() const {
            return ""; // to be implemented
        }
    };

    // ------------------------------------------------------------
    // objects for holding move data during runtime
    // ------------------------------------------------------------
    struct frameData {
        std::vector<int> hitBoxSets;
        std::vector<int> hurtBoxSets;
        sf::Texture * texture;
        sf::IntRect imageRegion;
        double gravity;
        sf::Vector2f movement;
    };

    struct moveData{
        std::vector<BoxSet<collision::HitBox>> hitBoxes;
        std::vector<BoxSet<collision::HurtBox>> hurtBoxes;
        std::vector<std::vector<int>> hitBoxOrder;
        std::vector<std::vector<int>> hurtBoxOrder;
        std::vector<int> imageOrder;
        std::vector<sf::Vector2f> movement;
        std::vector<double> gravity;
        sf::Texture * texture;
        int xOffset, yOffset;
        int width, height;
        frameData getFrameData(int f) {
            sf::IntRect imageRegion = {xOffset, yOffset + height * imageOrder[f], width, height};
            return {hitBoxOrder[f],hurtBoxOrder[f],texture, imageRegion,gravity[f],movement[f]};
        }


    };

    // ------------------------------------------------------------
    // parser object
    // ------------------------------------------------------------
    class Parser {
        std::string filename;
        std::vector<std::string> fileLines;

    public:
        Parser(std::string filename) : filename(std::move(filename)) {
            std::ifstream vals = std::ifstream(filename);

            std::string line;

            if (vals.is_open()) {
                while (std::getline(vals, line)) {
                    std::transform(line.begin(), line .end(), line.begin(),
                    [](unsigned char c){ return std::tolower(c); });

                    fileLines.push_back(line);
                }
            }
        }

        /*moveData parseMoveData() {
            int parsetype = 0; // 0 = normal, 1 = hitbox, 2 = hurtbox, 3 = projectile

            std::vector<BoxSet<collision::HitBox> > hitBoxes;
            std::vector<BoxSet<collision::HurtBox> > hurtBoxes;

            std::vector<std::vector<int>> hitBoxOrder;
            std::vector<std::vector<int>> hurtBoxOrder;

            std::vector<double> gravity;
            std::vector<sf::Vector2f> movement;


            int width, height;
            int xoffset, yoffset;


            std::vector<int> imageOrder;
            sf::Texture *texture = nullptr;

            std::vector<std::array<int,4>> localBounds; // temp storage for pos: lines
            int currentID = -1;

            for (std::string line: fileLines) {
                if (line.empty()) continue;

                std::vector<std::string> flag = util::split(line, ':');
                if (flag.empty()) continue;

                // -------------------------------
                // NORMAL PARSING
                // -------------------------------
                if (parsetype == 0) {
                    if (flag[0] == "id") {
                        // ignored in editor
                    }

                    if (flag[0] == "file") {
                        texture = new sf::Texture();
                        texture->loadFromFile(flag[1]);
                    }

                    if (flag[0] == "framenum") {
                        // store if needed
                    }

                    if (flag[0] == "frameorder") {
                        std::vector<std::string> nums = util::split(flag[1], ',');
                        for (auto &n: nums) imageOrder.push_back(std::stoi(n));
                    }

                    if (flag[0] == "movement") {
                        // store if needed
                    }

                    if (flag[0] == "momentumcarry") {
                        // store if needed
                    }

                    if (flag[0] == "gravity") {
                        // store if needed
                    }

                    if (flag[0] == "hitbox_order") {
                        // handled later after hitboxes are parsed
                    }

                    if (flag[0] == "hurtbox_order") {
                        // handled later after hurtboxes are parsed
                    }

                    if (flag[0] == "hitbox_start") {
                        parsetype = 1;
                        currentID++;
                        localBounds.clear();
                    }

                    if (flag[0] == "hurtbox_start") {
                        parsetype = 2;
                        currentID++;
                        localBounds.clear();
                    }

                    if (flag[0] == "projectile_start") {
                        parsetype = 3;
                    }
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
                        std::vector<std::string> parts = util::split(flag[1], ',');
                        for (auto &p: parts) {
                            std::vector<std::string> xywh = util::split(p, 'x');
                            if (xywh.size() == 4) {
                                int x = std::stoi(xywh[0]);
                                int y = std::stoi(xywh[1]);
                                int w = std::stoi(xywh[2]);
                                int h = std::stoi(xywh[3]);
                                localBounds.emplace_back(std::array<int,4>{x, y, w, h});
                            }
                        }
                    }

                    if (flag[0] == "SAVE") {
                        collision::HitBox hb(localBounds, {});
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
                        std::vector<std::string> parts = util::split(flag[1], ',');
                        for (auto &p: parts) {
                            std::vector<std::string> xywh = util::split(p, 'x');
                            if (xywh.size() == 4) {
                                int x = std::stoi(xywh[0]);
                                int y = std::stoi(xywh[1]);
                                int w = std::stoi(xywh[2]);
                                int h = std::stoi(xywh[3]);
                                localBounds.emplace_back(std::array{x, y, w, h});
                            }
                        }
                    }

                    if (flag[0] == "SAVE") {
                        collision::HurtBox hb(localBounds, collision::hurtboxType::HURTBOX);
                        BoxSet<collision::HurtBox> set(hb, currentID);
                        hurtBoxes.push_back(set);
                    }
                }

                // -------------------------------
                // PROJECTILE PARSING (stub)
                // -------------------------------
                else if (parsetype == 3) {
                    if (flag[0] == "projectile_end") {
                        parsetype = 0;
                    }
                }
            }

            moveData md;
            md.hitBoxes = hitBoxes;
            md.hurtBoxes = hurtBoxes;
            md.imageOrder = imageOrder;
            md.texture = texture;

            return md;
        }*/

        moveData parseMoveData() {
            int parsetype = 0; // 0 = normal, 1 = hitbox, 2 = hurtbox, 3 = projectile

            std::vector<BoxSet<collision::HitBox> > hitBoxes;
            std::vector<BoxSet<collision::HurtBox> > hurtBoxes;

            std::vector<std::vector<int> > hitBoxOrder;
            std::vector<std::vector<int> > hurtBoxOrder;

            std::vector<double> gravity;
            std::vector<sf::Vector2f> movement;

            int width = 0, height = 0;
            int xoffset = 0, yoffset = 0;

            std::vector<int> imageOrder;
            sf::Texture *texture = nullptr;

            std::vector<std::array<int, 4> > localBounds; // temp for pos: lines
            int currentID = -1;

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
                    }

                    if (flag[0] == "file") {
                        texture = new sf::Texture();
                        texture->loadFromFile(flag[1]);
                    }

                    if (flag[0] == "framenum") {
                        // can be used later if needed
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

                    if (flag[0] == "movement") {
                        // example: "movement:0x0,0"
                        // you can expand this later into per-frame movement
                    }

                    if (flag[0] == "momentumcarry") {
                        // store if needed
                    }

                    if (flag[0] == "gravity") {
                        // example: "gravity:19x1"
                        // you can expand this later into per-frame gravity
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
                        // example: "hitboxorder:-1x-1"
                        // left unexpanded for now; can be wired into hitBoxOrder later
                    }

                    if (flag[0] == "hurtboxorder") {
                        // example: "hurtboxorder:30x0"
                        // left unexpanded for now; can be wired into hurtBoxOrder later
                    }

                    if (flag[0] == "HITBOX_START") {
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

                    if (flag[0] == "HURTBOX_START") {
                        parsetype = 2;
                        currentID++;
                        localBounds.clear();
                    }

                    if (flag[0] == "PROJECTILE_START") {
                        parsetype = 3;
                    }
                }

                // -------------------------------
                // HITBOX PARSING
                // -------------------------------
                else if (parsetype == 1) {
                    if (flag[0] == "HITBOX_END") {
                        parsetype = 0;
                        continue;
                    }

                    if (flag[0] == "pos") {
                        // pos:-13x45,26x45
                        auto parts = util::split(flag[1], ',');
                        for (auto &p: parts) {
                            auto xy = util::split(p, 'x');
                            if (xy.size() == 2) {
                                int x = std::stoi(xy[0]);
                                int y = std::stoi(xy[1]);
                                // width/height come from framesize
                                localBounds.emplace_back(std::array<int, 4>{x, y, width, height});
                            }
                        }
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

                    if (flag[0].rfind("SAVE", 0) == 0) {
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
                    if (flag[0] == "HURTBOX_END") {
                        parsetype = 0;
                        continue;
                    }

                    if (flag[0] == "pos") {
                        // pos:-13x45,26x45
                        auto parts = util::split(flag[1], ',');
                        for (auto &p: parts) {
                            auto xy = util::split(p, 'x');
                            if (xy.size() == 2) {
                                int x = std::stoi(xy[0]);
                                int y = std::stoi(xy[1]);
                                localBounds.emplace_back(std::array<int, 4>{x, y, width, height});
                            }
                        }
                    }

                    if (flag[0] == "type") {
                        // type:hurt  (you can map this to different hurtboxType later)
                    }

                    if (flag[0].rfind("SAVE", 0) == 0) {
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

            moveData md;
            md.hitBoxes = std::move(hitBoxes);
            md.hurtBoxes = std::move(hurtBoxes);
            md.hitBoxOrder = std::move(hitBoxOrder);
            md.hurtBoxOrder = std::move(hurtBoxOrder);
            md.imageOrder = std::move(imageOrder);
            md.movement = std::move(movement);
            md.gravity = std::move(gravity);
            md.texture = texture;
            md.xOffset = xoffset;
            md.yOffset = yoffset;
            md.width = width;
            md.height = height;

            return md;
        }

    };

    // ------------------------------------------------------------
    // writer object
    // ------------------------------------------------------------
    class Writer{};
}


// ------------------------------------------------------------
// Editor Class
// ------------------------------------------------------------
class Editor {

public:
    Editor(float winW, float winH, const sf::Font& font)
        : winW(winW), winH(winH), font(font)
    {
        initTextboxGrid();
        timelineY = grid[0][0].bounds.top - 60.f;
        initModeButtons();

        if (!fighterSprite.loadFromFile("../__pic/idle.png")) {
            std::cerr << "Failed to load fighter sprite\n";
        }

        // Set initial world-space position of the fighter
        fighterSprite.setWorldPos(200.f, 300.f);

        // Optionally center the sprite on its origin
        if (fighterSprite.loaded) {
            sf::FloatRect b = fighterSprite.sprite.getLocalBounds();
            fighterSprite.sprite.setOrigin(b.width * 0.5f, b.height * 0.5f);
            fighterSprite.sprite.setScale(8,8);
        }

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
    }

    void update(sf::RenderWindow& window) {
        sf::Vector2f mausu = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        sf::Vector2f m = cam.toWorld(mausu);
        if (activeRect) {
            activeRect->drag(sf::Vector2f(m.x, m.y), winW, winH);
        }
        if (draggingTimeline) {
            float mx = static_cast<float>(m.x);

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

    }

    void draw(sf::RenderWindow& window) {
        fighterSprite.draw(window, cam,0);


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
    brainHurt::Parser p =  brainHurt::Parser("idle");
    brainHurt::moveData movDat = p.parseMoveData();
    FighterSprite fighterSprite;

    Camera cam;

    float winW, winH;
    const sf::Font& font;

    std::vector<DraggableRect> rects;
    DraggableRect* activeRect = nullptr;

    std::vector<std::vector<Textbox>> grid;
    Textbox* activeTextbox = nullptr;

    mode currentMode = hitbox;
    std::vector<Button> modeButtons;

    int currentFrame = 0;
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

        grid.resize(rows, std::vector<Textbox>(cols, Textbox(0,0,0,0, "00")));

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                float x = startX + c * (tbW + pad);
                float y = startY + r * (tbH + pad);
                grid[r][c] = Textbox(x, y, tbW, tbH,"00");
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

        for (auto& r : rects) r.selected = false;
        rects.back().selected = true;
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
        sf::Vector2f mouse = cam.toWorld(mouseLocal);

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
            if (rects[i].tryGrab(mouse)) {
                activeRect = &rects[i];
                for (auto& r : rects) r.selected = false;
                rects[i].selected = true;
                clickedSomething = true;
                break;
            }
        }

        for (int i = rects.size() - 1; i >= 0; i--) {
            if (rects[i].contains(mouse)) {
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
            std::to_string(static_cast<int>(cam.getOffset().y/8)) + ")"
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

    Editor editor(1600, 900, font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);

            if (event.type == sf::Event::MouseButtonPressed)
                editor.handleMousePress(mouse, event.mouseButton.button);

            if (event.type == sf::Event::MouseButtonReleased)
                editor.handleMouseRelease(event.mouseButton.button);

            if (event.type == sf::Event::KeyPressed)
                editor.handleKeyPress(event.key.code);
        }

        editor.update(window);

        window.clear(sf::Color::Black);
        window.draw(sprite);
        editor.draw(window);
        window.display();
    }

    return 0;
}