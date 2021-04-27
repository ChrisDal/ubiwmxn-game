#pragma once
#include <vector> 

#include <Engine/Game.h>
#include <Game/MainCharacter.h>
#include <Game/Ennemie.h>
#include <Game/MovableEnnemies.h>
#include <Game/ObjectsElements.h>
#include <Game/DeadBody.h>
#include <Game/Tilemap.h>
#include <Game/Plateform.h>
#include <Game/EnumElements.h>
#include <Game/Vfx.h>

class GameDemo : public Game
{
public:
    GameDemo();
    ~GameDemo();

    void Update(float deltaTime) override;
    void Render(sf::RenderTarget& target) override;
    void RenderDebugMenu(sf::RenderTarget& target) override;
    void RenderEndMenu(sf::RenderTarget& target, sf::Vector2u& WINSIZE);

    inline bool IsBGMusicPlaying() 
    {
        return m_bgmusic.getStatus() == sf::SoundSource::Status::Playing ;
    }

    void setMusicVolume(float percentage) { m_bgmusic.setVolume(percentage); }
    float getMusicVolume() const { return m_bgmusic.getVolume(); }

private:
    sf::Font m_EndgameTextFont;
    sf::Text m_EndgameText;
    // UI Game
    sf::Font m_DeathsTextFont;
    sf::Sprite m_Tombstone;
    sf::Texture m_TextureTombstone;
    sf::Text m_DeathsText;
    sf::Sprite m_icoMusic; 
    sf::Texture m_TextureIcoMusic;
    sf::Texture m_TextureIcoNoMusic;
    sf::Sprite m_icoSFX;
    sf::Texture m_TextureIcoSound;
    sf::Texture m_TextureIcoNoSound;
    // Sound
    sf::SoundBuffer m_GameSoundBuffer;
    sf::SoundBuffer m_EndgameSoundBuffer;
    sf::Sound m_EndgameSound;
    sf::Sound m_GameSound;
    sf::Music m_bgmusic; 
    bool m_noMusic{ false };    // background music loaded 
    // Solid Elements
    TileMap m_Tilemap;
    MainCharacter* m_MainCharacter;
	std::vector<Plateform> m_plateform = {}; 
    // Affects life
    TileMap m_Elements; 
    sf::Texture m_TextureAtlas;
    sf::Texture m_TextureDead;
    ObjectsElements m_exit_sign;
    std::vector<MovableEnnemies> m_mushrooms = {};

    // struct gameelements m_GameElements;
    std::vector<Ennemie> m_cactus = {};
    std::vector<Ennemie> m_ennemies = {};
	std::vector<DeadBody> m_deadbodies = {};
	std::vector<ObjectsElements> m_objects = {};
	std::vector<ObjectsElements> m_checkpoints = {};

	
    // Game process
    bool m_IsFinished;
    bool m_Animation_AR{ true }; // Back and Forth for the mushroom 

};