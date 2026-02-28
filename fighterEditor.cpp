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

/*#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "functional"
#include "include/ANIMATION.h"
#include "include/COLLISION.h"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

#include "iostream"

static float distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

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
        txt.setCharacterSize(18);
        txt.setFillColor(sf::Color::Black);
        txt.setPosition(bounds.left + 5, bounds.top + 3);
        window.draw(txt);
    }
};


class DraggableRect {
public:
    float left, top, right, bottom;
    int grabbedCorner = -1;
    float grabRadius = 10.f;
    int group = 0;

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

    bool selected = false;

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        float width = right - left;
        float height = bottom - top;

        sf::RectangleShape rect;
        rect.setPosition(left, top);
        rect.setSize({width, height});

        if (selected)
            rect.setFillColor(sf::Color(255, 255, 0, 60));
        else
            rect.setFillColor(sf::Color(0, 255, 0, 60));

        rect.setOutlineColor(sf::Color::Green);
        rect.setOutlineThickness(1.f);
        window.draw(rect);

        auto c = corners();
        for (int i = 0; i < 4; i++) {
            sf::CircleShape h(grabRadius);
            h.setOrigin(grabRadius, grabRadius);
            h.setPosition(c[i]);

            if (i == grabbedCorner)
                h.setFillColor(sf::Color::Yellow);
            else
                h.setFillColor(sf::Color::Red);

            window.draw(h);
        }

        float cx = (left + right) * 0.5f;
        float cy = (top + bottom) * 0.5f;

        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(group));
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::Blue);

        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
        label.setPosition(cx, cy);

        window.draw(label);
    }

    bool contains(sf::Vector2f p) const {
        return p.x >= left && p.x <= right &&
               p.y >= top  && p.y <= bottom;
    }
};

namespace bottomBar {
    const int cols = 46;
    const int rows = 5;
    const float tbW = 30.f;
    const float tbH = 28.f;
    const float startX = 20.f;
    const float startY = 20.f;
    const float pad = 4.f;
}

enum mode {
    hitbox,
    hurtbox,
    frame,
    movement,
    MODE_COUNT
};

class editor {
private:
public:
    mode state;



};
int main() {
    sf::RenderWindow window(sf::VideoMode(1600, 900), "Multiple Draggable Rectangles");

    std::vector<DraggableRect> rects;
    DraggableRect* activeRect = nullptr;

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    // --- NEW: 20×4 GRID OF TEXTBOXES ---

    std::vector<std::vector<Textbox>> grid(bottomBar::rows, std::vector<Textbox>(bottomBar::cols, Textbox(0,0,0,0)));

    for (int r = 0; r < bottomBar::rows; r++) {
        for (int c = 0; c < bottomBar::cols; c++) {
            float x = bottomBar::startX + c * (bottomBar::tbW + bottomBar::pad);
            float y = bottomBar::startY + r * (bottomBar::tbH + bottomBar::pad);
            grid[r][c] = Textbox(x, y, bottomBar::tbW, bottomBar::tbH, "00");
        }
    }
    // -----------------------------------

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);

            // RIGHT CLICK: create new rectangle
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Right) {

                float w = 120.f;
                float h = 80.f;

                rects.emplace_back(mouse.x, mouse.y, mouse.x + w, mouse.y + h);

                for (auto& r : rects) r.selected = false;
                rects.back().selected = true;
            }

            // LEFT CLICK: selection + corner grabbing + textbox selection
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                bool clickedSomething = false;

                // --- NEW: check grid textboxes ---
                for (int r = 0; r < bottomBar::rows; r++) {
                    for (int c = 0; c < bottomBar::cols; c++) {
                        if (grid[r][c].contains(mouse)) {
                            grid[r][c].selected = true;
                            clickedSomething = true;
                        } else {
                            grid[r][c].selected = false;
                        }
                    }
                }
                // --------------------------------

                // Check rectangles from topmost to bottom
                for (int i = rects.size() - 1; i >= 0; i--) {

                    if (rects[i].tryGrab(mouse)) {
                        activeRect = &rects[i];

                        for (auto& r : rects) r.selected = false;
                        rects[i].selected = true;

                        clickedSomething = true;
                        break;
                    }

                    if (rects[i].contains(mouse)) {
                        for (auto& r : rects) r.selected = false;

                        if (!rects[i].selected) {
                            rects[i].selected = true;
                            clickedSomething = true;
                            break;
                        }
                        rects[i].selected = false;
                        clickedSomething = false;
                        break;
                    }
                }

                if (!clickedSomething) {
                    for (auto& r : rects) r.selected = false;
                }
            }

            if (event.type == sf::Event::MouseButtonReleased &&
                event.mouseButton.button == sf::Mouse::Left) {

                if (activeRect) activeRect->release();
                activeRect = nullptr;
            }

            if (event.type == sf::Event::KeyPressed) {

                int newGroup = -1;

                if (event.key.code == sf::Keyboard::Num0) newGroup = 0;
                if (event.key.code == sf::Keyboard::Num1) newGroup = 1;
                if (event.key.code == sf::Keyboard::Num2) newGroup = 2;
                if (event.key.code == sf::Keyboard::Num3) newGroup = 3;
                if (event.key.code == sf::Keyboard::Num4) newGroup = 4;
                if (event.key.code == sf::Keyboard::Num5) newGroup = 5;
                if (event.key.code == sf::Keyboard::Num6) newGroup = 6;
                if (event.key.code == sf::Keyboard::Num7) newGroup = 7;
                if (event.key.code == sf::Keyboard::Num8) newGroup = 8;
                if (event.key.code == sf::Keyboard::Num9) newGroup = 9;

                // --- NEW: typing into grid textboxes ---
                for (int r = 0; r < bottomBar::rows; r++) {
                    for (int c = 0; c < bottomBar::cols; c++) {
                        if (grid[r][c].selected) {
                            if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
                                grid[r][c].value.push_back(char('0' + (event.key.code - sf::Keyboard::Num0)));
                                if (grid[r][c].value.size() == 3) grid[r][c].value.erase(0,1);
                            }
                            if (event.key.code == sf::Keyboard::Backspace && !grid[r][c].value.empty()) {
                                grid[r][c].value.pop_back();
                            }
                        }
                    }
                }
                // --------------------------------------

                if (newGroup != -1) {
                    for (auto& r : rects) {
                        if (r.selected) {
                            r.group = newGroup;
                        }
                    }
                }

                if (event.key.code == sf::Keyboard::Backspace ||
                    event.key.code == sf::Keyboard::Delete) {

                    rects.erase(
                        std::remove_if(rects.begin(), rects.end(),
                            [](const DraggableRect& r) { return r.selected; }),
                        rects.end()
                    );

                    activeRect = nullptr;
                }
            }
        }

        if (activeRect) {
            sf::Vector2i m = sf::Mouse::getPosition(window);
            activeRect->drag(sf::Vector2f(m.x, m.y), window.getSize().x, window.getSize().y);
        }

        window.clear(sf::Color::Black);

        for (auto& r : rects) r.draw(window, font);

        // --- NEW: draw textbox grid ---
        for (int r = 0; r < bottomBar::rows; r++)
            for (int c = 0; c < bottomBar::cols; c++)
                grid[r][c].draw(window, font);
        // ------------------------------

        window.display();
    }

    return 0;
}*/
#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>

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

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        float width = right - left;
        float height = bottom - top;

        sf::RectangleShape rect;
        rect.setPosition(left, top);
        rect.setSize({width, height});
        rect.setFillColor(selected ? sf::Color(255,255,0,80) : sf::Color(0,255,0,80));
        rect.setOutlineColor(sf::Color::Green);
        rect.setOutlineThickness(1.f);
        window.draw(rect);

        auto c = corners();
        for (int i = 0; i < 4; i++) {
            sf::CircleShape h(grabRadius);
            h.setOrigin(grabRadius, grabRadius);
            h.setPosition(c[i]);
            h.setFillColor(i == grabbedCorner ? sf::Color::Yellow : sf::Color::Red);
            window.draw(h);
        }

        float cx = (left + right) * 0.5f;
        float cy = (top + bottom) * 0.5f;

        sf::Text label;
        label.setFont(font);
        label.setString(std::to_string(group));
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::Blue);

        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
        label.setPosition(cx, cy);

        window.draw(label);
    }
};

