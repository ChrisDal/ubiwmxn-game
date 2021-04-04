#pragma once

#include <vector>
#include <sstream>
#include <fstream>
#include <Game/Plateform.h>
#include <Game/Ennemie.h>
#include <Game/ObjectsElements.h>

enum class TmapType {backplateform, monstobjects};

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
    // Read external files
    void loadCsvTilemap(const std::string& levelcsv);
    void setTilemapType(bool isbackground);
    // Load and assign to quad
    bool load(const std::string& tileset, sf::Vector2u tileSize, unsigned int width, unsigned int height);
    // Load and create ennemies sprite 
    std::vector<Ennemie> loadObjects(const std::string& objectset, sf::Vector2u tileSize, sf::Vector2u NspriteSize, 
                                    unsigned int width, unsigned int height, std::vector<ObjectsElements>& l_objects, 
                                    std::vector<Ennemie>& cactus, std::vector<ObjectsElements>& m_checkpoints,
                                    ObjectsElements& exit_sign);
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    // Type of tiles 
    bool walkable_tile(sf::Vector2f& position); 
    short int ElementsTiles(sf::Vector2f& position);
    // Get 
	std::vector<Plateform> getPlateforms(); 
    sf::Vector2u getTileSize() const {return m_tilesize; }
    TmapType getTilemapType() const { return m_type; }
    sf::Vector2f getMainCharacterSpawnPosition() const { return m_spawnPosition; }

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
    // spawn position 
    sf::Vector2f m_spawnPosition{ 0.0f, 0.0f };
    // tilemap type : background tileset or ennemies and objects
    TmapType m_type; 

protected:
    // Protected functions : Read CSV parseline 
    std::vector<int> ReadLevelFile(const std::string& filename);

};