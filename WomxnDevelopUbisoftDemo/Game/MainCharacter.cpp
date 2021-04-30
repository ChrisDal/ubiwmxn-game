#include <iostream> 
#include <stdafx.h>
#include <Game/MainCharacter.h>
#include <Game/Ennemie.h> 

#define DEBUG 0 
#if DEBUG 
    #define LOG(x) std::cout << x  << " "
# else
    #define LOG(x)
#endif

using namespace sf;

float MainCharacter::m_SFX_volume = 18.0f;

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
        float rawValue = Joystick::getAxisPosition(index, axis);
        if (rawValue >= -deadZone && rawValue <= deadZone)
            return 0.0f;
        float value = (rawValue / 100.0f) * scale;
        return value;
    }
}



// constructor
MainCharacter::MainCharacter(sf::Vector2u WIN_LIMITS, sf::Vector2f spawn_position)
    : m_IsPlayingEndGame(false), m_IsPlayingEndLevel(false), 
        m_IsUsingJoystick(false), m_JoystickIndex(0), m_WasButtonPressed(false),
        c_left(false), c_right(false), c_up(false), c_down(false),  
        m_InTheAir(true), m_InTheWater(false),  m_InTheVoid(false), 
        m_CanJump(true), a_textsquare_offset(12,22)
{
    // Texture
    m_Texture.loadFromFile(".\\Assets\\hero\\cat_addon_sprite.png");
    const sf::Vector2f size(32.0f, 32.0f);
    m_Sprite.setTexture(m_Texture);
    m_Sprite.setTextureRect(sf::IntRect(a_textsquare_offset.x, a_textsquare_offset.y, 
                                        static_cast<int>(size.x), static_cast<int>(size.y)));
    
    // Origin
    m_Sprite.setOrigin(size * 0.5f);
    // Position
    m_Position = spawn_position; 
    m_Sprite.setPosition(m_Position);
    // Bounding Box
    SetBoundingBox(m_Position, sf::Vector2f(0.5625 * static_cast<float>(m_Sprite.getScale().x) * size.x,
                                            0.9375 * static_cast<float>(m_Sprite.getScale().y) * size.y));

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
    InitSoundType();
    // Visual Effects
    m_vfx = VFX(m_Position, false);
}


// Set Up MainCharacter and deadbodies for next level 
void MainCharacter::MoveToNextLevel(sf::Vector2f spawn_position)
{
    // Movement 
    m_Position = spawn_position; 
    m_RespawnPosition = m_Position;
    m_Sprite.setPosition(m_Position);
    // Bounding box
    const sf::Vector2f size(32.0f, 32.0f);
    SetBoundingBox(m_Position, sf::Vector2f(0.5625 * static_cast<float>(m_Sprite.getScale().x) * size.x,
                                0.9375 * static_cast<float>(m_Sprite.getScale().y) * size.y));

    m_Velocity = { 0.0f, 0.0f };
    // FX
    m_vfx = VFX(m_Position, false);
    m_vfx.setParamVFX(VFX::AnimName::EmptyFrame, m_Position);

    // Deadbodies 
    m_deadbodies.clear(); 
    // Ennemies 
    _colliding_plateforms = false;
    _colliding_deadbodies = false;
    _colliding_cactus = false;
    m_HitByEnnemies = false;
    m_isAlive = true; 
    m_Respawning = false; 

    // Reset Jump 
    m_CanJump = true;
    ResetJumpCounter();
    // Reset Elements
    m_touched_lava = false; 
    m_timer_lava = false;
    ResetTimers(); 
    ResetElements();

    // End or not
    m_IsPlayingEndGame  = false; 
    m_IsPlayingEndLevel = false;
    m_WasButtonPressed  = false;

}

