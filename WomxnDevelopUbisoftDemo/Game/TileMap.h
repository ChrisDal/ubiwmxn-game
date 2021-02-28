#pragma once

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    //TileMap(); 
    //~TileMap();
    bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};