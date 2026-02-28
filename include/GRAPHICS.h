//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

#include "SFML/Graphics.hpp"
#include "vector"
#include "array"
//contains the stuff necessary for rendering
namespace graphics {

    enum class layerID {
        SKY = -1,
        BACKDROP1 = 0,
        BACKDROP2 = 1,
        BACKDROP3 = 2,
        BACKDROP4 = 3,
        GROUND = 4,
        EFFECTS_BACK = 5,
        ENTITIES_BACK = 6,
        ENTITIES_FRONT = 7,
        EFFECTS_FRONT = 8,
        FOREGROUND = 9,
    };

    class ActiveSprite{
    public:
        bool active = false;
        int framesSinceLastChange = 0;
        int x = 0;
        int y = 0;
        int renderOrder = 0;
        sf::Sprite*sprite = nullptr;
        ActiveSprite(const sf::Texture &texture, bool startActive):sprite(new sf::Sprite(texture)) {

        }

        const sf::Texture* getTexture() {
            return sprite->getTexture();
        }

        void setTexture(const sf::Texture *texture) {
            sprite->setTexture(*texture);
        }

        void setTextureRect(const sf::IntRect& rect) {
            sprite->setTextureRect(rect);
        }

        ~ActiveSprite() {
            delete sprite;
        }
    };

    class Layer : public sf::Drawable , public sf::Transformable{
    public:

        sf::VertexArray spriteVA;
        std::vector<ActiveSprite*> sprites;
        layerID id;

        Layer() {
            sprites.reserve(1);
            spriteVA = sf::VertexArray(sf::Quads, 4);
        }
        Layer(int maxNumSprites) {
            sprites.reserve(maxNumSprites);
            spriteVA = sf::VertexArray(sf::Quads, maxNumSprites * 4);
        }
    private:
        sf::VertexArray m_vertices;
        sf::Texture m_texture;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            // apply the transform
            states.transform *= getTransform();
            // apply the tileset texture
            states.texture = &m_texture;
            // draw the vertex array
            target.draw(m_vertices, states);
        }

    };

    class AnimatedElement {
    public:
    protected:
    private:
    };


    class Backdrop {
    private:
        std::vector<std::array<double, 2>> layerPositions;
        std::vector<double> layerDepths;
        std::vector<int> layerFrames;

    protected:
    public:


    };



    const sf::Vector2u internalRes(320, 180); // the game's internal resolution
    sf::Vector2u windowSize(960, 540); // actual size of the game window

    sf::RenderTexture rt; // the texture that the game renders to
}
#endif