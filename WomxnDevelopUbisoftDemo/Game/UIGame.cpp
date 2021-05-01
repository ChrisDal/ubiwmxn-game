#include <stdafx.h>
#include <Game/EnumElements.h>
#include <Game/UIGame.h>
#include <Game/GameDemo.h>
#include <Game/MainCharacter.h>
#include <Game/MovableEnnemies.h>


/*
UIGame::UIGame()
	: m_alpha{ 0.85f }
	, m_BarFrameAlert{ (ImVec4)ImColor::HSV(0.5f / 7.0f, 0.5f, 0.6f) }
	, m_HistoAlert{ (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.6f) }
	, m_WinSize{ 1024, 768 }
{};*/

UIGame::UIGame(const std::string musicon, const std::string musicoff, 
				const std::string soundon, const std::string soundoff) 
	: m_alpha{0.85f}
	, m_BarFrameAlert{(ImVec4)ImColor::HSV(0.5f / 7.0f, 0.5f, 0.6f)}
	, m_HistoAlert{(ImVec4)ImColor::HSV(0.0f, 0.7f, 0.6f)}
	, m_WinSize{1024, 768}
{
	// Inputs 
	ImGuiIO& io = ImGui::GetIO();
	// Flags for positioning windows
    m_window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	
	// distance and pos
	m_ui_pos = ImVec2((m_corner & 1) ? io.DisplaySize.x - m_ingame_distance : m_ingame_distance, (m_corner & 2) ? io.DisplaySize.y - m_ingame_distance : m_ingame_distance);
	m_ui_pos_pivot = ImVec2((m_corner & 1) ? 1.0f : 0.0f, (m_corner & 2) ? 1.0f : 0.0f);

	// Music Image
	m_TextureMusicOn.loadFromFile(musicon); 
	m_TextureMusicOff.loadFromFile(musicoff);
	m_icoMusic.setTexture(m_TextureMusicOn);
	// Sound FX Image
	m_TextureSoundOn.loadFromFile(soundon);
	m_TextureSoundOff.loadFromFile(soundoff);
	m_icoSFX.setTexture(m_TextureSoundOn);
	
}; 






void UIGame::PlotTimerBar(const std::string name, float time_pct,
					ImVec4 framecol, ImVec4 histcolor, 
					ImVec4 framecol_alert, ImVec4 histcolor_alert)
{
	
	// Bar for a timer   
	float timer_progress = 1.0f - time_pct;

	if (timer_progress < 0.3f)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, framecol_alert);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, histcolor_alert);
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, framecol);
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, histcolor);
	}

	ImGui::ProgressBar(timer_progress, ImVec2(70.0f, 0.0f));
	ImGui::SameLine(0.0f, 5.0f);
	ImGui::Text(name.c_str());
	ImGui::PopStyleColor(2);
}

void UIGame::DisplayDeath(int nb_deaths, int nb_max_deaths)
{

	if (nb_deaths >= 9)
	{
		ImGui::TextColored(ImVec4(230.0f, 168.0f, 0.f, 1.f),
			"Deaths: %1d / %1d",
			nb_deaths, nb_max_deaths);
	}
	else
	{
		ImGui::TextColored(ImVec4(255.0f, 255.0f, 255.f, 1.f),
			"Deaths: %1d / %1d", nb_deaths, nb_max_deaths);
	}


}


