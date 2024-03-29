#include <stdafx.h>
#include <Game/TileMap.h>


std::vector<Ennemie> TileMap::loadObjects(const std::string& objectset, sf::Vector2u tileSize, sf::Vector2u NspriteSize, unsigned int width, unsigned int height, 
                                            std::vector<ObjectsElements>& l_objects, std::vector<Ennemie>& cactus, std::vector<ObjectsElements>& l_checkpoints, 
                                            ObjectsElements& exit_sign, ObjectsElements& hard_sign,
											std::vector<MovableEnnemies>& mush, std::vector<MovableEnnemies>& disc)
{
	
	if (m_type != TmapType::monstobjects)
	{
		return std::vector<Ennemie> {}; 
	}

	// keep in memory tiles level 
    m_tilesize = tileSize;
	
	std::vector<Ennemie> l_ennemies; 
	// Find tile of interest 
	// populate the vertex array
    for (unsigned int i = 0; i < width; ++i)
        for (unsigned int j = 0; j < height; ++j)
        {
            unsigned int k = i + j * width;
            if (m_tiles[k] >= 100)
			{
				// non movable ennemies 
				int ntiles = m_tiles[k] - 100;
                unsigned int tx = (ntiles % NspriteSize.x) * tileSize.x;
                unsigned int ty = (ntiles / NspriteSize.x) * tileSize.y;
                sf::Vector2u coord(tx, ty);

                sf::Vector2f spaw ((i+1) * tileSize.x - tileSize.x/2.0f, (j +1)* tileSize.y - tileSize.y/2.0f);

                if (m_tiles[k] == 100)
                {
                    m_spawnPosition = spaw; 
                    continue;
                }
                if( (m_tiles[k] >= 400) && (m_tiles[k] < 500) )   // Disc Enemy
                {
                    int code = m_tiles[k] - 400;
                    int length = code % 10;
                    coord.x = 0;
                    coord.y = 544; // Disc sprite in Atlas
                    MovableEnnemies::E_Direction direction = static_cast<MovableEnnemies::E_Direction>(code / 10);
                    disc.push_back(MovableEnnemies(spaw, tileSize.x, tileSize.y, coord, direction, length));
                    continue; 
                }
                if ((m_tiles[k] >= 500) && (m_tiles[k] < 600))   // Mushroom Enemy
                {
                    int code = m_tiles[k] - 500;
                    int length = code % 10;
                    MovableEnnemies::E_Direction direction = static_cast<MovableEnnemies::E_Direction>(code / 10);
                    coord.x = 0;
                    coord.y = 544+32; // mushroom sprite in Atlas
                    mush.push_back(MovableEnnemies(spaw, tileSize.x, tileSize.y, coord, direction, length));
                    continue;
                }
				else if (m_tiles[k] < 300 && m_tiles[k] > 200)
                {
                    
                    static bool animated = m_tiles[k] > 270;
                    static bool weak_against_fire = false;
					// Die at touch 
                    l_ennemies.push_back(Ennemie(spaw, false, animated, coord, tileSize.x, tileSize.y, weak_against_fire));
				}
                else if (m_tiles[k] == 330)
                {
                    static bool animated = true;
                    static bool weak_against_fire = true;
                    sf::Vector2u cactus_size(3 * 32, 4 * 32);
                    // match base of cactus with tile lower ypoint 
                    spaw.y += tileSize.y/2.0f - cactus_size.y/2.0f ;
                    cactus.push_back(Ennemie(spaw, false, animated, coord, cactus_size.x, cactus_size.y, weak_against_fire));
                }
                else if (m_tiles[k] > 100  && m_tiles[k] < 104)
				{
					// classical elements
                    l_checkpoints.push_back(ObjectsElements(spaw, true, true, coord, tileSize.x, tileSize.y, m_tiles[k]));
				}                
                else if (m_tiles[k] == 106)
				{
					// classical elements
                    exit_sign = ObjectsElements(spaw, true, false, coord, tileSize.x, tileSize.y, m_tiles[k]);
				}   
                else if (m_tiles[k] == 109)
                {
                    // panneau
                    hard_sign = ObjectsElements(spaw, true, false, coord, tileSize.x, tileSize.y, m_tiles[k]);
                }
                else
				{
					// classical elements
                    l_objects.push_back(ObjectsElements(spaw, true, false, coord, tileSize.x, tileSize.y, m_tiles[k]));
				}
			}

		}

	return l_ennemies; 
}

