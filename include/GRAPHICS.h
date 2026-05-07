//
// Created by wangs on 2/26/2026.
//

#ifndef INCLUDE_GRAPHICS_H
#define INCLUDE_GRAPHICS_H

#include "SFML/Graphics.hpp"
#include "vector"
#include "array"
#include "random"
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

    class Camera {
    private:
        std::mt19937 rng{std::random_device{}()};
        std::uniform_real_distribution<double> dist{-1.f, 1.f};
    public:
        double x = 0;
        double y = 0;
        double actualX = 0;
        double actualY = 0;
        double shakeX = 0;
        double shakeY = 0;
        double shakeMag = 0;
        double maxShakeMag = 0;
        int shakeFrames = 0;
        int shakeTime = 0;
        double inv_time = 0;

        void applyShake(double mag, int frames) {
            shakeFrames = 0;
            shakeTime = frames;
            maxShakeMag = mag;
            shakeMag = maxShakeMag;
            if (shakeTime != 0) {
                inv_time = 1.0/ shakeTime;
            }
            else {
                inv_time = 0;
            }
        }

        void update(double inX, double inY) {
            x = actualX = inX;
            y = actualY = inY;
            if (shakeFrames < shakeTime ) {
                double mag = (shakeFrames * inv_time) - 1.0;
                shakeMag = maxShakeMag * mag * mag;

                x += dist(rng) * shakeMag;
                y += dist(rng) * shakeMag;

                shakeFrames ++;
            }
            else {
                shakeFrames = 0;
                shakeTime = 0;
                shakeMag = 0;
            }
        }
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
    std::array<std::array<int,40>,20> tilesets;
    class TileMap : public sf::Drawable , public sf::Transformable {
    private:
        sf::Texture m_tileset;
        sf::VertexArray m_vertices;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            // Apply the transform (position, rotation, scale)
            states.transform *= getTransform();
            // Apply the tileset texture
            states.texture = &m_tileset;
            // Draw the vertex array
            target.draw(m_vertices, states);
        }

    protected:
    public:
        int width;
        int height;
        bool load (const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height) {
            if (!m_tileset.loadFromFile(tileset)) return false;

            this->width = width * tileSize.x;
            this->height = height * tileSize.y;

            m_vertices.setPrimitiveType(sf::Quads);
            m_vertices.resize(width * height * 4);

            for (unsigned int i = 0; i < width; ++i)
                for (unsigned int j = 0; j < height; ++j) {
                    // Get the current tile number
                    int tileNumber = tiles[i + j * width];

                    // Find its position in the tileset texture
                    int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                    int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                    // Pointer to the current tile's quad
                    sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

                    // Define its 4 corners
                    quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                    quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                    quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                    quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                    // Define its 4 texture coordinates
                    quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                    quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                    quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
                    quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
                }
            return true;
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
        std::vector<sf::Vector2i> layerOffsets;

        std::vector<sf::Texture> layerTextures;
        std::vector<int> layerFrames;


    protected:
    public:
        void update(sf::Vector2f camPos) {
            for (auto pos: layerPositions) {
                pos[0] = -camPos.x / layerDepths[0];
            }
        }


    };


    const sf::Vector2i internalRes(300, 180); // the game's internal resolution
    const double scale = 4;

    inline sf::Vector2i windowSize(internalRes.x * scale, internalRes.y * scale); // actual size of the game window



    sf::RenderTexture rt; // the texture that the game renders to
}
#endif