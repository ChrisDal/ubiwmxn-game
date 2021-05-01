#pragma once

#include <Game/Plateform.h> 
#include <Game/EnumElements.h>
#include <Game/TileMap.h>

class DeadBody : public sf::Drawable, public BoxCollideable, public Animation
{
	// Animation
	enum class AnimName { Idle, Stack, Water, Fire, Iced, Void, 
							Smoked, FireEnd, Food }; 
	static sf::Texture*  m_pTextureAtlas;
	
    struct AllAnims {
        struct AnimType Idle;
        struct AnimType Stack;
        struct AnimType Water;
        struct AnimType Fire;
        struct AnimType Iced;
        struct AnimType Void;
        struct AnimType Smoked;
        struct AnimType FireEnd;
        struct AnimType Food;
    };

	static const float TIME_DESTRUCTION_WATER;
	static const float TIME_DESTRUCTION_LAVA;
	
	
public: 
	DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, 
			bool pass_through, terrain::Element elem, bool sidex, bool killed_by_move);
	
	void Update(float deltaTime, TileMap& Tm);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Texture handling 
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// Animation 
	void Play(AnimName anim_name, float deltaTime, bool looping);
	void setFrameTexture(AnimName anim_name, float deltaTime);
	void setFacingDirection(float speedx) override;

	// animation
	inline void setCurrentAnim(AnimName anim_name) { a_current_anim = anim_name; }
	void InitAnimType(); 

	// Time
	void ResetElapsedTime();
	bool ReachedTime();
	bool CanBeRemoved();
	void EatenByEnnemies() { m_external_flag = true; }
	
	// Plateform attribut
	void setWalkable(const bool& walkable) { m_isWalkable = walkable; }
	bool getWalkable() const { return m_isWalkable; }
	void DeadToPlateform();
	Plateform* get_Plateform();

	// Fire 
	terrain::Element getDeathTerrain() const { return m_death_element; }
	bool getIsOnFire() const { return m_isOnFire;  }

	// Food Transformation 
	void setFoodFrame();
	bool isFoodSet() const { return m_deathfood > 0;  }

	// Remove and Destructor 
	~DeadBody() {};

	

private:
	
	sf::Vector2f m_Position; 
	sf::Vector2f m_Velocity; 
	sf::Vector2f m_size; 
	sf::Sprite m_Sprite;
	Plateform m_plateform;
	terrain::Element m_death_element; 
	AnimName a_current_anim{ AnimName::Idle };
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
	bool m_isOnFire; 
	bool m_isFood; // will transform into dish
	
	// animation 
    std::map< AnimName, AnimType > dictAnim;
	float t_elapsed = 0.0f;
	short int m_deathfood = -1; // set final dish if food 
	bool m_external_flag{ false }; // eaten by mushrooms

};