void MainCharacter::Update(float deltaTime, std::vector<Plateform>& Pf, TileMap& Tm, 
                            std::vector<Ennemie>& l_ennemie, std::vector<Ennemie>& l_cactus, 
                            std::vector<MovableEnnemies>& l_mennemies)
{
    if (m_IsPlayingEndLevel or m_IsPlayingEndGame)
    {
        return;
    }

    // Define if in the air or not 
    setInElements(Tm);

    // Alive and respawning process
    if (!m_isAlive and not m_Respawning and a_done_anim)
    {
        // call reborn 
		m_Respawning = true; 
		// Reset Animation flag ending
		a_done_anim = false;
        
        // create dead bodies : no_solid = go through
        // Solid : in the air or in the water 
        // No Solid : in the lava, in the void
        bool no_solid = m_DiedInLava or m_DiedInVoid;
        terrain::Element died_element; 
        if (m_DiedInLava)
        {
            died_element = terrain::Element::Lava;
        }
        else
        {
            died_element = m_current_elem;
        }
		m_deadbodies.push_back(DeadBody(m_Position, 32, 32, no_solid, died_element, a_direction, m_HitByEnnemies));
        DeathCounterAdd();
		// assign position to respawn spot 
		m_Position = m_RespawnPosition;
        m_Velocity = { 0.0f, 0.0f };

        // VFX
        m_vfx.setParamVFX(VFX::AnimName::Reborn, m_Position);

        // Reset Elements 
        ResetElements();

    } 
	else if (m_Respawning)
	{
		Play(AnimName::Reborn, deltaTime, false); 
        m_vfx.Update(deltaTime);
		if (a_done_anim)
		{
			m_Respawning = false; 

		}
        // reset counters 
        ResetTimers();
        // Reassign dead flags
        m_DiedInWater = false;
        m_DiedInVoid  = false;
        m_DiedInLava  = false;
        // Reset Lava switch 
        m_touched_lava = false;
        m_timer_lava = false;
        // Reset Ennemies hit 
        m_HitByEnnemies = false; 
        return;
	}


    // Alive or not 
    bool living = Alive(deltaTime, l_ennemie, l_mennemies);

    
    if (not living)
    {
        
		// Launch dies 
        if (m_DiedInLava or m_touched_lava)
        {
			Play(AnimName::FireEnd, deltaTime, false);
        }
        else
        {
			Play(AnimName::Die, deltaTime, false);
        }
        // VFX
        m_vfx.setParamVFX(VFX::AnimName::Death, m_Position);
        m_vfx.Update(deltaTime);

        if (m_soundfx.getStatus() == sf::SoundSource::Status::Stopped)
        {
            std::string death_sound = "Assets\\Sounds\\deaths\\cat_meow_0" + std::to_string(rand() % 5 + 1) + ".wav";
            // Assign both animation 
            LoadStructSound(m_AllSounds.Die, death_sound, false);
            dictSound[AnimName::Die] = m_AllSounds.Die;
            LoadStructSound(m_AllSounds.FireEnd, death_sound, false);
            dictSound[AnimName::FireEnd] = m_AllSounds.FireEnd;
            // set Buffer and play
            setSoundType(m_current_anim);
            playSFX(m_current_anim);
        }

        return;
    }
	else if (m_Respawning and living)
	{
		// No play just set new position
		m_Velocity = {0.0f, 0.0f};
		m_Sprite.setPosition(m_Position);
		SetCenter(m_Position);
        m_vfx.setPosition(m_Position);
		return; 
	}
	
	// Updating on Deadbodies 
    UpdateDeadBodies(deltaTime, Tm);


    // Handling Lava: One time call per life
    if (m_InTheLava and (not m_touched_lava))
    {
        // Keep information on went to lava
		m_touched_lava = true;
        // launch timer 
        m_timer_lava = true;
    }
    
    // touched deadbodies lava
    if (isCollidingDeadBodyLava())
    {
        // Keep information  "went to/ touched lava"
        m_touched_lava = true;
        // launch timer 
        m_timer_lava = true;
    }
    // Firefighter
    if (m_InTheWater and m_touched_lava)
    {
        m_timer_lava = false; 
        m_touched_lava = false; 
    }

    // Position and movement process calculations
    static const float SPEED_MAX = 150.0f;
    static const float WATER_SPEED_MAX = SPEED_MAX * 0.3f;
    static const float SPEED_MAX_FALL = 800.0f;
    static const float WATER_SPEED_MAX_FALL = SPEED_MAX_FALL / 8.0f;

    static const float APPLIED_FACTOR = 0.6f;
    static const float APPLIED_FALL_FACTOR = APPLIED_FACTOR * 0.82f;
    static const float APPLIED_WATER_FACTOR = APPLIED_FACTOR * 0.25f;
    static const float GRAVITY = 98.1f;
    static const float NO_GRAVITY = 0.0f;
    static const float JUMP_HEIGHT = 32.f*12.0f*10.0f;
    static const short unsigned int NB_MAX_JUMPS = 3;

    static const float SPEED_INC = 12.0f;
    static const float DEAD_ZONE = 25.0f;
    static const float SLOWDOWN_RATE = 0.5f;

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
        ResetJumpCounter();
    }
	

    // determine if can jump or not 
    m_CanJump = (m_nbjumps < NB_MAX_JUMPS);
    if (m_InTheWater)
    {
        m_CanJump = (m_nbjumps < 1);
    }

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
                        // Get Jumping Position
                        m_vfx.setPosition(m_Position);
                    }
                    else if (m_InTheWater)
                    {
                        m_Velocity.y = -sqrtf(2.0f * GRAVITY * JUMP_HEIGHT * APPLIED_WATER_FACTOR);
                        m_IsJumping = true;
                        m_nbjumps++;
                        // Get Jumping Position
                        m_vfx.setPosition(m_Position);
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
        s_Velocity.x = (m_Velocity.x >= 0.0f);
        s_Velocity.y = (m_Velocity.y >= 0.0f);
    }
    else
    {
        
        s_Velocity.x = (m_Velocity.x >= 0.0f);

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
            m_Velocity.x = 0.0f;
            
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
                    // Get Jumping Position
                    m_vfx.setPosition(m_Position);

                }
                else
                {
                    m_IsJumping = false;
                }
            }
            s_Velocity.y = (m_Velocity.y >= 0.0f);
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
    setPosition(deltaTime, Pf, l_cactus, colliding_loop);
	// Set New position
    m_Sprite.setPosition(m_Position);
    SetCenter(m_Position);
	
    // VFX Settings 
    m_vfx.setDirection(a_direction);

	// Animation to play
    if (m_InTheLava and not a_done_anim)
    {
        if (m_current_anim != AnimName::FireSet && m_current_anim != AnimName::FireEnd)
        {
            Play(AnimName::FireBegin, deltaTime, false);
            m_vfx.setParamVFX(VFX::AnimName::EmptyFrame, m_Position);
            
        }
        else if (m_current_anim == AnimName::FireSet)
        {
            Play(m_current_anim, deltaTime, true);
            m_vfx.setParamVFX(VFX::AnimName::EmptyFrame, m_Position);
        }
        // else nothing

        if (m_soundfx.getStatus() == sf::SoundSource::Status::Stopped)
        {
            // Sound when Anim is defined 
            setSoundType(m_current_anim);
            playSFX(m_current_anim);
        }

        if (m_IsJumping)
        {
            // VFX: jumping position already set
            m_vfx.setParamVFX(VFX::AnimName::DustJump);
        }


    }
    else if (m_touched_lava or m_InTheLava)
    {
        Play(AnimName::FireSet, deltaTime, true);  
        if (m_soundfx.getStatus() == sf::SoundSource::Status::Stopped)
        {
            // Sound when Anim is defined 
            setSoundType(m_current_anim);
            playSFX(m_current_anim);
        }

        if (m_IsJumping)
        {
            // VFX: jumping position already set
            m_vfx.setParamVFX(VFX::AnimName::DustJump);
        }

    }
	else if (m_IsJumping)
	{
        

		if (m_nbjumps == 1)
		{
			Play(AnimName::Jump, deltaTime, true);
		}
		else
		{
			Play(AnimName::DoubleJump, deltaTime, true);
		}

        // VFX 
        m_vfx.setParamVFX(VFX::AnimName::DustJump);

        // Sound when Anim is defined 
        setSoundType(m_current_anim);
        // Leap authorized = sound
        if (k_KeyboardPressed[4] or k_JoystickPressed[0])
            playSFX(m_current_anim);
	}
	else
	{
		if ((std::abs(m_Velocity.y) == 0.0f) and (std::abs(m_Velocity.x) == 0.0f))
		{
			Play(AnimName::Idle, deltaTime, true);
            m_vfx.setParamVFX(VFX::AnimName::EmptyFrame, m_Position);
		}
		// moving left right 
		else if ((std::abs(m_Velocity.y) == 0.0f) and (std::abs(m_Velocity.x) != 0.0f))
		{
			Play(AnimName::Walk, deltaTime, true);
            // VFX 
            sf::Vector2f dustpos = m_Position;
            if (a_direction) {
                dustpos -= sf::Vector2f(std::abs(m_Sprite.getTextureRect().width) / 2.0f, -2.0f);
            } else {
                dustpos += sf::Vector2f(std::abs(m_Sprite.getTextureRect().width) / 1.3f, 2.0f);
            }
            
            m_vfx.setParamVFX(VFX::AnimName::DustTrail, dustpos);
		}
	}



    if (m_soundfx.getStatus() == sf::SoundSource::Status::Stopped)
    {
        resetPlaying();
    }

    // Animate VFX 
    m_vfx.Update(deltaTime);
    LOG(m_vfx.getCurrentAnimName());
	
}



