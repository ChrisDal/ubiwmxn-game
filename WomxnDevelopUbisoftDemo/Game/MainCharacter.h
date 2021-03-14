#pragma once
#include <Game/TileMap.h>
#include <Game/Plateform.h> 

class MainCharacter : public sf::Drawable, public BoxCollideable
{
public:	
    MainCharacter(sf::Vector2u WIN_LIMITS);

    void Update(float deltaTime, std::vector<Plateform> &Pf, TileMap& Tm);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    
    sf::Vector2f getVelocity() const; 
	bool getCollidingPf() const { return _colliding_plateforms;}
	bool isCollidingLeft(const BoxCollideable& other, const bool k_keyboard[5]) const;
	bool isCollidingRight(const BoxCollideable& other, const bool k_keyboard[5]) const;
	bool isCollidingUp(const BoxCollideable& other, const bool k_keyboard[5]) const;
	bool isCollidingDown(const BoxCollideable& other, const bool k_keyboard[5]) const;
    void isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf, bool& colliding);
    
    //  get

	bool getCollidingLeft() const { return c_left;}
	bool getCollidingRight() const { return c_right;}
	bool getCollidingUp() const { return c_up;}
	bool getCollidingDown() const { return c_down;}
    bool getInTheAir()  const { return m_InTheAir; }

    void SetPosition(float deltaTime, std::vector<Plateform>& Pf, short unsigned int& cloop);
    void setInTheAir(TileMap& Tm);


private:
    sf::Texture m_Texture;
    sf::Sprite m_Sprite;

    bool m_IsUsingJoystick;
    unsigned int m_JoystickIndex;
    bool m_WasButtonPressed;

    sf::Vector2f m_Position; 
    sf::Vector2f m_Velocity;
    bool m_IsPlayingEndGame;

    // in the air vs on the floor 
    bool m_InTheAir; 

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

    
    
};