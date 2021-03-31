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

// constructor
MainCharacter::MainCharacter(sf::Vector2u WIN_LIMITS, sf::Vector2f spawn_position)
    : m_IsPlayingEndGame(false), m_IsUsingJoystick(false), m_JoystickIndex(0), m_WasButtonPressed(false), 
    c_left(false), c_right(false), c_up(false), c_down(false),  m_InTheAir(true), m_InTheWater(false), 
    m_InTheVoid(false), m_CanJump(true), a_textsquare_offset(12,22)
{

    m_Texture.loadFromFile(".\\Assets\\hero\\cat_addon_sprite.png");
    m_Sprite.setTexture(m_Texture);
    m_Sprite.setTextureRect(sf::IntRect(a_textsquare_offset.x, a_textsquare_offset.y, 32, 32));
    //const sf::Vector2f size(static_cast<float>(m_Texture.getSize().x), static_cast<float>(m_Texture.getSize().y));
    const sf::Vector2f size(static_cast<float>(32), static_cast<float>(32));


    m_Sprite.setOrigin(size * 0.5f);
    m_Position = spawn_position; 
    m_Sprite.setPosition(m_Position);
    //m_Sprite.setScale(2.0f, 2.0f);

    SetBoundingBox(m_Position, sf::Vector2f(0.5625*m_Sprite.getScale().x*size.x, 0.9375 * m_Sprite.getScale().y * size.y));

    m_IsUsingJoystick = GetFirstJoystickIndex(m_JoystickIndex);

    // sf vectors for min and max of windows app 
    WIN_LIMIT_X.x = 0.0f;
    WIN_LIMIT_X.y = (float)WIN_LIMITS.x;
    WIN_LIMIT_Y.x = 0.0f;
    WIN_LIMIT_Y.y = (float)WIN_LIMITS.y;

    // Reborn init 
    m_RespawnPosition = m_Position; 
    // Animations structures and mapping
    InitAnimType();             // ToDo : make a configuration files for animation's details
   
}