// draw MainCharacter and its deadbodies
void MainCharacter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);

    for (const auto& dbd : m_deadbodies)
    {
        target.draw(dbd);
    }

    target.draw(m_vfx);
}

void MainCharacter::StartEndGame()
{
    m_IsPlayingEndGame = true;
}

void MainCharacter::StartEndLevel()
{
    m_IsPlayingEndLevel = true;
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
            m_current_elem = terrain::Element::Air;
            break;        
        case(1): 
            m_InTheAir = false;
            m_InTheWater = true; 
            m_InTheLava  = false;
            m_InTheVoid  = false; 
            m_current_elem = terrain::Element::Water;
            break;
        case(2):
            m_InTheAir = false;
            m_InTheWater = false;
            m_InTheLava  = true;
            m_InTheVoid  = false;
            m_current_elem = terrain::Element::Lava;
            break;
        case(3): 
            m_InTheAir = false;
            m_InTheWater = false;
            m_InTheLava  = false;
            m_InTheVoid  = true;
            m_current_elem = terrain::Element::Void;
            break;
        case(10): // Transition Water => Air
            m_InTheAir = true;
            m_InTheWater = false;
            m_InTheLava = false;
            m_InTheVoid = false;
            m_current_elem = terrain::Element::Air;
            break;
        case(20): // Transition Lava => Air
            m_InTheAir = true;
            m_InTheWater = false;
            m_InTheLava = false;
            m_InTheVoid = false;
            m_current_elem = terrain::Element::Air;
            break;
        default:
            break;
        }
    }

}


