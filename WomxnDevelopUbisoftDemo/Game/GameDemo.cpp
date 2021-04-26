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
    // Sound
	m_TextureIcoMusic.loadFromFile(".\\Assets\\icons\\musicOn.png"); 
	m_TextureIcoNoMusic.loadFromFile(".\\Assets\\icons\\musicOff.png"); 
	m_icoMusic.setTexture(m_TextureIcoMusic);
	m_TextureIcoSound.loadFromFile(".\\Assets\\icons\\audioOn.png"); 
	m_TextureIcoNoSound.loadFromFile(".\\Assets\\icons\\audioOff.png"); 
	m_icoSFX.setTexture(m_TextureIcoSound);


    // map tile 
    m_Tilemap.loadCsvTilemap("Assets\\levels\\Level1-TMPF-mini-map.csv");
    //m_Tilemap.loadCsvTilemap("Assets\\levels\\Level1-TMPF-Niv-01-tiles.csv");
    // test
    m_Tilemap.load("Assets\\tileset_32x32.png", sf::Vector2u(32, 32), 32, 24);
	// define found plateform 
	m_plateform = m_Tilemap.getPlateforms(); 

    // Ennemies 
    m_Elements.loadCsvTilemap("Assets\\levels\\Level1-TMPF-mini-map-elements.csv");
    //m_Elements.loadCsvTilemap("Assets\\levels\\Level1-TMPF-Niv-01-elem.csv");
    m_Elements.setTilemapType(false);

    // Get texture 
    const std::string texture_name = "Assets\\ennemies_empty3.png"; 
    m_TextureAtlas.loadFromFile(texture_name);
    Ennemie::SetTextureAtlas(&m_TextureAtlas);
    ObjectsElements::SetTextureAtlas(&m_TextureAtlas);
    MovableEnnemies::SetTextureAtlas(&m_TextureAtlas);
    VFX::SetTextureAtlas(&m_TextureAtlas);
	
	// Dead Body Texture
	const std::string dead_texture_name = "Assets\\daedcat_addon_sprite.png"; 
    m_TextureDead.loadFromFile(dead_texture_name);
	DeadBody::SetTextureAtlas(&m_TextureDead);


    const sf::Vector2u WINSIZE = { 1024, 768 };
    m_ennemies = m_Elements.loadObjects(texture_name, sf::Vector2u(32, 32), sf::Vector2u(10, 50), 
                                        static_cast<unsigned int>(WINSIZE.x / 32.0f), 
                                        static_cast<unsigned int>(WINSIZE.y / 32.0f),
                                        m_objects, m_cactus, m_checkpoints, m_exit_sign, m_mushrooms);
    
    // SetUp Ennemies Path 
    for (MovableEnnemies& mush : m_mushrooms)
    {
        sf::Vector2f TargetPoint{ mush.GetCenter() };
        TargetPoint.x += 32.0f * 6.0f;
        mush.setPath(mush.GetCenter(), TargetPoint);
    } 
    
   

    // Load main character 
    m_MainCharacter = new MainCharacter(WINSIZE, m_Elements.getMainCharacterSpawnPosition());

    // Set sound volume 
    m_MainCharacter->SetSFXVolume(50.0f); 
    this->setMusicVolume(m_MainCharacter->GetSFXVolume()/10.0f);
	
}

GameDemo::~GameDemo()
{
    delete m_MainCharacter;
}

