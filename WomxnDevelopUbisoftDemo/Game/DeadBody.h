#pragma once

#include <Game/Plateform.h> 
#include <Game/EnumElements.h>

class DeadBody : public sf::Drawable, public BoxCollideable, public Animation
{
	// common to dead bodies
	enum class AnimName { Idle, Stack, Launch, Fire, Iced, Void, 
							Smoked, FireEnd, Ladder,Reborn }; 
	static sf::Texture*  m_pTextureAtlas;
	
    struct AllAnims {
        struct AnimType Idle;
        struct AnimType Stack;
        struct AnimType Launch;
        struct AnimType Fire;
        struct AnimType Iced;
        struct AnimType Void;
        struct AnimType Smoked;
        struct AnimType FireEnd;
        struct AnimType Ladder;
    };
	
	
public: 
	DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, 
			bool pass_through, terrain::Element elem, bool sidex);
	
	void Update(float deltaTime); 
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Texture handling 
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// Animation 
	void Play(AnimName anim_name, float deltaTime);
	void setFrameTexture(AnimName anim_name, float deltaTime);
	void setFacingDirection(float speedx) override;

	// animation
	inline void setCurrentAnim(AnimName anim_name) { a_current_anim = anim_name; }
	void InitAnimType(); 
	// Plateform attribut
	void setWalkable(const bool& walkable) { m_isWalkable = walkable; }
	bool getWalkable() const { return m_isWalkable; }
	void DeadToPlateform();
	Plateform* get_Plateform();

	// Remove and Destructor 
	~DeadBody() {};

	

private:
	
	sf::Vector2f m_Position; 
	sf::Vector2f m_size; 
	sf::Sprite m_Sprite;
	Plateform m_plateform;
	terrain::Element m_death_element; 
	AnimName a_current_anim{ AnimName::Smoked };
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
	
	// animation 
    std::map< AnimName, AnimType > dictAnim;
};