void MainCharacter::Update(float deltaTime, std::vector<Plateform>& Pf, TileMap& Tm, std::vector<Ennemie>& l_ennemie)
{
    if (m_IsPlayingEndGame)
    {
        return;
    }

    if (!m_isAlive and not m_Respawning and a_done_anim)
    {
        printf("Is Dying ");
        // call reborn 
        m_Velocity = {0.0f, 0.0f}; 
		m_Respawning = true; 
        // create dead bodies 
        bool solid_dbody = not (m_InTheWater or m_InTheAir); 
		m_deadbodies.push_back(DeadBody(m_Position, 32, 32, solid_dbody));
			
		// assign position to respawn spot 
		m_Position = m_RespawnPosition;


    } 
	else if (m_Respawning)
	{
        printf("Is Respawing ");
		Play(AnimName::Reborn, deltaTime, false); 
		if (a_done_anim)
		{
			m_Respawning = false; 
		}
        return;
	}


    // Define if in the air or not 
    setInElements(Tm);

    // Alive or not 
    bool living = Alive(deltaTime, l_ennemie);
    if (not living)
    {
        // Launch dies 
        Play(AnimName::Die, deltaTime, false);
        return;
    }
		


    const float SPEED_MAX = 150.0f;
    const float WATER_SPEED_MAX = SPEED_MAX * 0.3f;
    const float SPEED_MAX_FALL = 800.0f;
    const float WATER_SPEED_MAX_FALL = SPEED_MAX_FALL / 8.0f;

    const float APPLIED_FACTOR = 0.6f;
    const float APPLIED_FALL_FACTOR = APPLIED_FACTOR * 0.82f;
    const float APPLIED_WATER_FACTOR = APPLIED_FACTOR * 0.15f;
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

    // in the Air or on plateforms
    if (m_isWalkable and ( m_InTheAir || m_InTheVoid ))
    {
        m_Velocity.y = fmin(m_Velocity.y + (GRAVITY * APPLIED_FALL_FACTOR), SPEED_MAX_FALL);
    }
    else if (m_InTheWater and m_isWalkable)
    {
        m_Velocity.y = fmin(m_Velocity.y + (GRAVITY * APPLIED_WATER_FACTOR), WATER_SPEED_MAX_FALL);
    }
    else
    {
        m_Velocity.y = NO_GRAVITY;
        m_nbjumps = 0;
    }
	
	// on deadbodies = plateform  
	if (_colliding_deadbodies)
	{
        printf("Colliding dead ");
        m_Velocity.y = NO_GRAVITY;
		m_nbjumps = 0;
        if (m_nbjumps >= NB_MAX_JUMPS)
        {
            printf("Colliding dead but can no jump ");
        }
	}

    // determine if can jump or not 
    m_CanJump = m_nbjumps < NB_MAX_JUMPS;

    // Velocity and Jumping determination 
    if (m_IsUsingJoystick)
    {

        if (m_InTheWater)
        {
            m_Velocity.x = GetScaledAxis(m_JoystickIndex, Joystick::Axis::X, DEAD_ZONE, WATER_SPEED_MAX);
        }
        else
        {
            m_Velocity.x = GetScaledAxis(m_JoystickIndex, Joystick::Axis::X, DEAD_ZONE, SPEED_MAX);
        }
        
        setFacingDirection();

        // Joystick Index 0 = A, 1 = B , 2 = X , 3 = Y 
        if (Joystick::isButtonPressed(m_JoystickIndex, 0))
        {
            if (!m_WasButtonPressed)
            {
                m_WasButtonPressed = true;
                k_JoystickPressed[0] = true; 

                if (m_CanJump)
                {
                    if (!m_InTheWater)
                    {
                        m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT * APPLIED_FACTOR);
                        m_InTheAir = true;
                        m_IsJumping = true;
                        m_nbjumps++;
                    }
                    else if (m_InTheWater)
                    {
                        m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT * APPLIED_WATER_FACTOR);
                        m_IsJumping = true;
                        m_nbjumps++;
                    }
                    else
                    {
                        m_IsJumping = false;
                    }
                }
                else
                {
                    m_IsJumping = false;
                }

            }
            
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_WasButtonPressed = false;
                k_JoystickPressed[0] = false; 
                m_IsJumping = false;
            }
        }
    }
    else
    {
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            if (m_InTheWater)
			{
				m_Velocity.x = fmin(m_Velocity.x + SPEED_INC, WATER_SPEED_MAX);
			}
			else 
			{
				m_Velocity.x = fmin(m_Velocity.x + SPEED_INC, SPEED_MAX);
			}
			
            k_KeyboardPressed[0] = true;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left))
        {
			if (m_InTheWater)
			{
				m_Velocity.x = fmax(m_Velocity.x - SPEED_INC, -WATER_SPEED_MAX);
			}
			else
			{
				m_Velocity.x = fmax(m_Velocity.x - SPEED_INC, -SPEED_MAX);
			}
            
			k_KeyboardPressed[1] = true;
        }
        else
        {
            // if we were going left modify velocity
            m_Velocity.x = std::abs(m_Velocity.x * SLOWDOWN_RATE);
            if (s_Velocity.x)
            {
                m_Velocity.x = -m_Velocity.x;
            }

            
            k_KeyboardPressed[0] = false;
            k_KeyboardPressed[1] = false;
        }
		
		// facing direction
        setFacingDirection();

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
                if (m_CanJump)
                {
					if (!m_InTheWater)
					{
						m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT * APPLIED_FACTOR);
						
					}
					else 
					{
						m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT * APPLIED_WATER_FACTOR);
					}
					
					m_IsJumping = true;
					m_nbjumps++;

                }
                else
                {
                    m_IsJumping = false;
                }
            }
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_WasButtonPressed = false;
                k_KeyboardPressed[4] = false;
                m_IsJumping = false;
            }
        }

    }


    // Test collision with new Position 
    short unsigned int colliding_loop = 0; 
    setPosition(deltaTime, Pf, colliding_loop);
	// Set New position
    m_Sprite.setPosition(m_Position);
    SetCenter(m_Position);
	
	s_Velocity.x = (m_Velocity.x >= 0.0f);
	s_Velocity.y = (m_Velocity.y >= 0.0f);
	

	// Animation to play
	if (m_IsJumping)
	{
		if (m_nbjumps == 1)
		{
			Play(AnimName::Jump, deltaTime, true);
		}
		else
		{
			Play(AnimName::DoubleJump, deltaTime, true);
		}
	}
	else
	{
		if ((std::abs(m_Velocity.y) == 0.0f) and (std::abs(m_Velocity.x) == 0.0f))
		{
			Play(AnimName::Idle, deltaTime, true);
		}
		// moving left right 
		else if ((std::abs(m_Velocity.y) == 0.0f) and (std::abs(m_Velocity.x) != 0.0f))
		{
			Play(AnimName::Walk, deltaTime, true);
		}
	}
	
}