void GameDemo::Update(float deltaTime)
{

    if (not m_noMusic)
    {
        if ( not IsBGMusicPlaying())
        {
            // if pause or stopped => play
            m_bgmusic.play(); 
        }
    }

    m_MainCharacter->Update(deltaTime, m_plateform, m_Tilemap,
                            m_ennemies, m_cactus, m_mushrooms);

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
    }

    // Check the mushrooms 
    for (MovableEnnemies& mvenm : m_mushrooms)
    {
        mvenm.Update(deltaTime);
        if (mvenm.getIsFinished())
        {
            sf::Vector2f TargetPoint{ mvenm.GetCenter() };

            if (m_Animation_AR) { 
                TargetPoint.x -= 32.0f * 5.0f; 
            } else { 
                TargetPoint.x += 32.0f * 5.0f; 
            }
            // Make AR 
            mvenm.setPath(mvenm.GetCenter(), TargetPoint);
            m_Animation_AR = not m_Animation_AR;
        }
    }


    if (!m_IsFinished)
    {
        if (m_exit_sign.Contains(m_MainCharacter->GetCenter()))
        {
            m_EndgameSound.play();

            m_MainCharacter->StartEndGame();
            m_exit_sign.StartEndGame();
            m_IsFinished = true;
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
    // Msuhroom 
    for (MovableEnnemies& mvenm : m_mushrooms)
    {
        target.draw(mvenm);
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

	

    ImGui::End();


    /*// ImGui example menu overlay 
    static bool show_app_simple_overlay = true;
    const float DISTANCE = 10.0f;
    static int corner = 3;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != -1)
    {
        window_flags |= ImGuiWindowFlags_NoMove;
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", &show_app_simple_overlay, window_flags))
    {
        //ImGui::Image(main_Door.GetTexture());   //  allow display an image in the UI  =>  For life bar or display an icon
        //ImGui::SetCursorPosY(static_cast<float>(main_Door.GetTexture().getSize().y));            // use to put back the cursor on top/left corner of the image to display above it

        ImGui::Text("Main character velocity");
        ImGui::Separator();
        ImGui::Text(" Velocity character X : %.2f", m_MainCharacter->getVelocity().x);
        ImGui::Text(" Velocity character Y : %.2f", m_MainCharacter->getVelocity().y);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (&show_app_simple_overlay && ImGui::MenuItem("Close")) show_app_simple_overlay = false;
            ImGui::EndPopup();
        }
    }
    ImGui::End();  */
    
    // Death Menu overlay 
    static bool show_UI = true;
    const float DISTANCE2 = 2.0f;
    static int corner2 = 0;
    ImGuiIO& io2 = ImGui::GetIO();
    ImGuiWindowFlags window_flags2 = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner2 != -1)
    {
        window_flags2 |= ImGuiWindowFlags_NoMove;
        ImVec2 window_pos2 = ImVec2((corner2 & 1) ? io2.DisplaySize.x - DISTANCE2 : DISTANCE2, (corner2 & 2) ? io2.DisplaySize.y - DISTANCE2 : DISTANCE2);
        ImVec2 window_pos_pivot2 = ImVec2((corner2 & 1) ? 1.0f : 0.0f, (corner2 & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos2, ImGuiCond_Always, window_pos_pivot2);
    }
    ImGui::SetNextWindowBgAlpha(0.85f); // Transparent background
    if (ImGui::Begin("Game UI", &show_UI, window_flags2))
    {
        int nb_deaths = m_MainCharacter->DeadBodiesCounter();
        if (nb_deaths >= 9)
        {
            ImGui::TextColored(ImVec4(230.0f, 168.0f, 0.f, 1.f), "Deaths: %1d / %1d", nb_deaths, m_MainCharacter->DeadBodiesMax());
        }
        else
        {
            ImGui::TextColored(ImVec4(255.0f, 255.0f, 255.f, 1.f), "Deaths: %1d / %1d", nb_deaths, m_MainCharacter->DeadBodiesMax());
        }

        ImGui::SameLine(0.0f, 20.0f); 

        // Bar for water  
        float timer_progress_water = 1.0f - m_MainCharacter->GetPourcentageAllowedTime(terrain::Element::Water);
        
        if (timer_progress_water < 0.3f)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.4f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.6f));

        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(188.8f / 360.f, 0.4f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(188.8f / 360.f, 0.7f, 0.7f));
        }

        ImGui::ProgressBar(timer_progress_water, ImVec2(70.0f, 0.0f));
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text("Water");
        ImGui::PopStyleColor(2);
        // Bar for Void
        ImGui::SameLine(0.0f, 20.0f);
        float timer_progress_void = 1.0f - m_MainCharacter->GetPourcentageAllowedTime(terrain::Element::Void);
        if (timer_progress_void < 0.3f)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.4f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.6f));

        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(5.0f / 7.0f, 0.5f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(5.0f / 7.0f, 0.7f, 0.6f));
        }
        ImGui::ProgressBar(timer_progress_void, ImVec2(70.0f, 0.0f));
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text("Void");
        ImGui::PopStyleColor(2);

        // Bar for Void
        ImGui::SameLine(0.0f, 20.0f);
        float timer_progress_lava = 1.0f - m_MainCharacter->GetPourcentageAllowedTime(terrain::Element::Lava);
        if (timer_progress_lava < 0.3f)
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.4f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(0.0f / 7.0f, 0.7f, 0.6f));

        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.5f / 7.0f, 0.5f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor::HSV(0.5f / 7.0f, 0.7f, 0.6f));
        }
        ImGui::ProgressBar(timer_progress_lava, ImVec2(70.0f, 0.0f));
        ImGui::SameLine(0.0f, 5.0f);
        ImGui::Text("Lava");
        ImGui::PopStyleColor(2);
		
		
		
		// Parameters 
		ImGui::SetCursorPosX(1024.0f - 150.0f);
		ImGui::SetCursorPosY(0.0f);
		// music 
		ImGui::PushID(10);
        bool click_music = ImGui::ImageButton(*m_icoMusic.getTexture(), 
											    sf::Vector2f(m_icoMusic.getTexture()->getSize()), 
                                                0,  sf::Color(0,0,0,255), sf::Color(255,255,255) );
		if (ImGui::IsItemHovered()) 
			ImGui::SetTooltip("Music Volume");	
			
		int slider_music = this->getMusicVolume();
		if (click_music)
		{
			ImGui::OpenPopup("music_popup");	
		}

		if (ImGui::BeginPopup("music_popup", ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Volume");
			ImGui::SliderInt("", &slider_music, 0, 100, "%d", ImGuiSliderFlags_None);
			ImGui::EndPopup();
            this->setMusicVolume((float)slider_music);
		}
        if (this->getMusicVolume() == 0.0f)
        {
            m_icoMusic.setTexture(m_TextureIcoNoMusic);
        }
        else
        {
            if (m_icoMusic.getTexture() != &m_TextureIcoMusic)
            {
                m_icoMusic.setTexture(m_TextureIcoMusic);
            }
        }
		
		// Enable Disable music on right click 
        /*if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            if (m_icoMusic.getTexture() == &m_TextureIcoMusic)
            {
                m_icoMusic.setTexture(m_TextureIcoNoMusic);
                this->setMusicVolume(0.0f);
            }
            else
            {
                m_icoMusic.setTexture(m_TextureIcoMusic);
                this->setMusicVolume(m_MainCharacter->GetSFXVolume() / 10.f);
            }
        }*/
		
        ImGui::PopID();
		ImGui::SameLine(0.0f, 5.0f);
		// SFX
        ImGui::PushID(11);
        bool click_sfx = ImGui::ImageButton(*m_icoSFX.getTexture(), 
                                            sf::Vector2f(m_icoSFX.getTexture()->getSize()),
                                            0, sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255));
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Modify SFX Volume");
        }
			
		// Enable Disable SFX 
        /*if (click_sfx)
        {
            if (m_icoSFX.getTexture() == &m_TextureIcoSound)
            {
                m_icoSFX.setTexture(m_TextureIcoNoSound);
                m_MainCharacter->SetSFXVolume(0.0f);
            }
            else
            {
                m_icoSFX.setTexture(m_TextureIcoSound);
                m_MainCharacter->SetSFXVolume(50.0f);
            }
        }*/
		
		// Handling Volume
		int slider_i = m_MainCharacter->GetSFXVolume();
		if (click_sfx)
		{
			ImGui::OpenPopup("my_volume_popup");
		}
		
		if (ImGui::BeginPopup("my_volume_popup", ImGuiWindowFlags_NoMove))
		{
			ImGui::Text("Volume");
			ImGui::SliderInt("", &slider_i, 0, 100, "%d", ImGuiSliderFlags_None);
			ImGui::EndPopup();
            m_MainCharacter->SetSFXVolume((float)slider_i);
		}
		
        if (m_MainCharacter->GetSFXVolume() == 0.0f)
        {
            m_icoSFX.setTexture(m_TextureIcoNoSound);
        }
        else
        {
            if (m_icoSFX.getTexture() != &m_TextureIcoSound)
            {
                m_icoSFX.setTexture(m_TextureIcoSound);
            }
        }
        ImGui::PopID();

    }

    ImGui::End();



}