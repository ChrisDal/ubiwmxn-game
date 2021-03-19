#include <stdafx.h>
#include <Game/MainCharacter.h>

using namespace sf;

// Joystick helpers
namespace
{
    bool GetFirstJoystickIndex(unsigned int& index)
    {
        index = 0;
        while (index < Joystick::Count)
        {
            if (Joystick::isConnected(index) && Joystick::hasAxis(index, Joystick::Axis::X) && Joystick::hasAxis(index, Joystick::Axis::Y))
                return true;

            index++;
        }

        return false;
    }

    float GetScaledAxis(unsigned int index, sf::Joystick::Axis axis, float deadZone, float scale)
    {
        float value = (Joystick::getAxisPosition(index, axis) / 100.0f) * scale;
        if (value >= -deadZone && value <= deadZone)
            return 0.0f;

        return value;
    }
}


MainCharacter::MainCharacter(sf::Vector2u WIN_LIMITS)
    : m_IsPlayingEndGame(false), m_Position(160.0f, 672.0f), m_IsUsingJoystick(false), m_JoystickIndex(0), m_WasButtonPressed(false), 
    c_left(false), c_right(false), c_up(false), c_down(false),  m_InTheAir(true), m_CanJump(true)
{

    m_Texture.loadFromFile(".\\Assets\\bulle.png");

    const sf::Vector2f size(static_cast<float>(m_Texture.getSize().x), static_cast<float>(m_Texture.getSize().y));

    m_Sprite.setTexture(m_Texture);
    m_Sprite.setOrigin(size * 0.5f);
    m_Sprite.setPosition(m_Position);

    SetBoundingBox(m_Position, size);

    m_IsUsingJoystick = GetFirstJoystickIndex(m_JoystickIndex);

    // sf vectors for min and max of windows app 
    WIN_LIMIT_X.x = 0;
    WIN_LIMIT_X.y = WIN_LIMITS.x;
    WIN_LIMIT_Y.x = 0;
    WIN_LIMIT_Y.y = WIN_LIMITS.y;

}


void MainCharacter::Update(float deltaTime, std::vector<Plateform>& Pf, TileMap& Tm)
{
    if (m_IsPlayingEndGame)
    {
        return;
    }

    const float SPEED_MAX = 150.0f;
    const float SPEED_MAX_FALL = 1000.0f;

    const float GRAVITY = 98.1f;
    const float NO_GRAVITY = 0.0f;
    const float JUMP_HEIGHT = 32.f*13.0f*10.0f;
    const short unsigned int NB_MAX_JUMPS = 3; 

    const float SPEED_INC = 10.0f;
    const float DEAD_ZONE = 5.0f;
    const float SLOWDOWN_RATE = 0.5f;

    // handling collision : new pos vs old pos 
    const sf::Vector2f old_Position = m_Position;
    
    // List = Right Left Up Down Space
    bool k_KeyboardPressed[5] = { false, false, false, false, false };
    bool k_JoystickPressed[8] = { false, false, false, false, false, false, false, false };

    // Sign Velocity : true pos, false neg 
    sf::Vector2<bool> s_Velocity = { false, false }; 

    // Define if in the air or not 
    setInTheAir(Tm);

    // in the Air or on plateforms
    if (m_InTheAir)
    {
        m_Velocity.y = fmin(m_Velocity.y + GRAVITY, SPEED_MAX_FALL);
    }
    else
    {
        m_Velocity.y = NO_GRAVITY;
        m_nbjumps = 0;
        m_CanJump = true;
    }


    // Velocity and Jumping determination 
    if (m_IsUsingJoystick)
    {
        m_Velocity.x = GetScaledAxis(m_JoystickIndex, Joystick::Axis::X, DEAD_ZONE, SPEED_MAX);
        s_Velocity.x = (m_Velocity.x >= 0.0f); 
        // Joystick Index 0 = A, 1 = B , 2 = X , 3 = Y 
        if (Joystick::isButtonPressed(m_JoystickIndex, 0))
        {
            if (!m_WasButtonPressed)
            {
                m_WasButtonPressed = true;
                k_JoystickPressed[0] = true; 
                 
                // Jumping 
                if (m_nbjumps < NB_MAX_JUMPS)
                {
                    m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT);
                    m_InTheAir = true;
                    m_CanJump = true;
                    m_nbjumps++;
                }
                else
                {
                    m_CanJump = false;
                }
            }
            
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_Sprite.setScale(1.0f, 1.0f);
                m_WasButtonPressed = false;
                k_JoystickPressed[0] = false; 
            }
        }

        s_Velocity.y = (m_Velocity.y >= 0.0f);
    }
    else
    {
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            m_Velocity.x = fmin(m_Velocity.x + SPEED_INC, SPEED_MAX);
            k_KeyboardPressed[0] = true;
            s_Velocity.x = (m_Velocity.x >= 0.0f);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            m_Velocity.x = fmax(m_Velocity.x - SPEED_INC, -SPEED_MAX);
            k_KeyboardPressed[1] = true;
            s_Velocity.x = (m_Velocity.x >= 0.0f);
        }
        else
        {
            // if we were going left modify velocity
            m_Velocity.x = std::abs(m_Velocity.x * SLOWDOWN_RATE);
            if (s_Velocity.x)
            {
                m_Velocity = -m_Velocity;
            }

            
            k_KeyboardPressed[0] = false;
            k_KeyboardPressed[1] = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            k_KeyboardPressed[2] = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            k_KeyboardPressed[3] = true;
        }
        else
        {
            k_KeyboardPressed[2] = false;
            k_KeyboardPressed[3] = false;
        }

        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            if (!m_WasButtonPressed)
            {
                m_WasButtonPressed = true;
                k_KeyboardPressed[4] = true;

                // Jumping 
                if (m_nbjumps < NB_MAX_JUMPS)
                {
                    m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT);
                    m_InTheAir = true;
                    m_CanJump = true;
                    m_nbjumps++;
                }
                else
                {
                    m_CanJump = false;
                }
            }
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_Sprite.setScale(1.0f, 1.0f);
                m_WasButtonPressed = false;
                k_KeyboardPressed[4] = false;
            }
        }
    }


    // Test collision with new Position 
    short unsigned int colliding_loop = 0; 
    setPosition(deltaTime, Pf, colliding_loop);

    m_Sprite.setPosition(m_Position);
    SetCenter(m_Position);
}