// ------------------------------------------------------------
// Editor Class
// ------------------------------------------------------------
class Editor {
public:
    Editor(float winW, float winH, const sf::Font& font)
        : winW(winW), winH(winH), font(font)
    {
        initTextboxGrid();
    }

    void handleMousePress(sf::Vector2f mouse, sf::Mouse::Button button) {
        if (button == sf::Mouse::Right) {
            createRect(mouse);
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
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        handleTextboxTyping(key);
        handleRectangleKeys(key);
    }

    void update(sf::RenderWindow& window) {
        if (activeRect) {
            sf::Vector2i m = sf::Mouse::getPosition(window);
            activeRect->drag(sf::Vector2f(m.x, m.y), winW, winH);
        }
    }

    void draw(sf::RenderWindow& window) {
        for (auto& r : rects)
            r.draw(window, font);

        for (auto& row : grid)
            for (auto& tb : row)
                tb.draw(window, font);
    }

private:
    float winW, winH;
    const sf::Font& font;

    std::vector<DraggableRect> rects;
    DraggableRect* activeRect = nullptr;

    std::vector<std::vector<Textbox>> grid;
    Textbox* activeTextbox = nullptr;

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
    // Rectangle Logic
    // ------------------------------------------------------------
    void createRect(sf::Vector2f mouse) {
        float w = 120.f;
        float h = 80.f;

        rects.emplace_back(mouse.x, mouse.y, mouse.x + w, mouse.y + h);

        for (auto& r : rects) r.selected = false;
        rects.back().selected = true;
    }

    void handleLeftClick(sf::Vector2f mouse) {
        bool clickedSomething = false;

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
};

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main() {
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
        editor.draw(window);
        window.display();
    }

    return 0;
}