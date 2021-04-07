#pragma once
#include <vector> 

#include <Engine/Game.h>
#include <Game/MainCharacter.h>
#include <Game/Ennemie.h>
#include <Game/ObjectsElements.h>
#include <Game/DeadBody.h>
#include <Game/Tilemap.h>
#include <Game/Plateform.h>
#include <Game/EnumElements.h>

class GameDemo : public Game
{
public:
    GameDemo();
    ~GameDemo();

    void Update(float deltaTime) override;
    void Render(sf::RenderTarget& target) override;
    void RenderDebugMenu(sf::RenderTarget& target) override;
    void RenderEndMenu(sf::RenderTarget& target, sf::Vector2u& WINSIZE);

private:
    sf::Font m_EndgameTextFont;
    sf::Text m_EndgameText;
    // UI Game
    sf::Font m_DeathsTextFont;
    sf::Sprite m_Tombstone;
    sf::Texture m_TextureTombstone;
    sf::Text m_DeathsText;
    sf::SoundBuffer m_GameSoundBuffer;
    sf::SoundBuffer m_EndgameSoundBuffer;
    sf::Sound m_EndgameSound;
    sf::Sound m_GameSound;
    // Solid Elements
    TileMap m_Tilemap;
    MainCharacter* m_MainCharacter;
	std::vector<Plateform> m_plateform = {}; 
    ObjectsElements m_exit_sign; 
    // Affects life
    TileMap m_Elements; 
    sf::Texture m_TextureAtlas;
    sf::Texture m_TextureDead;
    std::vector<Ennemie> m_cactus = {};
    std::vector<Ennemie> m_ennemies = {};
	std::vector<DeadBody> m_deadbodies = {};
	std::vector<ObjectsElements> m_objects = {};
	std::vector<ObjectsElements> m_checkpoints = {};

	
    // Game process
    bool m_IsFinished;

};