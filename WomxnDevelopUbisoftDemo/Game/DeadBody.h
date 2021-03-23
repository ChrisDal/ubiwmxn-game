#pragma once

#include <Game/MainCharacter.h>

class DeadBody : public sf::Drawable, public BoxCollideable, public Animation
{
	// common to dead bodies
	enum class AnimAction { Idle, Stack, Launch, Fire, Iced, Slippy }; 
	static sf::Texture*  m_pTextureAtlas;
	
public: 
	DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, MainCharacter mchara);
	
	void Update(float deltaTime); 
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Texture handling 
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// Animation 
	void Play(AnimAction anim_name, float deltaTime);
	void setFrameTexture(AnimAction anim_name, float deltaTime);


	inline void setCurrentAnim(AnimAction anim_name) { a_current_anim = anim_name; }
	

private:
	
	sf::Vector2f m_Position; 
	sf::Vector2f m_size; 
	sf::Sprite m_Sprite;
	
	// dependant of maincharacter 
	MainCharacter* m_pMainCharacter = nullptr; 
	AnimAction a_current_anim{ AnimAction::Idle };
	
	// collisions 
	bool _colliding_plateforms{false}; 
	// colision side detection on Quad
	bool c_left; 
	bool c_right; 
	bool c_up; 
	bool c_down; 
};
