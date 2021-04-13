#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <stdafx.h>
#include <Game/DeadBody.h>
#include <Game/Plateform.h>

#define DEBUG 1 
#if DEBUG 
    #define LOG(x) std::cout << x  << " "
# else
    #define LOG(x)
#endif

sf::Texture* DeadBody::m_pTextureAtlas = nullptr;
const float DeadBody::TIME_DESTRUCTION_WATER = 27.0f;

// constructor 
DeadBody::DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, bool pass_through, terrain::Element elem, bool sidex)
    : m_isWalkable(pass_through), m_Position{position}, m_Velocity{0.0f, 0.0f},
    c_down{false}, c_left{false}, c_up{false}, c_right{false}
{

    sf::Vector2i offset_texture = sf::Vector2i(12, 23);
    setTextureOffset(offset_texture);
    m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));
    sf::Vector2u textdeath{ 0,0 }; // texture position

    // Set texture 
    m_Sprite.setTexture(*m_pTextureAtlas);
    // According to death terrain draw something
    m_death_element = elem; 
    // sidex : right(true) or left(false)
    a_direction = sidex;
    switch (elem)
    {
    case(terrain::Element::Air):
        // classical death
        textdeath.x = 0;
        textdeath.y = 0;
        break;
    case(terrain::Element::Water):
        // classical death
        //textdeath.x = 6;
        textdeath.x = 0;
        textdeath.y = 4;
        break;
    case(terrain::Element::Void):
        //textdeath.x = 5;
        textdeath.x = 0;
        textdeath.y = 0;
        break;
    case(terrain::Element::Lava):
        textdeath.x = 12;
        textdeath.y = 2;
        break;
    default:
        // classical death
        textdeath.x = 0;
        textdeath.y = 0;
        break; 

    }

    if (!a_direction)
    {
        m_Sprite.setTextureRect(sf::IntRect(getTextureOffset().x + static_cast<int>(m_size.x) + textdeath.x * 64,
                                            getTextureOffset().y + textdeath.y * 64,
                                            - static_cast<int>(m_size.x),
                                            static_cast<int>(m_size.y)));
    }
    else {
        m_Sprite.setTextureRect(sf::IntRect(getTextureOffset().x + textdeath.x * 64,
                                            getTextureOffset().y + textdeath.y * 64,
                                            static_cast<int>(m_size.x),
                                            static_cast<int>(m_size.y)));
    }

    // Set Origin
    m_Sprite.setOrigin(m_size * 0.5f);
    // Set Position
    m_Sprite.setPosition(m_Position);
    // Bounding box : neighboorhood
    SetBoundingBox(m_Position, sf::Vector2f(25.0f,12.0f));


    // can we pass through the dead body = walk through 
    // else plateform
    if (!m_isWalkable)
    {
        DeadToPlateform();
    }

    // Initial Animation 
    Stop();         // reset counters
    InitAnimType(); // set animation data
	ResetElapsedTime(); 
};

void DeadBody::ResetElapsedTime()
{
	t_elapsed = 0.0f;
}

// Timer for water
bool DeadBody::ReachedTime()
{
    float timer = 10.0f;
    switch (m_death_element)
    {
    case (terrain::Element::Water): 
        timer = TIME_DESTRUCTION_WATER; 
        break; 
    default: 
        timer = 60.0f; // removed after 1 min
        break;
    }
    return t_elapsed > timer;
}

bool DeadBody::CanBeRemoved()
{
    bool is_done = ReachedTime(); // More conditions later maybe
    return is_done; 
}

void DeadBody::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void DeadBody::Update(float deltaTime, TileMap& Tm)
{
    sf::Vector2f new_pos = m_Position - sf::Vector2f(0.0f, 10.0f);
    float theta = 0.0f;
    // Update if any actions of the player on dead body 
    // According to terrain play 
	switch (m_death_element)
    {
    case(terrain::Element::Air):
        // classical death
		Play(AnimName::Stack, deltaTime);	
        m_Velocity.y = 0.0f;
        break;
    case(terrain::Element::Water):
        // death water
		Play(AnimName::Water, deltaTime);
        // Do get up til air 
        t_elapsed += deltaTime;
        // In water or in transition water//air: velocity ++
        
        if (Tm.ElementsTiles(m_Position) == 1 )
        {
            // Poussée d'archimede 1000 km/m^-3 * 0.02 m^3 * g 
            m_Velocity.y -= (1000 * 0.008f * 9.81f) / 32.0f;
        }
        else if (Tm.ElementsTiles(new_pos) == 10 and Tm.ElementsTiles(m_Position) == 1)
        {
            // Poussée d'archimede 1000 km/m^-3 * 0.02 m^3 * g 
            m_Velocity.y -= (1000 * 0.008f * 9.81f) / 32.0f;
            // Ajout rotation
            float Am0 = 10.0f / t_elapsed;
            theta = Am0 * cos(2 * M_PI * t_elapsed / (1.0f));
            if (std::abs(theta) < 1.0f)
            {
                theta = 0.0f;
            }
        }
        else if (Tm.ElementsTiles(new_pos) == 10 and Tm.ElementsTiles(m_Position) == 10)
        {
            // Poussée d'archimede 1000 km/m^-3 * 0.02 m^3 * g 
            m_Velocity.y -= (1000 * 0.008f * 9.81f) / 32.0f;
            // Ajout rotation
            float Am0 = 10.0f / t_elapsed;
            theta = Am0 * cos(2 * M_PI * t_elapsed / (1.0f));
            if (std::abs(theta) < 1.8f)
            {
                theta = 0.0f;
            }
        }
        else if (Tm.ElementsTiles(m_Position) == 10 and Tm.ElementsTiles(new_pos) == 0)
        {
            m_Velocity.y = 0.0f; 
            // Ajout rotation
            float Am0 = 10.0f / t_elapsed;
            theta = Am0 * cos(2 * M_PI * t_elapsed / (1.0f));
            if (std::abs(theta) < 1.8f)
            {
                theta = 0.0f;
            }
        }
        else
        {
            m_Velocity.y = 0.0f;
        }

        break;
    case(terrain::Element::Void):
		Play(AnimName::Void, deltaTime);
        m_Velocity.y = 0.0f;
        break;
    case(terrain::Element::Lava):
		// pas d'animation 
        m_Velocity.y = 0.0f;
        break;
    default:
        // no animation
        m_Velocity.y = 0.0f;
        break; 

    }

    // Set new position brut 
    m_Position += m_Velocity * deltaTime;
    // Set New position
    m_Sprite.setPosition(m_Position);
    SetCenter(m_Position);
    m_Sprite.setRotation(theta);
    // Update plateform position
    m_plateform.setPosition(m_Position);


};


