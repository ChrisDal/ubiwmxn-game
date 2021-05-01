#include <iostream>
#include "stdafx.h"
#include "GameDemo.h"


GameDemo::GameDemo()
    : Game{ "9Lives" }
    , m_IsFinished{ false }
{
    m_EndgameTextFont.loadFromFile("Assets\\arial.ttf");

    m_EndgameText.setFont(m_EndgameTextFont);
    m_EndgameText.setPosition(500, 400);
    m_EndgameText.setString("!!! WIN !!!");
    m_EndgameText.setCharacterSize(24);
    m_EndgameText.setFillColor(sf::Color::Red);

    // Sounds
    m_EndgameSoundBuffer.loadFromFile("Assets\\Sounds\\Win_sound.wav");
    m_EndgameSound.setBuffer(m_EndgameSoundBuffer);
    // Music
    if (!m_bgmusic.openFromFile("Assets\\Sounds\\music\\7-Dark Fantasy Studio-Tribe.wav"))
    {
        if (!m_bgmusic.openFromFile("Assets\\Sounds\\music\\2-Dark Fantasy Studio-Angry bull.wav"))
            m_noMusic = true;
    }
    m_bgmusic.setLoop(true);


    // Future Game UI 
    m_DeathsTextFont.loadFromFile("Assets\\calibrib.ttf");
    m_TextureTombstone.loadFromFile(".\\Assets\\tombstone.png");
    m_Tombstone.setTexture(m_TextureTombstone);

    // Death and sound
    m_gameUI = std::make_unique<UIGame>(".\\Assets\\icons\\musicOn_small.png",
                                        ".\\Assets\\icons\\musicOff_small.png",
                                        ".\\Assets\\icons\\audioOn_small.png",
                                        ".\\Assets\\icons\\audioOff_small.png");

    // TEXTURES LOADING 
    // Get Main spritesheet
    m_AtlasName = "Assets\\ennemies_empty3.png";
    m_TextureAtlas.loadFromFile(m_AtlasName);
    // assign 
    Ennemie::SetTextureAtlas(&m_TextureAtlas);
    ObjectsElements::SetTextureAtlas(&m_TextureAtlas);
    MovableEnnemies::SetTextureAtlas(&m_TextureAtlas);
    VFX::SetTextureAtlas(&m_TextureAtlas);

    // Dead Body Texture
    const std::string dead_texture_name = "Assets\\daedcat_addon_sprite.png";
    m_TextureDead.loadFromFile(dead_texture_name);
    DeadBody::SetTextureAtlas(&m_TextureDead);

    // TILEMAPS
    bool firstlevel = not (m_level > 0); 
    NextLevel(firstlevel);

    // SetUp Ennemies Path 
    for (MovableEnnemies& mush : m_mushrooms)
    {
        // A/R
        sf::Vector2f TargetPoint{ mush.GetCenter() };
        TargetPoint.x += 32.0f * 6.0f;
        // Mushroom config
        mush.SetSFXVolume(50.0f);
        mush.configurePatrol(mush.GetCenter(), TargetPoint);
        // Routine config
        std::shared_ptr<MoveToAR> rout = std::make_shared<MoveToAR>(mush.GetCenter(), TargetPoint, &mush); 
        m_Routines.push_back(rout);
        
    }     
	
	// SetUp Ennemies Path 
    for (MovableEnnemies& disc : m_discs)
    {
        // A/R
        sf::Vector2f TargetPoint{ disc.GetCenter() };
        TargetPoint.y += 32.0f * 5.0f;
        // Disc config
        disc.SetSFXVolume(50.0f);
        disc.configurePatrol(disc.GetCenter(), TargetPoint);
        disc.setAnimatedByRot(true); 
        // Routine config
        std::shared_ptr<MoveToAR> rout = std::make_shared<MoveToAR>(disc.GetCenter(), TargetPoint, &disc); 
        m_RoutinesDiscs.push_back(rout);
        
    } 

    // Set sound volume 
    m_MainCharacter->SetSFXVolume(50.0f); 
    this->setMusicVolume(m_MainCharacter->GetSFXVolume()/10.0f);
	
}


// Clear data level
void GameDemo::ClearDataLevel()
{
    // unload Data : call destructors; 
    m_plateform.clear();
    m_mushrooms.clear();
    m_discs.clear();
    m_cactus.clear();
    m_ennemies.clear();
    m_objects.clear();
    m_checkpoints.clear();
}