void UIGame::DisplayAudio(GameDemo* game, MainCharacter* perso, std::vector<MovableEnnemies>& l_ennemies)
{
	ImGui::PushID(10);
	bool click_music = ImGui::ImageButton(*m_icoMusic.getTexture(), 
											sf::Vector2f(m_icoMusic.getTexture()->getSize()), 
											0,  sf::Color(0,0,0,255), sf::Color(255,255,255) );
	if (ImGui::IsItemHovered()) 
		ImGui::SetTooltip("Music Volume");	
		
	int slider_music = static_cast<int>(game->getMusicVolume());
	
	if (click_music)
	{
		ImGui::OpenPopup("music_popup");	
	}

	if (ImGui::BeginPopup("music_popup", ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Volume");
		ImGui::SliderInt("", &slider_music, 0, 100, "%d", ImGuiSliderFlags_None);
		ImGui::EndPopup();
		game->setMusicVolume((float)slider_music);
	}
	if (game->getMusicVolume() == 0.0f)
	{
		m_icoMusic.setTexture(m_TextureMusicOff);
	}
	else
	{
		if (m_icoMusic.getTexture() != &m_TextureMusicOn)
		{
			m_icoMusic.setTexture(m_TextureMusicOn);
		}
	}
	
	ImGui::PopID();
	
	ImGui::SameLine(0.0f, 5.0f);
	
	// Enable Disable music on right click 
	/*if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
		if (m_icoMusic.getTexture() == &m_TextureIcoMusic)
		{
			m_icoMusic.setTexture(m_TextureIcoNoMusic);
			game->setMusicVolume(0.0f);
		}
		else
		{
			m_icoMusic.setTexture(m_TextureIcoMusic);
			game->setMusicVolume(perso->GetSFXVolume() / 10.f);
		}
	}*/
	
	ImGui::PushID(11);
	bool click_sfx = ImGui::ImageButton(*m_icoSFX.getTexture(), 
										sf::Vector2f(m_icoSFX.getTexture()->getSize()),
										0, sf::Color(0, 0, 0, 255), sf::Color(255, 255, 255));
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Modify SFX Volume");
	}
	
	
	int slider_i = static_cast<int>(perso->GetSFXVolume());
	if (click_sfx)
	{
		ImGui::OpenPopup("my_volume_popup");
	}
	
	if (ImGui::BeginPopup("my_volume_popup", ImGuiWindowFlags_NoMove))
	{
		ImGui::Text("Volume");
		ImGui::SliderInt("", &slider_i, 0, 100, "%d", ImGuiSliderFlags_None);
		ImGui::EndPopup();
		perso->SetSFXVolume(static_cast<float>(slider_i));
		
		for (auto& mush : l_ennemies)
		{
			mush.SetSFXVolume(static_cast<float>(slider_i));
		}
	}
	
	if (perso->GetSFXVolume() == 0.0f)
	{
		m_icoSFX.setTexture(m_TextureSoundOff);
	}
	else
	{
		if (m_icoSFX.getTexture() != &m_TextureSoundOn)
		{
			m_icoSFX.setTexture(m_TextureSoundOn);
		}
	}
	ImGui::PopID();
	
}



// Display Main UI : Death Bar, Timers and Sound handling 
void UIGame::DisplayUI(GameDemo* game, MainCharacter* perso, std::vector<MovableEnnemies>& l_ennemies)
{

	// Position and transparence 
	ImGui::SetNextWindowPos(m_ui_pos, ImGuiCond_Always, m_ui_pos_pivot);
	ImGui::SetNextWindowBgAlpha(UIGame::m_alpha); // Transparent background

	if (ImGui::Begin("Game UI", &m_showUI, m_window_flags))
	{

		DisplayDeath(perso->DeadBodiesCounter(), perso->DeadBodiesMax());

		ImGui::SameLine(0.0f, 20.0f);
		// water 
		PlotTimerBar("Water",
			perso->GetPourcentageAllowedTime(terrain::Element::Water),
			(ImVec4)ImColor::HSV(188.8f / 360.f, 0.4f, 0.7f),
			(ImVec4)ImColor::HSV(188.8f / 360.f, 0.7f, 0.7f), 
			m_BarFrameAlert, m_HistoAlert);

		ImGui::SameLine(0.0f, 20.0f);

		// Void
		PlotTimerBar("Void",
			perso->GetPourcentageAllowedTime(terrain::Element::Void),
			(ImVec4)ImColor::HSV(5.0f / 7.0f, 0.5f, 0.6f),
			(ImVec4)ImColor::HSV(5.0f / 7.0f, 0.7f, 0.6f),
			m_BarFrameAlert, m_HistoAlert);

		ImGui::SameLine(0.0f, 20.0f);

		// Lava 
		PlotTimerBar("Lava",
			perso->GetPourcentageAllowedTime(terrain::Element::Lava),
			(ImVec4)ImColor::HSV(0.5f / 7.0f, 0.5f, 0.6f),
			(ImVec4)ImColor::HSV(0.5f / 7.0f, 0.7f, 0.6f),
			m_BarFrameAlert, m_HistoAlert);

		// Set cursor to the right 
		ImGui::SetCursorPosX(m_WinSize.x - 75.0f);
		ImGui::SetCursorPosY(0.0f);

		// handling audio
		DisplayAudio(game, perso, l_ennemies);
	}

	ImGui::End();

}