void MainCharacter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);

    for (const auto& dbd : m_deadbodies)
    {
        target.draw(dbd);

    }
}

void MainCharacter::StartEndGame()
{
    m_IsPlayingEndGame = true;
}


void MainCharacter::setInElements(TileMap& Tm)
{
    // Left and right down corners
    sf::Vector2f left_feet = m_Position  + sf::Vector2f(-float(m_BoundingBox.width)/2.0f, float(m_BoundingBox.height)/2.0f); 
    sf::Vector2f right_feet = m_Position + sf::Vector2f(+float(m_BoundingBox.width)/2.0f, float(m_BoundingBox.height)/2.0f); 
    
    // Delta pixels
    const float D_PIXELS = 2.0f; 

    // All right and left tiles are walkable 
    sf::Vector2f nl_feet = left_feet  + sf::Vector2f(0.0f, D_PIXELS);
    sf::Vector2f nr_feet = right_feet + sf::Vector2f(0.0f, D_PIXELS);

       
    // Can pass through the tile if true
    if (Tm.walkable_tile(nl_feet) && Tm.walkable_tile(nr_feet))
    {
        m_isWalkable = true; 
    }
    else 
    {
        m_isWalkable = false;
    }

    // reel type 
    if (Tm.ElementsTiles(nl_feet) == Tm.ElementsTiles(nr_feet))
    {
        switch (Tm.ElementsTiles(nl_feet))
        {
        case(0): 
            m_InTheAir = true;
            m_InTheWater = false;
            m_InTheLava  = false;
            m_InTheVoid  = false; 
            break;        
        case(1): 
            m_InTheAir = false;
            m_InTheWater = true; 
            m_InTheLava  = false;
            m_InTheVoid  = false; 
            break;
        case(2):
            m_InTheAir = false;
            m_InTheWater = false;
            m_InTheLava  = true;
            m_InTheVoid  = false;
            break;
        case(3): 
            m_InTheAir = false;
            m_InTheWater = false;
            m_InTheLava  = false;
            m_InTheVoid  = true;
            break;
        default:
            break;
        }
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
        _colliding_deadbodies = false;
        isCollidingSolid(new_Position, Pf);

        if ((!_colliding_plateforms) and (!_colliding_deadbodies))
        {
            m_Position = new_Position;
            break;
        }
		else if (!m_isWalkable and _colliding_deadbodies)
		{
			m_Velocity.y = 0.0f;
			cloop++;
            setPosition(deltaTime, Pf, cloop);
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

// colliding plateforms and dead bodies 
void MainCharacter::isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf)
{
 
    bool k_left = (m_Velocity.x < 0) and (std::abs(m_Velocity.x) != 0.0f);
    bool k_right = (m_Velocity.x > 0) and (std::abs(m_Velocity.x) != 0.0f);
    bool k_up = (m_Velocity.y < 0) and (std::abs(m_Velocity.y) != 0.0f);
    bool k_down = (m_Velocity.y > 0) and (std::abs(m_Velocity.y) != 0.0f);

    // Test all Plateforms = Plateforms + Dead bodies 
    for (const Plateform& pfmi : Pf)
    {
        if (this->IsColliding(pfmi)) {
            _colliding_plateforms = true;
            
            // determine collision side character referentiel
            c_left  = isCollidingLeft(pfmi, k_left);
            c_right = isCollidingRight(pfmi, k_right);
            c_up    = isCollidingUp(pfmi, k_up);
            c_down  = isCollidingDown(pfmi, k_down);
            
            m_isWalkable = false;
            m_InTheAir = false;
            // If collision stops velocity on the direction
            bool collided_pf_left = c_left and not s_Velocity.x;
            bool collided_pf_right = c_right and s_Velocity.x;
            
            // corners bumping against plateforms
            if ((c_left) or (c_right))
            {
                m_isWalkable = true;
                m_InTheAir = true;

                if (collided_pf_left or collided_pf_right)
                {
                    m_Velocity.x = 0.0f;
                }

                 
            }
            

        }
    };
	
    Plateform* p_pfmi = nullptr; 
    for (DeadBody& dbd : m_deadbodies)
    {
        p_pfmi = dbd.get_Plateform();
        if (p_pfmi == nullptr)
        {
            continue; 
        }

        if (this->IsColliding(*p_pfmi))
        {
            _colliding_deadbodies = true;
            m_isWalkable = false;
            m_InTheAir = false;
        }

    }

    if ((!_colliding_plateforms) and (!_colliding_deadbodies))
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

// ----------------------------------------
//          Colliding 
// ----------------------------------------

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


//  ---------------------------------
//              Animation 
// ----------------------------------

// 
void MainCharacter::Play(AnimName anim_name, float deltaTime, bool loop)
{
    
    if (m_current_anim != anim_name)
    {
        Stop(); // reset counters
    }
    // play once
    if (not loop)
    {
        // Get next Anim
        if (a_framecounttexture == (dictAnim[anim_name].nb_frames_anim-1))
        {
            Pause(); 
            a_done_anim = true; 
            setPlaying(false);
            Stop();
            return; 
        }

        if (not a_done_anim)
        {
            // Update frame texture 
            setFrameTexture(anim_name, deltaTime);
            // set current
            setCurrentAnim(anim_name);
            setPlaying(true);

        }

    } 
    else 
    {
        // Update frame texture 
        setFrameTexture(anim_name, deltaTime);
        // set current
        setCurrentAnim(anim_name);
        setPlaying(true);
    }
    

    

}

void MainCharacter::Pause() 
{
    // reste sur la meme frame 
    setPlaying(false);
};


void MainCharacter::Stop() {
    // Reset counters
    setPlaying(false);
    a_framecount = 0; 
    a_framecounttexture = 0;
	sumdeltaTime = 0.0f;
};


void MainCharacter::InitAnimType()
{
    m_AllAnims.Idle = { 4, 0, 0, "Idle" };
    m_AllAnims.Walk = { 8, 1, 0, "Walk" };
    m_AllAnims.Jump = { 8, 2, 0, "Jump" };
    m_AllAnims.DoubleJump = { 6, 2, 2, "DoubleJump" };
    m_AllAnims.Die = { 7, 4, 0, "Die" };
    m_AllAnims.Hurt = { 2, 4, 1, "Hurt" };
    m_AllAnims.Dodge = { 6, 14, 0, "Dodge" };
    m_AllAnims.Surprise = { 6, 14, 0, "Surprise" };
    m_AllAnims.Reborn = { 4, 4, 7, "Reborn" };

    dictAnim[AnimName::Idle]        = m_AllAnims.Idle;
    dictAnim[AnimName::Walk]        = m_AllAnims.Walk;
    dictAnim[AnimName::Jump]        = m_AllAnims.Jump;
    dictAnim[AnimName::DoubleJump]  = m_AllAnims.DoubleJump;
    dictAnim[AnimName::Die]         = m_AllAnims.Die;
    dictAnim[AnimName::Dodge]       = m_AllAnims.Dodge;
    dictAnim[AnimName::Surprise]    = m_AllAnims.Surprise;
    dictAnim[AnimName::Reborn]      = m_AllAnims.Reborn;

}

void MainCharacter::setFrameTexture(AnimName anim_name, float deltaTime)
{
    short unsigned int nb_frames_anim=1; 
    short unsigned int line_anim=0;
    short unsigned int a_offset = 0; // offset frame pour l'animation si besoin
    const sf::Vector2i sizetexture = {64,64};
	
    switch (anim_name)
    {
        case AnimName::Idle:      
            nb_frames_anim = 4;
            line_anim = 0; 
            a_offset = 0;
            break;
        case AnimName::Walk:      
            nb_frames_anim = 8;
            line_anim = 1; 
            a_offset = 0;
            break;
        case AnimName::Jump:       
            nb_frames_anim = 8;
            line_anim = 2; 
            a_offset = 0;
            break;
        case AnimName::DoubleJump : 
            nb_frames_anim = 6;
            line_anim = 2; 
            a_offset = 2;
            break;
        case AnimName::Die:
            nb_frames_anim = 7;
            line_anim = 4;
            a_offset = 0;
            break;
        case AnimName::Attack: 
            nb_frames_anim = 6;
            line_anim = 15; 
            a_offset = 0;
            break;
        case AnimName::Hurt: 
            nb_frames_anim = 2;
            line_anim = 4;
            a_offset = 1;
            break;
        case AnimName::Dodge: 
            nb_frames_anim = 6;
            line_anim = 14; 
            a_offset = 0;
            break;
        case AnimName::Surprise: 
            nb_frames_anim = 6;
            line_anim = 14; 
            a_offset = 0;
            break;
        case AnimName::Reborn: 
            nb_frames_anim = 4;
            line_anim = 4;
            a_offset = 7;
            break;

        default: 
            nb_frames_anim = 4;
            line_anim = 4;
            a_offset = 0;
            break;
    }


    // DT = 1/ 60.0 APP FRAMERATE 
	sumdeltaTime += deltaTime;

    // si on dépasse 1/framerate(anim) on change de frame  
    if (sumdeltaTime > a_spi)
    {
        // set texture 
        int y = line_anim * sizetexture.y ;
        int x = (a_framecounttexture % (nb_frames_anim)) * sizetexture.x + ( a_offset * sizetexture.x);
        int leftrect = x + a_textsquare_offset.x;
        int sizex = sizetexture.x / 2;
        
        if (!a_direction)
        {
            leftrect = x + sizetexture.x - a_textsquare_offset.x;
            sizex = - sizetexture.x / 2;
        }
        
        m_Sprite.setTextureRect(sf::IntRect(leftrect, y + a_textsquare_offset.y, sizex, sizetexture.y/2));
        // ++ number of frame in this specific animation modulo nb frames anim 
        a_framecounttexture++;
		sumdeltaTime = 0.0f; 
    }


    a_framecount++;
}



std::string MainCharacter::getAnimName()
{

    std::string animname = "No anim"; 

    if (dictAnim.find(m_current_anim) != dictAnim.end())
    {
        animname = dictAnim[m_current_anim].name;
    }
    

    return animname;
}


void MainCharacter::setFacingDirection()
{
    // direction for animation 
    if (m_Velocity.x < -0.01f)
    {
        a_direction = false;
    }
    else if (m_Velocity.x > 0.01f)
    {
        a_direction = true;
    }
    // else ==0.0f : relies on previous value

}


// ----------------------------------------------
//                  Gameplay 
// ----------------------------------------------

// Timer for time consecutively spent in elements tiles
bool MainCharacter::TimerElements(float deltaTime, bool& inelement_flag, const float& limit, float& element_timer)
{   
    bool limit_reaches = false; 

    if (inelement_flag)
    {
        element_timer += deltaTime; 

        if (element_timer > limit)
        {
            element_timer = 0.0f;
            inelement_flag = false;
            limit_reaches = true;
        }   
    }
    else 
    {
        element_timer = 0.0f;
    }

    return limit_reaches;
}

// Set Alive or Dead
bool MainCharacter::Alive(float deltaTime, std::vector<Ennemie> l_ennemies)
{
    const float TIMER_DEAD_WATER = 2.0f; 
    const float TIMER_DEAD_VOID  = 0.5f; 
    
    if (m_Respawning)
    {
        setAliveOrDead(true);
        return getAlive();
    }
    
    // update elements counters
    m_DiedInWater = TimerElements(deltaTime, m_InTheWater, TIMER_DEAD_WATER, m_CounterWater);
    m_DiedInVoid  = TimerElements(deltaTime, m_InTheVoid , TIMER_DEAD_VOID , m_CounterVoid );
    
    if (m_DiedInWater or m_DiedInVoid)
    {
        setAliveOrDead(false);
        return getAlive();
    }


    // check at each frame if it collides against ennemies 
    for (auto const& enm : l_ennemies)
    {

       
        if (IsColliding(enm) and m_isAlive)
        {	// Die 
            setAliveOrDead(false);
            printf("Dieee");
            break;
        }
    }


    return getAlive();
}
	


