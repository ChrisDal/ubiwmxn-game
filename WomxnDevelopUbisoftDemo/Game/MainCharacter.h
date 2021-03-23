#pragma once
#include <Game/TileMap.h>
#include <Game/Plateform.h> 



class MainCharacter : public sf::Drawable, public BoxCollideable
{
    enum class AnimName { Idle, Walk, Jump, DoubleJump, 
                            Attack, Hurt, Die, Dodge, 
                            Surprise, Reborn };

public:	
    MainCharacter(sf::Vector2u WIN_LIMITS);

    void Update(float deltaTime, std::vector<Plateform> &Pf, TileMap& Tm);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    
    sf::Vector2f getVelocity() const; 
	bool getCollidingPf() const { return _colliding_plateforms;}
	bool isCollidingLeft(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingRight(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingUp(const BoxCollideable& other,  bool keypressed) const;
	bool isCollidingDown(const BoxCollideable& other,  bool keypressed) const;
    void isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf, bool& colliding);
    
    //  get

	bool getCollidingLeft() const { return c_left;}
	bool getCollidingRight() const { return c_right;}
	bool getCollidingUp() const { return c_up;}
	bool getCollidingDown() const { return c_down;}
    bool getInTheAir()  const { return m_InTheAir; }
	
	// Inputs 
    bool getJoystickPressed(int& btn_indx) const; 
	bool getKeyboardKey(std::string keyname) const;

    // Set 
    void setPosition(float deltaTime, std::vector<Plateform>& Pf, short unsigned int& cloop);
    void setInTheAir(TileMap& Tm);
    
    //
    bool isAllowJumping() const { return m_CanJump;}

    // Animation 
    void Play(AnimName anim_name, float deltaTime);
    void Pause();
    void Stop();
    void setFrameTexture(AnimName anim_name, float deltaTime);
    void resetFrameCounter() {a_framecount = 0; a_framecounttexture= 0; };
    void setCurrentAnim(AnimName anim_name) { m_current_anim = anim_name; };
    bool getPlaying() const { return is_PlayingAnim; };
    void setPlaying(const bool& status) { is_PlayingAnim = status; };
    std::string getAnimName(); 


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
    bool m_IsPlayingEndGame;

    // in the air vs on the floor 
    bool m_InTheAir; 

    // Jumping
    bool m_CanJump; 
    short unsigned int m_nbjumps{ 0 };

    // max move window 
    sf::Vector2f WIN_LIMIT_X;
    sf::Vector2f WIN_LIMIT_Y;
	
	// test 
	bool _colliding_plateforms{false}; 
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
    AnimName m_current_anim = AnimName::Idle; 
    bool is_PlayingAnim{ false };
    // Facing direction
    bool direction{ false }; // true: right, false: left 
    void setFacingDirection(); 
    
    
};