// Clear and prepare for next level
bool GameDemo::NextLevel(bool firstlevel)
{
    
    if (not firstlevel)
    {
        // Unload data for TileMaps
        m_Tilemap.PreNextLevel();
        m_Elements.PreNextLevel();
    }

    // Remove Game Data
    ClearDataLevel(); 

    // Load Tilemap : Backgrounds (plateform) and Elements (ennemies, position)
    LoadTileMaps(m_level);

    // LOAD elements for Game  
    m_ennemies = m_Elements.loadObjects(m_AtlasName, 
                                        sf::Vector2u(32, 32), sf::Vector2u(10, 50),
                                        static_cast<unsigned int>(m_WINSIZE.x / 32.0f),
                                        static_cast<unsigned int>(m_WINSIZE.y / 32.0f),
                                        m_objects, m_cactus, 
                                        m_checkpoints, m_exit_sign, 
                                        m_mushrooms, m_discs);

    // Main character 
    if (firstlevel)
    {
        m_MainCharacter = new MainCharacter(m_WINSIZE, m_Elements.getMainCharacterSpawnPosition());
    }
    else
    {
        m_MainCharacter->MoveToNextLevel(m_Elements.getMainCharacterSpawnPosition());
    }

    m_level++;

    return true;
}

/// Load TileSet and Elements maps from CSV
void GameDemo::LoadTileMaps(int level)
{
    char tilename[45];
    int cx;
    cx = snprintf(tilename, 45, "Assets\\levels\\Level-TMPF-Niv-%02d-tiles.csv", level);
    // background tilemap
    m_Tilemap.loadCsvTilemap(std::string(tilename));
    m_Tilemap.load("Assets\\tileset_32x32.png", sf::Vector2u(32, 32), 32, 24);
    // define found plateform 
    m_plateform = m_Tilemap.getPlateforms();

    char elemname[45];
    cx = snprintf(elemname, 45, "Assets\\levels\\Level-TMPF-Niv-%02d-elem.csv", level);
    m_Elements.loadCsvTilemap(std::string(elemname));
    m_Elements.setTilemapType(false);

}

GameDemo::~GameDemo()
{
    delete m_MainCharacter;
}

void GameDemo::Update(float deltaTime)
{

    if (not m_noMusic)
    {
        if (not IsBGMusicPlaying())
        {
            // if pause or stopped => play
            m_bgmusic.play(); 
        }
    }

    m_MainCharacter->Update(deltaTime, m_plateform, m_Tilemap,
                            m_ennemies, m_cactus, m_mushrooms, m_discs);

    // Handling death of cactus
    std::vector<Ennemie>::iterator it_cactus = m_cactus.begin();
    while (it_cactus != m_cactus.end())
    {
        if (it_cactus->GetIsDead())
        {
            // erase returns following element
            it_cactus = m_cactus.erase(it_cactus);
        }
        else
        {
            ++it_cactus;
        }
    }
    // Update Living cactus
    for (int i = 0; i < m_cactus.size(); i++)
    {
        m_cactus[i].Update(deltaTime, m_MainCharacter);
    }

    // Basic Ennemies 
    for (int i = 0; i < m_ennemies.size(); i++)
    {
        m_ennemies[i].Update(deltaTime, m_MainCharacter);
    }


    // Check the checkpoints
    for (int k = 0; k < m_checkpoints.size(); k++)
    {
        if (m_checkpoints[k].IsColliding(*m_MainCharacter))
        {
            m_MainCharacter->setRespawnPosition(m_checkpoints[k].GetCenter());
            m_checkpoints[k].Update(deltaTime, true);
        }
        else
        {
            m_checkpoints[k].Update(deltaTime, false);
        }
       
    }
	
   // Check the discs 
    for (int i = 0; i < m_discs.size(); i++)
    {
		// MoveToAR routine 
        m_discs[i].Update(deltaTime);
        m_RoutinesDiscs[i]->Act(&m_discs[i], deltaTime);
	}


    m_deadbodies = m_MainCharacter->getDeadBodies();

    // Check the mushrooms 
    for (int i = 0; i < m_mushrooms.size(); i++)
    {
        // MoveToAR routine 
        m_mushrooms[i].Update(deltaTime);

        // routines handling 
        if (m_mushrooms[i].IsColliding(*m_MainCharacter))
        {
            m_toapplyRoutine = true; 
        }

        if ((not m_appliedRoutine) and m_toapplyRoutine)
        {
            sf::Vector2f TargetPoint = { m_MainCharacter->GetCenter().x, m_mushrooms[i].GetCenter().y };
            m_Routines.erase(m_Routines.begin() + i);
            m_Routines.insert(m_Routines.begin() + i, std::make_shared<EatDeadbody>(&m_mushrooms[i], TargetPoint));

            // Play eating Sound 
            m_mushrooms[i].EatPerso(); 

            // Routine applied : nothing else to be done : wait until finished
            m_toapplyRoutine = false;
            m_appliedRoutine = true;
        }

        // MoveTO AR doesn't succeed = infinite, others routine yes
        // EatDeadBody is the only other Routine 
        if (m_Routines[i]->isSuccessfull())
        {
            // last deadbody == killed by Ennemies
            int k = m_deadbodies.size() - 1;
            m_MainCharacter->RemoveDeadbody(k);

            // stop eating perso
            m_mushrooms[i].StopEatingPerso();

            auto rout = std::make_shared<MoveToAR>(m_mushrooms[i].getPatrol()[0], m_mushrooms[i].getPatrol()[1], &m_mushrooms[i]);
            m_Routines.erase(m_Routines.begin() + i);
            m_Routines.insert(m_Routines.begin() + i, rout);

            // handling application of Routines
            // Patrol is the default routine when applied we can interrupt any time
            m_toapplyRoutine = false;
            m_appliedRoutine = false;
        }

        m_Routines[i]->Act(&m_mushrooms[i], deltaTime);

    }


    if (!m_IsFinished)
    {
        if (m_exit_sign.Contains(m_MainCharacter->GetCenter()))
        {
            m_EndgameSound.play();

            if (m_level < m_NBLEVELS)
            {
                // End of the level
                bool isfirst = (m_level == 0); 
                NextLevel(isfirst);
                m_bgmusic.pause();
            }
            else
            {
                // End of the game 
                m_MainCharacter->StartEndGame();
                m_exit_sign.StartEndGame();
                m_IsFinished = true;
            }
            
            
        }
    }
}