// code from SFML tutorial page 
bool TileMap::load(const std::string& tileset, sf::Vector2u tileSize, unsigned int width, unsigned int height)
{

    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset))
        return false;
    
    // level given not correctly sized
    int surface = width * height;
    if (m_tiles.size() != surface)
        return false;

    // keep in memory tiles level 
    m_tilesize = tileSize; 
    m_plateforms = {};

    // Interpreted as Quad when drawn, init as points
    if (m_vertices.getPrimitiveType() != sf::Quads)
    {
        m_vertices.setPrimitiveType(sf::Quads);
    }
    

    // resize the vertex array to fit the level size
    unsigned int nsize = width * height * 4;
    if (static_cast<unsigned int>(m_vertices.getVertexCount())!= nsize)
    {
        m_vertices.resize(nsize);
    }
    

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < width; ++i)
        for (unsigned int j = 0; j < height; ++j)
        {
            // get the current tile number
            int index = i + j * width;
            int tileNumber = m_tiles[index];

            // find its position in the tileset texture
            int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
            int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

            // get a pointer to the current tile's quad
            sf::Vertex* quad = &m_vertices[index * int(4)];

            // define its 4 corners
            quad[0].position = sf::Vector2f((float)(i * tileSize.x),         (float)(j * tileSize.y));
            quad[1].position = sf::Vector2f((float)((i + 1) * tileSize.x),   (float)(j * tileSize.y));
            quad[2].position = sf::Vector2f((float)((i + 1) * tileSize.x),   (float)((j + 1) * tileSize.y));
            quad[3].position = sf::Vector2f((float)(i * tileSize.x),         (float)((j + 1) * tileSize.y));

            // define its 4 texture coordinates
            quad[0].texCoords = sf::Vector2f((float)(tu * tileSize.x),          (float)(tv * tileSize.y));
            quad[1].texCoords = sf::Vector2f((float)((tu + 1) * tileSize.x),    (float)(tv * tileSize.y));
            quad[2].texCoords = sf::Vector2f((float)((tu + 1) * tileSize.x),    (float)((tv + 1) * tileSize.y));
            quad[3].texCoords = sf::Vector2f((float)(tu * tileSize.x),          (float)((tv + 1) * tileSize.y));
			
			// create plateforms : bounding box associated 
			for (int k = 0; k < sizeof(m_nowalk) ; k++) 
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

}

// Clear data for next level 
void TileMap::PreNextLevel()
{
    // plateforms removing
    m_plateforms = {}; 
    
    // Vertex clear : doesn't deallocate the corresponding memory
    m_vertices.clear();
}


// is the position tile walkable 
bool TileMap::walkable_tile(sf::Vector2f& position)
{
    bool walkable = false;
    // error 
    if (m_tilesize.x <= 0 && m_tilesize.y <= 0) {
        return walkable;
    }
    // from position get tile 
    unsigned int ki = getTileIndex(position); 

    // 0 >= plateforme >= 9 
    // 9 < Walkable < 100  
    if (m_tiles[ki] > 9 && m_tiles[ki] < 121) {
        walkable = true;
    }
    return walkable;
}

// Get a vector according to elements of Tiles : 
//  0 : air, 1 : water, 2 : lava, 3 : void, -1 : non valid tile 
short int TileMap::ElementsTiles(sf::Vector2f& position)
{
    short int element = -1;
    // error 
    if (m_tilesize.x <= 0 && m_tilesize.y <= 0) {
        return element;
    }
    // from position get tile 
    unsigned int ki = getTileIndex(position);

    // 0 >= plateforme >= 9 
    // 9 < Walkable < 100  
    switch (m_tiles[ki])
    {
        case(10): element = 0;  break;   // Air
        case(11): element = 10; break;   // Water => Air
        case(12): element = 1;  break;   // Water
        case(13): element = 20; break;   // Lava => Air 
        case(14): element = 2;  break;   // Lava
        case(15): element = 3;  break;   // Void
        case(16): element = 0;  break;   // Air
        case(17): element = 0;  break;   // Air
        case(18): element = 0;  break;   // Air
        case(19): element = 0;  break;   // Air
        default: element = -1;  break;
    }

    return element;

}

std::vector<Plateform> TileMap::getPlateforms()
{
    return m_plateforms;
}

// read csv 
void TileMap::loadCsvTilemap(const std::string& levelcsv)
{
    // Read CSV files and assign to Tilemap attributes return std::vector
    m_tiles = ReadLevelFile(levelcsv);
}

// Read File and create board in memory usable
std::vector<int> TileMap::ReadLevelFile(const std::string& filename)
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
            while (sline >> n) {
                eboard.push_back(n); // add vector to board  
                sline >> c; 
            }
        }
    }
    return eboard;
}

const int TileMap::getTileIndex(const sf::Vector2f& position)
{
    // from position get tile 
    sf::Vector2f tilepos(std::ceil(position.x / m_tilesize.x), std::ceil(position.y / m_tilesize.y));
    sf::FloatRect tilemapsize = m_vertices.getBounds();
    // index 
    int index = int(tilepos.x) - 1 + (int(tilepos.y) - 1) * int(tilemapsize.width / m_tilesize.x);
    
    return index;
}


void TileMap::setTilemapType(bool isbackground)
{
    if (isbackground)
    {
        m_type = TmapType::backplateform;
    }
    else
    {
        m_type = TmapType::monstobjects;
    }
}