void MainCharacter::ResetElements()
{
    // set Elements 
    m_InTheAir      = false;
    m_InTheWater    = false;
    m_InTheVoid     = false;
    m_InTheLava     = false;
}

void MainCharacter::setRespawnPosition(const sf::Vector2f& new_respawn)
{
    m_RespawnPosition = new_respawn;
}

void MainCharacter::ResetJumpCounter()
{
    m_nbjumps = 0;
}

void MainCharacter::setPosition(float deltaTime, std::vector<Plateform>& Pf, std::vector<Ennemie>& l_cactus, short unsigned int& cloop)
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
        _colliding_cactus = false;
        isCollidingSolid(new_Position, Pf, l_cactus);

        if ((!_colliding_plateforms) and (not ( _colliding_deadbodies or _colliding_cactus)))
        {
            m_Position = new_Position;
            break;
        }
		else if ( _colliding_deadbodies)
		{
            new_Position = m_Position;
            m_Velocity.y = 0.0f;
			cloop++;
            setPosition(deltaTime, Pf, l_cactus, cloop);
		}
        else
        {
            new_Position = m_Position;
            m_Velocity.y *= 0.9f;
            cloop++;
            setPosition(deltaTime, Pf, l_cactus, cloop);
            
        }
    } 
}

// colliding plateforms and dead bodies 
void MainCharacter::isCollidingSolid(sf::Vector2f newpos, std::vector<Plateform>& Pf, std::vector<Ennemie>& l_cactus)
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

            // determine collision side character referentiel
            bool c_down_dead = OnTopOf(*p_pfmi);
            // We are on top of dead body 
            if (c_down_dead)
            {
                ResetJumpCounter();
            }
        }
    }	
    
    for (Ennemie& cac : l_cactus)
    {
        if (this->IsColliding(cac))
        {
            _colliding_cactus = true;
            m_isWalkable = false;
            m_InTheAir = false;

            // determine collision side character referentiel
            bool col_right = ToTheLeft(cac);
            bool col_left = ToTheRight(cac);

            m_isWalkable = false;
            m_InTheAir = false;
            // If collision stops velocity on the direction
            bool collided_cac_left = col_left and not s_Velocity.x;
            bool collided_cac_right = col_right and s_Velocity.x;

            if (collided_cac_left or collided_cac_right)
            {
                m_Velocity.x = 0.0f;
                m_isWalkable = true;
                m_InTheAir = true;
            }
        }
    }

    if ((!_colliding_plateforms) and (!_colliding_deadbodies) and (!_colliding_cactus))
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


