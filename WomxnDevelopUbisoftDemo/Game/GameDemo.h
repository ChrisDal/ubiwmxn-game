#pragma once
#include <vector> 

#include <Engine/Game.h>
#include <Game/Door.h>
#include <Game/Exit.h>
#include <Game/MainCharacter.h>
#include <Game/Ennemie.h>
#include <Game/Tilemap.h>
#include <Game/Plateform.h>

class GameDemo : public Game
{
public:
    GameDemo();

    void Update(float deltaTime) override;
    void Render(sf::RenderTarget& target) override;
    void RenderDebugMenu(sf::RenderTarget& target) override;

private:
    sf::Font m_EndgameTextFont;
    sf::Text m_EndgameText;
    sf::SoundBuffer m_EndgameSoundBuffer;
    sf::Sound m_EndgameSound;
    // Solid Elements
    TileMap m_Tilemap;
    Door m_Door;
    Exit main_Door;
    MainCharacter m_MainCharacter;
	std::vector<Plateform> m_plateform = {}; 
    // Affects life
    TileMap m_Elements; 
    sf::Texture m_TextureAtlas;
    std::vector<Ennemie> m_ennemies = {};
	
    // Game process
    bool m_IsFinished;

};