void DeadBody::DeadToPlateform()
{   

    sf::FloatRect bbox = GetBoundingBox();
    sf::Vector2f a(bbox.left, bbox.top);
	sf::Vector2f b(bbox.left + bbox.width, bbox.top);
	sf::Vector2f c(bbox.left + bbox.width, bbox.top + bbox.height);
	sf::Vector2f d(bbox.left, bbox.top + bbox.height);
    m_plateform =  Plateform(a, b, c, d);
}

Plateform* DeadBody::get_Plateform()
{ 
	
    Plateform* pm_plateform = nullptr; 
    
    if (!m_isWalkable)
    {
        pm_plateform = &m_plateform;
    }
    
	return pm_plateform;  
}

//////////////////
//   Animation ///
//////////////////
// No loop 
void DeadBody::Play(AnimName anim_name, float deltaTime)
{
    if (a_done_anim)
	{
		return; 
	}
	
	if (a_frametexture == (dictAnim[anim_name].nb_frames_anim-1))
	{
        setDoneAnimation(true);
		setPlaying(false);
		Stop();
		return; 
	}
	// Update frame texture 
    setFrameTexture(anim_name, deltaTime);
    // set current
    setCurrentAnim(anim_name);
    setPlaying(true);
}


void DeadBody::setFrameTexture(AnimName anim_name, float deltaTime)
{
    short unsigned int nb_frames_anim   = dictAnim[anim_name].nb_frames_anim;
    short unsigned int line_anim 	    = dictAnim[anim_name].line_anim; 
    short unsigned int a_offset 		= dictAnim[anim_name].a_offset; // offset frame pour l'animation si besoin
    const sf::Vector2i sizetexture = {64,64};
	
	// Animation changes
	if (anim_name != a_current_anim)
	{
		// reset counters
        Stop();
		// flag anim done valid on not looping animations
		a_done_anim = false; 
	}

    // DT = 1/ 60.0 APP FRAMERATE 
    a_sumdeltaTime += deltaTime;

    // si on dépasse 1/framerate(anim) on change de frame  
    if (a_sumdeltaTime > (1.0f / a_framerate))
    {
        // set texture rectangle
        int y = line_anim * sizetexture.y ;
        int x = (a_frametexture % (nb_frames_anim)) * sizetexture.x + (a_offset * sizetexture.x);
        int leftrect = x + a_textsquare_offset.x;
        int sizex = sizetexture.x / 2;
        
        // flip texture according to facing direction
        if (!a_direction)
        {
            leftrect = x + a_textsquare_offset.x + sizetexture.x / 2;
            sizex = - sizetexture.x / 2;
        }
        
        m_Sprite.setTextureRect(sf::IntRect(leftrect, y + a_textsquare_offset.y, sizex, sizetexture.y/2));
        // ++ number of frame in this specific animation modulo nb frames anim 
        a_frametexture++;
        a_sumdeltaTime = 0.0f;
    }

	a_framecount++;
    // ToDo : reset counters before they it the maximum => call Pause
}

// override 
void DeadBody::setFacingDirection(float speedx)
{
    // direction for animation 
    if (speedx < -0.01f)
    {
        a_direction = false;
    }
    else if (speedx > 0.01f)
    {
        a_direction = true;
    }
    else
    {
        a_direction = false;
    }

}

void DeadBody::InitAnimType()
{
    
    m_AllAnims.Idle 	= { 4, 1, 0, "Idle" };
    m_AllAnims.Stack 	= { 7, 0, 0, "Stack" };
    m_AllAnims.Water 	= { 7, 4, 0, "Water" };
    m_AllAnims.Fire 	= { 5, 2, 6, "Fire" };
    m_AllAnims.Iced 	= { 2, 0, 0, "Iced" };
    m_AllAnims.Void 	= { 3, 0, 1, "Void" };
    m_AllAnims.Smoked 	= { 9, 2, 0, "Smoked" };
    m_AllAnims.FireEnd 	= { 3, 2, 11, "FireEnd" };
    m_AllAnims.Ladder 	= { 1, 0, 5, "Ladder" };
	
	dictAnim[AnimName::Idle]       = m_AllAnims.Idle;
    dictAnim[AnimName::Stack]      = m_AllAnims.Stack;
    dictAnim[AnimName::Water]       = m_AllAnims.Water;
    dictAnim[AnimName::Fire]  		= m_AllAnims.Fire;
    dictAnim[AnimName::Iced]        = m_AllAnims.Iced;
    dictAnim[AnimName::Void]     	= m_AllAnims.Void;
    dictAnim[AnimName::Smoked]  	= m_AllAnims.Smoked;
    dictAnim[AnimName::FireEnd]     = m_AllAnims.FireEnd;
    dictAnim[AnimName::Ladder]      = m_AllAnims.Ladder;

}