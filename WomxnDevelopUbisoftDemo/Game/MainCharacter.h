#pragma once
#include <Game/TileMap.h>
#include <Game/Plateform.h> 
#include <Game/DeadBody.h> 
#include <Game/EnumElements.h> 

class Ennemie;

class MainCharacter : public sf::Drawable, public BoxCollideable
{
    enum class AnimName { Idle, Walk, Jump, DoubleJump, 
                            Attack, Hurt, Die, 
                            FireSet, FireBegin, FireEnd,
                            Reborn };

    struct AnimType {
        short unsigned int nb_frames_anim{ 0 };
        short unsigned int line_anim{ 0 };
        short unsigned int a_offset{ 0 };
        std::string name{ "" };
    };

    struct AllAnims {
        struct AnimType Idle;
        struct AnimType Walk;
        struct AnimType Jump;
        struct AnimType DoubleJump;
        struct AnimType Die;
        struct AnimType Hurt;
        struct AnimType FireSet;
        struct AnimType FireBegin;
        struct AnimType FireEnd;
        struct AnimType Reborn;
    };

public:	
    MainCharacter(sf::Vector2u WIN_LIMITS, sf::Vector2f spawn_position);

    void Update(float deltaTime, std::vector<Plateform> &Pf, TileMap& Tm, std::vector<Ennemie>& l_ennemie, std::vector<Ennemie>& l_cactus);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    
    // Colliding and position 
	bool getCollidingPf() const { return _colliding_plateforms;}
	bool isCollidingLeft(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingRight(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingUp(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingDown(const BoxCollideable& other,  bool keypressed) const;
    void isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf, std::vector<Ennemie>& l_cactus);
    bool OnTopOf(BoxCollideable& other);
    bool BelowOf(BoxCollideable& other);
    inline bool ToTheLeft(BoxCollideable& other);
    inline bool ToTheRight(BoxCollideable& other);
    
    // Get Colliding 
	bool getCollidingLeft() const { return c_left;}
	bool getCollidingRight() const { return c_right;}
	bool getCollidingUp() const { return c_up;}
	bool getCollidingDown() const { return c_down;}
	
	// Inputs 
    bool getJoystickPressed(int& btn_indx) const; 
	bool getKeyboardKey(std::string keyname) const;

    // Position / Velocity
    void setPosition(float deltaTime, std::vector<Plateform>& Pf, std::vector<Ennemie>& l_cactus, short unsigned int& cloop);
    void setRespawnPosition(const sf::Vector2f& new_respawn);
    sf::Vector2f getVelocity() const;
    
    // Jump
    bool isAllowJumping() const { return m_CanJump;}
    void ResetJumpCounter();
	
	// Set and Get Alive
	void setAliveOrDead(const bool& not_dead) { m_isAlive = not_dead;}
	bool getAlive() const { return m_isAlive; }
	// Define if Alive or not
	bool Alive(float deltaTime, std::vector<Ennemie> l_ennemies); 

    // Elements with associated time counters
    void setInElements(TileMap& Tm); // set air, water, void, lava
    void ResetElements();
    bool TimerElements(float deltaTime, bool& inelement_flag, const float& limit, float& element_timer);
    void ResetTimers();
	float GetPourcentageAllowedTime(terrain::Element elem) const ;
    bool OnFire() const { return m_touched_lava; } // Lava set this on fire
    bool getInTheAir()  const { return m_InTheAir; }
    
    //  ----- Death
    // Death counter 
    int  DeathCounter() const { return m_death_counter; }
    void DeathCounterAdd() { m_death_counter += 1; }
    void DeathCounterReset() { m_death_counter = 0; }
    int  DeadBodiesCounter() const { return m_deadbodies.size() > MAX_DEADBODIES ? MAX_DEADBODIES : static_cast<int>(m_deadbodies.size());  }
    int  DeadBodiesMax() const { return MAX_DEADBODIES; }
    // Dead body count
    void UpdateDeadBodies(); 


    // Animation 
    void InitAnimType();
    void Play(AnimName anim_name, float deltaTime, bool loop);
    void Pause();
    void Stop();
    void setFrameTexture(AnimName anim_name, float deltaTime);
    //void resetFrameCounter() {a_framecount = 0; a_framecounttexture= 0; };
    void setCurrentAnim(AnimName anim_name) { m_current_anim = anim_name; };
    bool getPlaying() const { return is_PlayingAnim; };
    void setPlaying(const bool& status) { is_PlayingAnim = status; };
    std::string getAnimName(); 
    bool a_done_anim{ false }; 


private:
    sf::Texture m_Texture;
    sf::Sprite m_Sprite;

    bool m_IsUsingJoystick;
    unsigned int m_JoystickIndex;
    bool m_WasButtonPressed;

    sf::Vector2f m_Position; 
    sf::Vector2f m_Velocity;
    // Sign Velocity : true pos, false neg 
    sf::Vector2<bool> s_Velocity = { false, false };
	
	// DEAD BODIES 
	std::vector<DeadBody> m_deadbodies{}; 
    // Timers
    bool  m_DiedInWater{ false }; 
    bool  m_DiedInVoid{ false };
    bool  m_DiedInLava{ false };
    // counters 
    int m_death_counter{ 0 };
    const short unsigned int MAX_DEADBODIES{ 9 };
	
    // amount of time in elements 
    float m_CounterWater{ 0.0f };
    float m_CounterVoid{ 0.0f };
    float m_CounterLava{ 0.0f };
	
    bool m_IsPlayingEndGame;

    // in the air vs on the floor 
    bool m_isWalkable; 
    bool m_InTheAir; 
    bool m_InTheWater;
    bool m_InTheVoid;
    bool m_InTheLava;
    bool m_touched_lava{ false }; 
    bool m_timer_lava{ false }; 


    // Jumping
    bool m_CanJump; 
    bool m_IsJumping; 
    short unsigned int m_nbjumps{ 0 };
	
	// Dead or alive 
	bool m_isAlive{true}; 
	bool m_Respawning{false}; 
    sf::Vector2f m_RespawnPosition{0.0f,0.f};

    // max move window 
    sf::Vector2f WIN_LIMIT_X;
    sf::Vector2f WIN_LIMIT_Y;
	
	// Colliding 
	bool _colliding_plateforms{false}; 
	bool _colliding_deadbodies{false}; 
	bool _colliding_cactus{false}; 
	// colision side detection on Quad
	bool c_left; 
	bool c_right; 
	bool c_up; 
	bool c_down; 


    // animation texure 
    sf::Vector2u a_textsquare_offset;
    // Animation has a framerate mais on resfresh tous les deltaTime = 30 ou 60fps
    unsigned int a_framecount = 0;
    unsigned int a_framecounttexture = 0;
    unsigned int a_framerate = 10; // anim = 10 fps
    const float a_spi = 1.0f / a_framerate; // inverse framerate
	float sumdeltaTime = 0.0f;
    // AnimType
    std::map< AnimName, AnimType > dictAnim;
    AnimName m_current_anim = AnimName::Idle; 
    bool is_PlayingAnim{ false };
    std::vector<AnimName> m_OneTimeAnimations{};
    AllAnims m_AllAnims;
    // Facing direction
    bool a_direction{ false }; // true: right, false: left 
    void setFacingDirection(); 

    // current element : Air / Void / Water 
    terrain::Element m_current_elem{ terrain::Element::Air };
    
    
};