void MainCharacter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void MainCharacter::StartEndGame()
{
    m_IsPlayingEndGame = true;
}


void MainCharacter::setInTheAir(TileMap& Tm)
{
    // Left and right down corners
    sf::Vector2f left_feet = m_Position  + sf::Vector2f(-float(m_BoundingBox.width)/2.0f, float(m_BoundingBox.height)/2.0f); 
    sf::Vector2f right_feet = m_Position + sf::Vector2f(+float(m_BoundingBox.width)/2.0f, float(m_BoundingBox.height)/2.0f); 
    
    // Delta pixels
    const float D_PIXELS = 2.0f; 

    // All right and left tiles are walkable 
    sf::Vector2f nl_feet = left_feet  + sf::Vector2f(0.0f, D_PIXELS);
    sf::Vector2f nr_feet = right_feet + sf::Vector2f(0.0f, D_PIXELS);

       
    // In the air if down neighboors tiles are walkable 
    if (Tm.walkable_tile(nl_feet) && Tm.walkable_tile(nr_feet))
    {
        m_InTheAir = true; 
    }
    else 
    {
        m_InTheAir = false;
    }


}

void MainCharacter::setPosition(float deltaTime, std::vector<Plateform>& Pf, short unsigned int& cloop)
{   

    while (cloop < 20)
    {
        sf::Vector2f new_Position = m_Position;
        // new position calculations 
        new_Position += m_Velocity * deltaTime;

        m_Sprite.setPosition(new_Position);
        SetCenter(new_Position);

        // Colliding Check
        _colliding_plateforms = false;
        isCollidingSolid(new_Position, Pf, _colliding_plateforms);

        if (!_colliding_plateforms)
        {
            m_Position = new_Position;
            break;
        }
        else
        {
            new_Position = m_Position;
            m_Velocity.y *= 0.9f;
            cloop++;
            setPosition(deltaTime, Pf, cloop);
            
        }
        
    } 
}

