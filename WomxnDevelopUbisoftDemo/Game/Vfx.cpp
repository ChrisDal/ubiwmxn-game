#include <stdafx.h>
#include <iostream>
#include <Game/Vfx.h> 

#define DEBUG 0 
#if DEBUG 
	#define LOG(x) std::cout << x  << " "
# else
	#define LOG(x)
#endif

sf::Texture* VFX::m_pTextureAtlas = nullptr;

// Constructors 

VFX::VFX()
	: m_Position{50.0f, 50.0f}, m_Size{32.0f, 32.0f}
	, m_Rotation{0.0f}, m_Scale{1.0f, 1.0f}
{}; 



VFX::VFX(const sf::Vector2f position) 
	: m_Position(position), m_Scale{1.0f, 1.0f}, m_Rotation{ 0.0f }
{
	m_Size = sf::Vector2f(32.0f, 32.0f); 
	
	// Set texture 
    m_Sprite.setTexture(*m_pTextureAtlas);
	sf::Vector2i start_texture{ 0 , 30 * 32}; 
	m_Sprite.setTextureRect(sf::IntRect(start_texture.x, start_texture.y, m_Size.x, m_Size.y));
	// Set Origin
    m_Sprite.setOrigin(m_Size * 0.5f);
	 // Set Position
    m_Sprite.setPosition(m_Position);
	 // Set Rotation
    m_Sprite.setRotation(m_Rotation);
	// set Frame Rate for vfx 
	setFrameRate(10);
	
}


void VFX::setSpriteParameters(sf::Vector2f position, float rot, sf::Vector2f scale)
{
	this->setPosition(position); 
	m_Sprite.setRotation(rot); 
	m_Sprite.setScale(scale);
}

// moving along a position
void VFX::Update(float deltaTime, sf::Vector2f position, float rot, sf::Vector2f scale, AnimName vname, bool sidex)
{
	
	setSpriteParameters(position, rot, scale);
	// side : as spritesheet true, inverted false 
	a_direction = sidex; 
	// Play VFX
	Play(vname, deltaTime);
	
}

// Position/rotation/scale already fixed 
// Call play VFX 
void VFX::Update(float deltaTime, AnimName vname, bool sidex)
{
	// side : as spritesheet true, inverted false 
	a_direction = sidex; 
	Play(vname, deltaTime);
}

void VFX::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Sprite);
}




void VFX::Play(AnimName anim_name, float deltaTime)
{
	if (a_current_anim == AnimName::Idle and a_done_anim)
	{
		// idle = none 
		return; 
	}

	
	if (a_done_anim)
	{
		setCurrentAnim(AnimName::Idle);
		return; 
	}
	
	if (a_frametexture == (m_dictvfxs[anim_name].nb_frames_anim-1))
	{
		LOG("Done animating");
		setDoneAnimation(true);
		setPlaying(false);
		Stop();
		return; 
	}
	LOG("animating"); 
	// Update frame texture 
    setFrameTexture(anim_name, deltaTime);
    // set current
    setCurrentAnim(anim_name);
    setPlaying(true);
}


void VFX::setFrameTexture(AnimName anim_name, float deltaTime)
{
   short unsigned int nb_frames_anim    = m_dictvfxs[anim_name].nb_frames_anim;
    short unsigned int line_anim 	    = m_dictvfxs[anim_name].line_anim; 
    short unsigned int a_offset 		= m_dictvfxs[anim_name].a_offset; // offset frame
    static const sf::Vector2i sizetexture = sf::Vector2i(m_Size); // size texture = size of animated
	
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
        int sizex = sizetexture.x;
        
        // flip texture according to facing direction
        if (!a_direction)
        {
            leftrect = x + a_textsquare_offset.x + sizetexture.x;
            sizex = - sizetexture.x;
        }
        
        m_Sprite.setTextureRect(sf::IntRect(leftrect, y + a_textsquare_offset.y, 
											sizex, sizetexture.y));
        // ++ number of frame in this specific animation modulo nb frames anim 
        a_frametexture++;
        a_sumdeltaTime = 0.0f;
    }

	a_framecount++;

}


void VFX::SetFacingDirection(float speedx)
{
	a_direction = (speedx >= 0.0f); 
}

void VFX::InitAnimType()
{
	/* Informations
	struct AnimType {
		short unsigned int nb_frames_anim;
		short unsigned int line_anim;
		short unsigned int a_offset;
		std::string name;
	}; */

	m_vfxs.Idle		= { 1, 30, 0, "Idle" };
	m_vfxs.DemiCircularActivation = { 3, 30, 1, "DemiCircularActivation" };
	m_vfxs.DustJump = { 3, 30, 4, "DustJump" };
	m_vfxs.Reborn	= { 7, 31, 0, "Reborn" };
	m_vfxs.Fire		= { 1, 31, 6, "Fire" };

	m_dictvfxs[AnimName::Idle] = m_vfxs.Idle;
	m_dictvfxs[AnimName::DemiCircularActivation] = m_vfxs.DemiCircularActivation;
	m_dictvfxs[AnimName::DustJump] = m_vfxs.DustJump;
	m_dictvfxs[AnimName::Reborn] = m_vfxs.Reborn;
	m_dictvfxs[AnimName::Fire] = m_vfxs.Fire;

};