#pragma once 
#include <Game/Ennemie.h> 
#include <Game/SequencePath.h> 

class MovableEnnemies : public Ennemie
{

	enum class AnimName {Idle, Walk, Jump, Eat,  Die};

	static sf::Texture* m_pTextureAtlas;

	struct AllAnims {
		struct AnimType Idle;
		struct AnimType Walk;
		struct AnimType Jump;
		struct AnimType Eat;
		struct AnimType Die;
	};

	struct SoundType {
		sf::SoundBuffer s_buffer;       // sound buffer 
		bool is_playing{ false };       // sound is playing
		bool no_sound{ true };          // = sound not loaded
		std::string pathsound{ "" };    // path to wav sound file 
		bool looping{ false };          // looping or not 
	};

	struct AllSounds {
		struct SoundType Idle;
		struct SoundType Walk;
		struct SoundType Jump;
		struct SoundType Eat;
		struct SoundType Die;
	};

	static float m_SFX_volume;

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

	bool ObjectInRange(const BoxCollideable& dbd); 
	bool isPatrolling() const { return m_patrolling; }
	void configurePatrol(sf::Vector2f start, sf::Vector2f end);
	std::vector<sf::Vector2f> getPatrol()  { return std::vector<sf::Vector2f> {m_patrol_start, m_patrol_end}; }

	// Hit an other object 
	void EatPerso(); 
	void StopEatingPerso(); 

	// Sound
	void InitSoundType();
	void LoadStructSound(struct SoundType& onesound, const std::string soundpath, bool looping);
	inline void setSoundType(AnimName anim);
	inline void playSFX(AnimName anim);
	void resetPlaying();

	static void SetSFXVolume(float percentage) { m_SFX_volume = percentage; }
	static const float GetSFXVolume() { return m_SFX_volume; }

private:
	sf::Sprite m_Sprite;
	sf::Vector2f m_size;
	sf::Vector2f m_Position;
	sf::Vector2f m_Velocity;
	// movement 
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

	// Sound
	sf::Sound m_soundfx;
	std::map< AnimName, SoundType > dictSound;
	AllSounds m_AllSounds;

	// attributes for all types of ennemies
	bool _colliding_plateforms;
	bool moving;
	bool is_animated;
	bool _colliding_fire;
	bool _colliding_water;
	bool _colliding_void;
	bool _colliding_lava;
	bool _colliding{ false };
	
	bool m_patrolling{true}; 
	sf::Vector2f m_patrol_start{ 0.0f, 0.0f };
	sf::Vector2f m_patrol_end{ 32.0f, 0.0f };

};