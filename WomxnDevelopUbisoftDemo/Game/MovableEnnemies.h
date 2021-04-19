#pragma once 
#include <Game/Ennemie.h> 
#include <Game/SequencePath.h> 

class MovableEnnemies : public Ennemie
{

	static sf::Texture* m_pTextureAtlas;

	struct AllAnims {
		struct AnimType Idle;
		struct AnimType Walk;
		struct AnimType Jump;
		struct AnimType DoubleJump;
		struct AnimType Die;
		struct AnimType Hurt;
		struct AnimType Dodge;
	};


public:

	MovableEnnemies(sf::Vector2f SpawnPosition, unsigned int sx, unsigned sy, sf::Vector2u& upperleft);
	MovableEnnemies(){};
	~MovableEnnemies() {};

	static const sf::Texture* GetTextureAtlas() { return m_pTextureAtlas; }
	static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }

	bool setPath(sf::Vector2f beginpoint, sf::Vector2f endpoint);
	bool getIsFinished() const { return m_Path.isFinished();  }

	void Update(float deltaTime);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const; 
	void StartEnd() {};

private:
	sf::Sprite m_Sprite;
	sf::Vector2f m_size;
	sf::Vector2f m_Position;
	sf::Vector2f m_Velocity;
	SequencePath m_Path;

	// Death 
	bool m_Dead;

	// Fire : 
	bool m_weak_fire;
	float counter_fire = 0.0f;
	Fire m_current_fire = Fire::None;
	short unsigned int step_fire = 0;
	std::map<int, Fire> m_fire_steps{ {0, Fire::None}, {1, Fire::Red},  {2, Fire::White},
									{3, Fire::SemiTransp},{4, Fire::Transp} };

	// animation 
	float a_counter_seconds = 1.0f / 10.0f;

	// attributes for all types of ennemies
	bool _colliding_plateforms;
	bool moving;
	bool is_animated;
	bool _colliding_fire;
	bool _colliding_water;
	bool _colliding_void;
	bool _colliding_lava;
	bool _colliding{ false };

};