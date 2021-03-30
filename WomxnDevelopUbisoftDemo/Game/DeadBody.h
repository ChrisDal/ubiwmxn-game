#pragma once

#include <Game/Plateform.h> 

class DeadBody : public sf::Drawable, public BoxCollideable, public Animation
{
	// common to dead bodies
	enum class AnimAction { Idle, Stack, Launch, Fire, Iced, Slippy, 
							Smoked, Swollen, Ladder,Reborn }; 
	static sf::Texture*  m_pTextureAtlas;
	
    struct AllAnims {
        struct AnimType Idle;
        struct AnimType Stack;
        struct AnimType Launch;
        struct AnimType Fire;
        struct AnimType Iced;
        struct AnimType Slippy;
        struct AnimType Smoked;
        struct AnimType Swollen;
        struct AnimType Ladder;
    };

	
public: 
	DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy);
	
	void Update(float deltaTime); 
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Texture handling 
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// Animation 
	void Play(AnimAction anim_name, float deltaTime, bool loop);
	void setFrameTexture(AnimAction anim_name, float deltaTime);

	// animation
	inline void setCurrentAnim(AnimAction anim_name) { a_current_anim = anim_name; }
	void InitAnimType(); 
	// Plateform attribut
	void setWalkable(const bool& walkable) { m_isWalkable = walkable; }
	bool getWalkable() const { return m_isWalkable; }
	void DeadToPlateform();
	Plateform* get_Plateform();

private:
	
	sf::Vector2f m_Position; 
	sf::Vector2f m_size; 
	sf::Sprite m_Sprite;
	Plateform m_plateform;

	AnimAction a_current_anim{ AnimAction::Idle };
	AllAnims m_AllAnims;
	// collisions 
	bool _colliding_plateforms{false}; 
	// colision side detection on Quad
	bool c_left; 
	bool c_right; 
	bool c_up; 
	bool c_down; 

	// set if walkable or not 
	bool m_isWalkable;
};