void GameDemo::Render(sf::RenderTarget& target)
{
    target.clear(sf::Color(0, 0, 0));
    target.draw(m_Tilemap);
	
	// Ennemies 
	for ( const auto& enm : m_ennemies) 
	{
		target.draw(enm); 
	}		
    
    for ( const auto& cac : m_cactus) 
	{
		target.draw(cac); 
	}	
    
    // Elements 
	for ( const auto& elm : m_objects)
	{
		target.draw(elm);
	}
	
    // Checkpoints 
	for ( const auto& ckp : m_checkpoints)
	{
		target.draw(ckp);
	}
	
    // Exit and MC
    target.draw(m_exit_sign);
	
    // Mushrooms 
    for (MovableEnnemies& mvenm : m_mushrooms)
    {
        target.draw(mvenm);
    }
	
	// discs 
    for (MovableEnnemies& mdisc : m_discs)
    {
        target.draw(mdisc);
    }

    // Main character
    target.draw(*m_MainCharacter);

    if (m_IsFinished)
    {
        //target.draw(m_EndgameText);
        sf::Vector2u winsize = { 1024, 768 };
        RenderEndMenu(target, winsize);
    }
}

void GameDemo::RenderEndMenu(sf::RenderTarget& target, sf::Vector2u& WINSIZE)
{

    // ImGui example menu overlay 
    bool show_app_simple_overlay = true;
    const float DISTANCE = 10.0f;
    int corner = 3;
    ImGuiIO& io = ImGui::GetIO();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoMove;
    ImVec2 window_pos = ImVec2((WINSIZE.x / 2.0f), (WINSIZE.y / 2.0f));
    ImVec2 window_pos_pivot = ImVec2(0.5f, 0.5f);
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

    ImGui::SetNextWindowBgAlpha(0.85f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", &show_app_simple_overlay, window_flags))
    {
        ImGui::Image(m_TextureTombstone);   //  allow display an image in the UI  =>  For life bar or display an icon
        ImGui::SetCursorPosX(static_cast<float>(m_TextureTombstone.getSize().x + 10.0f ));            // use to put back the cursor on top/left corner of the image to display above it
        ImGui::SetCursorPosY(static_cast<float>(m_TextureTombstone.getSize().y / 2.0f));
        ImGui::Text("Victory !");
        ImGui::SetCursorPosY(static_cast<float>(m_TextureTombstone.getSize().y + 10.0f));
        ImGui::Separator();
        ImGui::Text("Total Number of Deaths : %d ", m_MainCharacter->DeathCounter());
        if (m_MainCharacter->DeathCounter() < 12)
        {
            ImGui::Text("Rank : Cream Puff");
            ImGui::Text("Oldest Cat to Have Ever Lived");
        }
        else if (m_MainCharacter->DeathCounter() < 15)
        {
            ImGui::Text("Rank : Felicette");
            ImGui::Text("RIP : the first Astrocat");
        }
        else if (m_MainCharacter->DeathCounter() < 18)
        {
            ImGui::Text("Rank : Nyan Cat");
            ImGui::Text("Cake and rainbow");
        }
        else if (m_MainCharacter->DeathCounter() < 22)
        {
            ImGui::Text("Rank : Grumpy Cat");
            ImGui::Text("Just google me");
        }
        else if (m_MainCharacter->DeathCounter() < 25)
        {
            ImGui::Text("Rank : Colonel Meow");
            ImGui::Text("The 2014 Guinness World record for the longest fur on a cat");
        }
        else if (m_MainCharacter->DeathCounter() < 30)
        {
            ImGui::Text("Rank : Tom");
            ImGui::Text("Without Jerry, try again !");
        }
        else
        {
            ImGui::Text("Rank : Garfield");
            ImGui::Text("Lasagna forever");
        }


    }
    ImGui::End();

}




void GameDemo::RenderDebugMenu(sf::RenderTarget& target)
{
    // DEBUG 
    ImGui::Begin("Debug Menu");
    ImGui::Text("Press F1 to close this debug menu");
    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Main character position"))
    {
        const auto& mainCharCenterPos = m_MainCharacter->GetCenter();

        ImGui::Text("X: %f", mainCharCenterPos.x);
        ImGui::Text("Y: %f", mainCharCenterPos.y);
    }

    if (ImGui::CollapsingHeader("Game status"))
    {
        if (m_IsFinished)
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "GAME ENDED");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "GAME IN PROGRESS");
        }
    }
	
	if (ImGui::CollapsingHeader("Sound status"))
    {
		ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), (std::to_string(this->getMusicVolume())).c_str());		
		ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), (std::to_string(m_MainCharacter->GetSFXVolume())).c_str());
    }
	
	if (ImGui::CollapsingHeader("MainCharacter Status"))
    {
        if (m_MainCharacter->getCollidingPf())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Collision with Plateform");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "No collision with plateform");
        }
		
		if (m_MainCharacter->getCollidingLeft())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Collision Left : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "Collision Left : No");
        }
        
        if (m_MainCharacter->getCollidingRight())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Collision Right : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "Collision Right : No");
        }
        
        if (m_MainCharacter->getCollidingUp())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Collision Up : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "Collision Up : No");
        }
        
        if (m_MainCharacter->getCollidingDown())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Collision Down : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "Collision Down : No");
        }
        
        if (m_MainCharacter->getInTheAir())
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "In the Air : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "In the Air : No");
        }

        if (m_MainCharacter->isAllowJumping())
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f), "Can Jump : Yes");
        }
        else
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Can Jump : No");
        }        
        
        if (m_MainCharacter->getPlaying())
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f),  m_MainCharacter->getAnimName().c_str());
        }
        else
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), m_MainCharacter->getAnimName().c_str());
        }        
        
        if (m_MainCharacter->getAlive())
        {
            ImGui::TextColored(ImVec4(0.f, 255.0f, 0.f, 1.f),  "Alive");
        }
        else
        {
            ImGui::TextColored(ImVec4(255.f, 0.f, 0.f, 1.f), "Dead");
        }
		

    }
    

    if (ImGui::CollapsingHeader("Inputs"))
    {

        if (ImGui::TreeNode("Pad"))
        {
            // Color buttons, demonstrate using PushID() to add unique 
            // identifier in the ID stack, and changing style.
            std::vector<std::string> buttons_name = { "A", "B", "X", "Y","LB", "RB", "Back","Menu"};
            for (int i = 0; i < 8; i++)
            {
                if (i > 0)
                    ImGui::SameLine();
                    ImGui::PushID(i);
                    if (m_MainCharacter->getJoystickPressed(i))
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 8.0f, 0.85f, 0.85f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 8.0f, 0.6f, 0.6f));
                    }
                    
                    ImGui::Button(buttons_name[i].c_str());
                    ImGui::PopStyleColor(1);
                    ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Keyboard"))
        {
            // Color buttons, demonstrate using PushID() to add unique 
            // identifier in the ID stack, and changing style.
            std::vector<std::string> keyboard_name = { "Up", "Down", "Left", "Right", "Space" };
            for (int i = 0; i < 5; i++)
            {
                if (i > 0)
                    ImGui::SameLine();
                    ImGui::PushID(i);
					if (m_MainCharacter->getKeyboardKey(keyboard_name[i]))
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 5.0f, 0.85f, 0.85f));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 5.0f, 0.6f, 0.6f));
                    }
                    ImGui::Button(keyboard_name[i].c_str());
                    ImGui::PopStyleColor(1);
                    ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }


    m_gameUI->DisplayUI(this, m_MainCharacter, m_mushrooms);

    ImGui::End();



}