void MainCharacter::isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf, bool& colliding)
{
 
    bool k_left = (m_Velocity.x < 0) and (std::abs(m_Velocity.x) != 0.0f);
    bool k_right = (m_Velocity.x > 0) and (std::abs(m_Velocity.x) != 0.0f);
    bool k_up = (m_Velocity.y < 0) and (std::abs(m_Velocity.y) != 0.0f);
    bool k_down = (m_Velocity.y > 0) and (std::abs(m_Velocity.y) != 0.0f);



    for (const Plateform& pfmi : Pf)
    {
        if (this->IsColliding(pfmi)) {
            _colliding_plateforms = true;
            
            // determine collision side character referentiel
            c_left  = isCollidingLeft(pfmi, k_left);
            c_right = isCollidingRight(pfmi, k_right);
            c_up    = isCollidingUp(pfmi, k_up);
            c_down  = isCollidingDown(pfmi, k_down);
            
            m_InTheAir = false;
            // If collision stops velocity on the direction
            if ((c_left and (m_Velocity.x < 0.0f)) or (c_right and (m_Velocity.x > 0.0f)))
            {
                m_Velocity.x = 0.0f;
            }
            
            // corners bumping against plateforms
            if ((c_left and c_down) or (c_right and c_up))
            {
                m_InTheAir = true;
                 
            }
            

        }
    };

    if (!_colliding_plateforms)
    {
        c_left = false; 
        c_right = false;
        c_up = false;
        c_down = false;
    }

    // Handling out window 
    if (newpos.x < WIN_LIMIT_X.x or newpos.x > WIN_LIMIT_X.y) {
        _colliding_plateforms = true;
    }
    if (newpos.y < WIN_LIMIT_Y.x or newpos.y > WIN_LIMIT_Y.y) {
        _colliding_plateforms = true;
    }

};

sf::Vector2f MainCharacter::getVelocity() const
{
    return MainCharacter::m_Velocity;
}


bool MainCharacter::getJoystickPressed(int& btn_indx) const
{

    if (Joystick::isButtonPressed(m_JoystickIndex, btn_indx))
    {
        return true; 
    }
    else 
    {
        return false; 
    }

}


bool MainCharacter::getKeyboardKey(std::string keyname) const
{
	bool value = false; 
    if (keyname == "Up") {
        value = Keyboard::isKeyPressed(Keyboard::Up);
    }
    else if (keyname == "Down") {
        value = Keyboard::isKeyPressed(Keyboard::Down);
    }
    else if (keyname == "Left") {
        value = Keyboard::isKeyPressed(Keyboard::Left);
    }
    else if (keyname == "Right") {
        value = Keyboard::isKeyPressed(Keyboard::Right);
    }
    else if (keyname == "Space") {
        value = Keyboard::isKeyPressed(Keyboard::Space);
    }
    else {
        value = false; 
    }
	
	return value; 

}


bool MainCharacter::isCollidingLeft(const BoxCollideable& other, bool keypressed) const
{
	
	bool leftup = other.Contains(m_BoundingBox.left, m_BoundingBox.top);
	bool leftdown = other.Contains(m_BoundingBox.left, m_BoundingBox.top + m_BoundingBox.height);

	return (leftup or leftdown) and keypressed;

}

bool MainCharacter::isCollidingRight(const BoxCollideable& other, bool keypressed) const
{

    bool righttup = other.Contains(m_BoundingBox.left + m_BoundingBox.width, m_BoundingBox.top);
    bool rightdown = other.Contains(m_BoundingBox.left + m_BoundingBox.width, m_BoundingBox.top + m_BoundingBox.height);

    return (righttup or rightdown) and keypressed;

}

bool MainCharacter::isCollidingUp(const BoxCollideable& other, bool keypressed) const
{

    bool leftup = other.Contains(m_BoundingBox.left, m_BoundingBox.top);
    bool rightup = other.Contains(m_BoundingBox.left + m_BoundingBox.width, m_BoundingBox.top);

    return (leftup or rightup) and keypressed;

}

bool MainCharacter::isCollidingDown(const BoxCollideable& other, bool keypressed) const
{

    bool leftdown = other.Contains(m_BoundingBox.left , m_BoundingBox.top + m_BoundingBox.width);
    bool rightdown = other.Contains(m_BoundingBox.left + m_BoundingBox.width, m_BoundingBox.top + m_BoundingBox.width);

    return (leftdown or rightdown) and keypressed;

}