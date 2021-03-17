#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <Game/Plateform.h>
#include <Game/Ennemie.h>

enum  TmapType {backplateform, monstobjects};

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    //TileMap(); 
    //~TileMap();
    // Read external files
    void loadCsvTilemap(const std::string& levelcsv);
    void setTilemapType(bool isbackground);
    // Load and assign to quad
    bool load(const std::string& tileset, sf::Vector2u tileSize, unsigned int width, unsigned int height);
    // Load and create ennemies sprite 
    std::vector<Ennemie> loadObjects(const std::string& objectset, sf::Vector2u tileSize, sf::Vector2u NspriteSize, unsigned int width, unsigned int height);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;


    bool walkable_tile(sf::Vector2f& position); 
    // Get 
	std::vector<Plateform> getPlateforms(); 
    sf::Vector2u getTileSize() const {return m_tilesize; }
    TmapType getTilemapType() const { return m_type; }

private:
    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
    std::vector<int> m_tiles;
    const int tiletype[11] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // infranchissable 
    const int m_nowalk[7] = {0, 2, 4, 6, 7, 8, 9};
    // create plateform objects 
    sf::Vector2u m_tilesize = { 0,0 };
	std::vector<Plateform> m_plateforms; 
    // tilemap type : background tileset or ennemies and objects
    TmapType m_type; 

protected:
    // Protected functions : Read CSV parseline 
    std::vector<int> ReadLevelFile(const std::string& filename);

};