// Return true if colliding with deadbody on fire because of lava
bool MainCharacter::isCollidingDeadBodyLava()
{
    // Handling colliding with deadbody in lava 
    bool lavacolliding = false;
    for (DeadBody& dbd : m_deadbodies)
    {
        if (not dbd.getIsOnFire())
        {
            continue;
        }

        if (dbd.IsColliding(*this))
        {
            lavacolliding = true;
        }
    }

    return lavacolliding;
}



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
//          Dead Bodies & Death
// ----------------------------------------

void MainCharacter::UpdateDeadBodies(float deltaTime, TileMap& Tm)
{
    // TO DO : maybe modified type of m_deadbodies => tableau ?
    if ( m_deadbodies.size() > MAX_DEADBODIES )
    {
        m_deadbodies.erase(m_deadbodies.begin());
    }

    // According to the timer for specific element (water)
    std::vector<DeadBody>::iterator it_dbd = m_deadbodies.begin();
    while (it_dbd != m_deadbodies.end())
    {
        if (it_dbd->CanBeRemoved())
        {
            // erase returns following element
            it_dbd = m_deadbodies.erase(it_dbd);
        }
        else
        {
            ++it_dbd;
        }
    }
	
    // Update Living deadbodies
    for (int i = 0; i < m_deadbodies.size(); i++)
    {
        m_deadbodies[i].Update(deltaTime, Tm);
    }

}


