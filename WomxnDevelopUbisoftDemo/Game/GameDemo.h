#pragma once
#include <vector> 

#include <Engine/Game.h>
#include <Game/Door.h>
#include <Game/Exit.h>
#include <Game/MainCharacter.h>
#include <Game/Ennemie.h>
#include <Game/ObjectsElements.h>
#include <Game/DeadBody.h>
#include <Game/Tilemap.h>
#include <Game/Plateform.h>

class GameDemo : public Game
{
public:
    GameDemo();
    ~GameDemo();

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
    MainCharacter* m_MainCharacter;
	std::vector<Plateform> m_plateform = {}; 
    // Affects life
    TileMap m_Elements; 
    sf::Texture m_TextureAtlas;
    sf::Texture m_TextureDead;
    std::vector<Ennemie> m_ennemies = {};
	std::vector<DeadBody> m_deadbodies = {};
	std::vector<ObjectsElements> m_objects = {};
	
    // Game process
    bool m_IsFinished;

};