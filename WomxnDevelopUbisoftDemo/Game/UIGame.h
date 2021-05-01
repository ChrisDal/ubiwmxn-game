#pragma once 

class MainCharacter; 
class GameDemo;
class MovableEnnemies;

class UIGame
{

public: 
	UIGame(const std::string musicon, const std::string musicoff,
		const std::string soundon, const std::string soundoff);
	~UIGame() {}; 

	void DisplayDeath(int nb_deaths, int nb_max_deaths);

	void PlotTimerBar(const std::string name, float time_pct,
		ImVec4 framecol, ImVec4 histcolor,
		ImVec4 framecol_alert, ImVec4 histcolor_alert);

	void DisplayAudio(GameDemo* game, MainCharacter* perso,
		std::vector<MovableEnnemies>& l_ennemies);

	void DisplayUI(GameDemo* game, MainCharacter* perso,
					std::vector<MovableEnnemies>& l_ennemies); 


private:
	
	bool m_showUI{ true };
	float m_ingame_distance{ 2.0f };
	int m_corner = 0; 
	// Imgui Init
	ImGuiWindowFlags m_window_flags; 
	
	ImVec2 m_ui_pos; 
	ImVec2 m_ui_pos_pivot; 
	// Windows 
	float m_alpha; 
	
	// Textures 
	sf::Texture m_TextureMusicOn; 
	sf::Texture m_TextureMusicOff; 
	sf::Texture m_TextureSoundOn; 
	sf::Texture m_TextureSoundOff; 
	
	// Sprites associated 
	sf::Sprite m_icoMusic;
	sf::Sprite m_icoSFX;
	
	// bar colors 
	ImVec4 m_BarFrameAlert;
	ImVec4 m_HistoAlert;
	
	// Size windows
	sf::Vector2f m_WinSize; 

};

