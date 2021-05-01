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
#include <Modules/MoveToAR.h>
#include <Modules/EatDeadbody.h>
#include <Game/UIGame.h>

class GameDemo : public Game
{

public:
    GameDemo();
    ~GameDemo();

    void Update(float deltaTime) override;
    void Render(sf::RenderTarget& target) override;
    void RenderDebugMenu(sf::RenderTarget& target) override;
    void RenderEndMenu(sf::RenderTarget& target, sf::Vector2u& WINSIZE);

    // Levels Handling 
    void LoadTileMaps(int level);
    void ClearDataLevel();
    bool NextLevel(bool firstlevel);
    


    inline bool IsBGMusicPlaying() 
    {
        return m_bgmusic.getStatus() == sf::SoundSource::Status::Playing ;
    }

    void setMusicVolume(float percentage) { m_bgmusic.setVolume(percentage); }
    float getMusicVolume() const { return m_bgmusic.getVolume(); }

private:

    const sf::Vector2u m_WINSIZE{ 1024, 768 };
    std::string m_AtlasName;
    const short unsigned int m_NBLEVELS = 2;
    sf::Font m_EndgameTextFont;
    sf::Text m_EndgameText;
    // UI Game
    sf::Font m_DeathsTextFont;
    sf::Sprite m_Tombstone;
    sf::Texture m_TextureTombstone;
    sf::Text m_DeathsText;

    // UI 
    std::unique_ptr<UIGame> m_gameUI;

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
    //std::unique_ptr<MainCharacter> m_MainCharacter;
	std::vector<Plateform> m_plateform = {}; 
    // Affects life
    TileMap m_Elements; 
    sf::Texture m_TextureAtlas;
    sf::Texture m_TextureDead;
    ObjectsElements m_exit_sign;
    std::vector<MovableEnnemies> m_mushrooms = {};
    std::vector<MovableEnnemies> m_discs = {};

    // Game Elements
    std::vector<Ennemie> m_cactus = {};
    std::vector<Ennemie> m_ennemies = {};
	std::vector<DeadBody> m_deadbodies = {};
	std::vector<ObjectsElements> m_objects = {};
	std::vector<ObjectsElements> m_checkpoints = {};

    // Levels
    short unsigned int m_level{ 0 };
	
    // Game process
    bool m_IsFinished;

    // behavior mushrooms 
    std::vector< std::shared_ptr<Routine> > m_Routines; 
    std::vector< std::shared_ptr<Routine> > m_RoutinesDiscs; 
    // routines interruption and set
    bool m_appliedRoutine{ false };
    bool m_toapplyRoutine{ false };

};