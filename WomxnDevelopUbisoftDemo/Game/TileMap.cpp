#include <stdafx.h>
#include <Game/TileMap.h>

// code from SFML tutorial page 

bool TileMap::load(const std::string& tileset, sf::Vector2u tileSize, unsigned int width, unsigned int height)
{

    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset))
        return false;
    
    // level given not correctly sized
    if (m_tiles.size() != (width*height))
        return false;

    // keep in memory tiles level 
    m_tilesize = tileSize; 

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    unsigned int nsize = width * height * 4;
    m_vertices.resize(nsize);

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < width; ++i)
        for (unsigned int j = 0; j < height; ++j)
        {
            // get the current tile number
            int tileNumber = m_tiles[i + j * width];

            // find its position in the tileset texture
            int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
            int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

            // get a pointer to the current tile's quad
            sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

            // define its 4 corners
            quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

            // define its 4 texture coordinates
            quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);

            // create plateforms : bounding box associated 
            for (int k = 0; k < sizeof(m_nowalk); k++)
            {
                if (tileNumber == m_nowalk[k])
                    m_plateforms.push_back(Plateform(quad[0].position, quad[1].position, quad[2].position, quad[3].position));
            }


        }

        return true;

};


void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);

};

// is the position tile walkable 
bool TileMap::walkable_tile(sf::Vector2f& position)
{
    bool walkable = false;
    // error 
    if (m_tilesize.x <= 0 && m_tilesize.y <= 0) {
        return walkable;
    }
    // from position get tile 
    sf::Vector2f tilepos(std::ceil(position.x / m_tilesize.x), std::ceil(position.y / m_tilesize.y));
    sf::FloatRect tilemapsize = m_vertices.getBounds();
    int tiletype = m_tiles[int(tilepos.x) - 1 + (int(tilepos.y) - 1) * int(tilemapsize.width / m_tilesize.x)];

    // 0 >= plateforme >= 9 
    // 9 < Walkable < 100  
    if (tiletype > 9 && tiletype < 100) {
        walkable = true;
    }
    return walkable;
}

std::vector<Plateform> TileMap::getPlateforms()
{
    return m_plateforms;
}

// read csv 
void TileMap::readCsvTilemap(const std::string& levelcsv, int row, int col)
{
    // From name verify existence 
    // create a file object 
    std::ifstream pcsv;
    std::string filename = "D:\\DALILA\\GameDesign\\Ubisoft\\womxn-develop-at-ubisoft-demo\\WomxnDevelopUbisoftDemo\\"+ levelcsv;


    m_tiles = ReadLevelFile(filename);
 
}

// Read File and create board in memory usable
std::vector<int> TileMap::ReadLevelFile(std::string& filename)
{

    std::ifstream myfile(filename);
    std::vector<int> eboard;

    if (myfile) {
        std::string line;

        while (getline(myfile, line)) 
        {
            std::istringstream sline(line);
            int n; 
            char c; 
            while (sline >> n >> c && c == ',') {
                eboard.push_back(n); // add vector to board  
            }
        }
    }
    return eboard;
}