// action by mushrooms : tag to be removed 
void MainCharacter::RemoveDeadbody(int index)
{
    if (index < m_deadbodies.size())
    {
        m_deadbodies[index].EatenByEnnemies(); 
    }
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


bool MainCharacter::OnTopOf(BoxCollideable& other)
{
   
    const sf::Vector2f center_a = GetCenter(); 
    const sf::Vector2f center_b = other.GetCenter();

    bool y_top = center_a.y < center_b.y ; 
    bool neighb_x = std::abs(center_a.x - center_b.x) < 64;

    return y_top && neighb_x;
}

bool MainCharacter::BelowOf(BoxCollideable& other)
{

    const sf::Vector2f center_a = GetCenter();
    const sf::Vector2f center_b = other.GetCenter();

    bool y_down = center_a.y > center_b.y;
    bool neighb_x = std::abs(center_a.x - center_b.x) < 64;

    return y_down && neighb_x;
}

inline bool MainCharacter::ToTheLeft(BoxCollideable& other)
{

    const sf::Vector2f center_a = GetCenter();
    const sf::Vector2f center_b = other.GetCenter();

    bool x_left = center_a.x < center_b.x;
    bool neighb_y = std::abs(center_a.y - center_b.y) < 128;

    return x_left && neighb_y;
}

inline bool MainCharacter::ToTheRight(BoxCollideable& other)
{

    const sf::Vector2f center_a = GetCenter();
    const sf::Vector2f center_b = other.GetCenter();

    bool x_right = center_a.x > center_b.x;
    bool neighb_y = std::abs(center_a.y - center_b.y) < 128;

    return x_right && neighb_y;
}

//  ---------------------------------
//              Animation 
// ----------------------------------

// Animation : set Frame (texture)
void MainCharacter::Play(AnimName anim_name, float deltaTime, bool loop)
{
    
    if (m_current_anim != anim_name)
    {
        Stop(); // reset counters
		//if (not loop) // flag anim done valid on not looping animations
		a_done_anim = false;  
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
    m_AllAnims.DoubleJump   = { 6, 2, 2, "DoubleJump" };
    m_AllAnims.Die          = { 6, 4, 0, "Die" };
    m_AllAnims.Hurt         = { 2, 4, 1, "Hurt" };
    m_AllAnims.FireSet      = { 5, 11, 4, "FireSet" };
    m_AllAnims.FireBegin    = { 4, 11, 0, "FireBegin" };
    m_AllAnims.FireEnd      = { 4, 11, 10, "FireEnd" };
    m_AllAnims.Reborn       = { 3, 4, 6, "Reborn" };

    dictAnim[AnimName::Idle]        = m_AllAnims.Idle;
    dictAnim[AnimName::Walk]        = m_AllAnims.Walk;
    dictAnim[AnimName::Jump]        = m_AllAnims.Jump;
    dictAnim[AnimName::DoubleJump]  = m_AllAnims.DoubleJump;
    dictAnim[AnimName::Die]         = m_AllAnims.Die;
    dictAnim[AnimName::FireSet]     = m_AllAnims.FireSet;
    dictAnim[AnimName::FireBegin]   = m_AllAnims.FireBegin;
    dictAnim[AnimName::FireEnd]     = m_AllAnims.FireEnd;
    dictAnim[AnimName::Reborn]      = m_AllAnims.Reborn;

}

void MainCharacter::setFrameTexture(AnimName anim_name, float deltaTime)
{
    short unsigned int nb_frames_anim=1; 
    short unsigned int line_anim=0;
    short unsigned int a_offset = 0; // offset frame pour l'animation si besoin
    const sf::Vector2i sizetexture = {64,64};
	
    nb_frames_anim = dictAnim[anim_name].nb_frames_anim;
    line_anim = dictAnim[anim_name].line_anim;
    a_offset = dictAnim[anim_name].a_offset;

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

// Set the direction that the character is facing : left(false) or right(true), 
// set a_direction value
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


float MainCharacter::GetPourcentageAllowedTime(terrain::Element elem) const 
{
	// begining 0% = out of element 
	float pct_time = 0.0f;

    static const float MAX_TIMER_WATER = 1.5f; 
    static const float MAX_TIMER_VOID  = 0.25f; 
    static const float MAX_TIMER_LAVA  = 3.0f; 
	
	switch(elem)
	{
		case(terrain::Element::Water):
			pct_time = m_CounterWater / MAX_TIMER_WATER;
			break; 
		case(terrain::Element::Void):
			pct_time = m_CounterVoid / MAX_TIMER_VOID;
			break; 
		case(terrain::Element::Lava):
			pct_time = m_CounterLava / MAX_TIMER_LAVA;
			break; 
		default:
			break; 
	}

	return pct_time;
	
}

void MainCharacter::ResetTimers()
{
    m_CounterWater = 0.0f; 
    m_CounterVoid  = 0.0f;
    m_CounterLava  = 0.0f;
}

// Set Alive or Dead
bool MainCharacter::Alive(float deltaTime, std::vector<Ennemie> l_ennemies, std::vector<MovableEnnemies> l_mennemies)
{
    static const float TIMER_DEAD_WATER = 1.5f; 
    static const float TIMER_DEAD_VOID  = 0.25f;
    static const float TIMER_DEAD_LAVA  = 3.0f;
    
    if (m_Respawning)
    {
		setAliveOrDead(true);
        return getAlive();
    }
    
    // update elements counters
    bool isDead_Water = TimerElements(deltaTime, m_InTheWater, TIMER_DEAD_WATER, m_CounterWater);
    bool isDead_Void = TimerElements(deltaTime, m_InTheVoid , TIMER_DEAD_VOID , m_CounterVoid );
    bool isDead_Lava = TimerElements(deltaTime, m_timer_lava, TIMER_DEAD_LAVA, m_CounterLava);

    if (isDead_Water) { m_DiedInWater = true; }
    if (isDead_Void)  { m_DiedInVoid  = true; }
    if (isDead_Lava)  { m_DiedInLava  = true; }
    
    if (m_DiedInWater or m_DiedInVoid or m_DiedInLava)
    {
		setAliveOrDead(false);
        return getAlive();
    }


    // check at each frame if it collides against ennemies 
    for (auto const& enm : l_ennemies)
    {
        if (IsColliding(enm) and m_isAlive)
        {	
			// Die 
            setAliveOrDead(false);
            break;
        }
    }

    for (auto const& menm : l_mennemies)
    {
        if (IsColliding(menm) and m_isAlive)
        {
            // Die 
            setAliveOrDead(false);
            m_HitByEnnemies = true;
            break;
        }
    }


    return getAlive();
}

// ----------------- //
//      Sound        //
// ----------------- //


void MainCharacter::LoadStructSound(struct SoundType& onesound, const std::string soundpath, bool looping)
{
    onesound.no_sound = not onesound.s_buffer.loadFromFile(soundpath);
    onesound.is_playing = false;
    onesound.pathsound  = soundpath;
    onesound.looping    = looping; 
};

void MainCharacter::InitSoundType()
{
    /*
    struct SoundType {
        sf::SoundBuffer s_buffer;       // sound buffer 
        bool is_playing{ false };       // sound is playing
        bool no_sound{ true };          // = sound not loaded
        std::string pathsound{ "" };    // path to wav sound file 
        bool looping{ false };          // looping or not
    };
    */

    LoadStructSound(m_AllSounds.Jump, "Assets\\Sounds\\jump_03.wav", false); 
    LoadStructSound(m_AllSounds.DoubleJump, "Assets\\Sounds\\jump_water_06.wav", false);
    LoadStructSound(m_AllSounds.Die, "Assets\\Sounds\\deaths\\cat_meow_01.wav", false);
    LoadStructSound(m_AllSounds.FireEnd, "Assets\\Sounds\\deaths\\cat_meow_02.wav", false);
    LoadStructSound(m_AllSounds.FireBegin, "Assets\\Sounds\\lava.flac", false);
    LoadStructSound(m_AllSounds.FireSet, "Assets\\Sounds\\ignition.flac", false);
    // default initialisation for the others
    dictSound[AnimName::Idle] = m_AllSounds.Idle;
    dictSound[AnimName::Walk] = m_AllSounds.Walk;
    dictSound[AnimName::Jump] = m_AllSounds.Jump;
    dictSound[AnimName::DoubleJump] = m_AllSounds.DoubleJump;
    dictSound[AnimName::Die] = m_AllSounds.Die;
    dictSound[AnimName::FireSet] = m_AllSounds.FireSet;
    dictSound[AnimName::FireBegin] = m_AllSounds.FireBegin;
    dictSound[AnimName::FireEnd] = m_AllSounds.FireEnd;
    dictSound[AnimName::Reborn] = m_AllSounds.Reborn;

}

void MainCharacter::setSoundType(AnimName anim)
{
    if (not dictSound[anim].no_sound)
    {
        m_soundfx.setBuffer(dictSound[anim].s_buffer);
        m_soundfx.setVolume(GetSFXVolume());
        m_soundfx.setLoop(dictSound[anim].looping);
    }
}

void MainCharacter::playSFX(AnimName anim)
{
    if (dictSound[anim].no_sound) { return; }
    m_soundfx.setBuffer(dictSound[anim].s_buffer); // re-set buffer 
    m_soundfx.play();
    dictSound[anim].is_playing = true; 
}

void MainCharacter::resetPlaying()
{
    // TODO iterate on enum
    dictSound[AnimName::Idle].is_playing = false;
    dictSound[AnimName::Walk].is_playing = false;
    dictSound[AnimName::Jump].is_playing = false;
    dictSound[AnimName::DoubleJump].is_playing = false;
    dictSound[AnimName::Die].is_playing = false;
    dictSound[AnimName::FireSet].is_playing = false;
    dictSound[AnimName::FireBegin].is_playing = false;
    dictSound[AnimName::FireEnd].is_playing = false;
    dictSound[AnimName::Reborn].is_playing = false;
}

