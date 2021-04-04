#include <stdafx.h>
#include <Game/DeadBody.h>
#include <Game/Plateform.h>


sf::Texture* DeadBody::m_pTextureAtlas = nullptr;

// constructor 
DeadBody::DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, bool pass_through, terrain::Element elem)
    : m_isWalkable(pass_through), m_Position{position},
    c_down{false}, c_left{false}, c_up{false}, c_right{false}
{

    sf::Vector2i offset_texture = sf::Vector2i(12, 22);
    setTextureOffset(offset_texture);
    m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));
    sf::Vector2u textdeath{ 0,0 }; /// by line 

    // Set texture 
    m_Sprite.setTexture(*m_pTextureAtlas);
    // According to death terrain draw something
    m_death_element = elem; 
    switch (elem)
    {
    case(terrain::Element::Air):
        // classical death
        textdeath.x = 0;
        textdeath.y = 0;
        break;
    case(terrain::Element::Water):
        // classical death
        textdeath.x = 6;
        textdeath.y = 0;
        break;
    case(terrain::Element::Void):
        textdeath.x = 5;
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

    m_Sprite.setTextureRect(sf::IntRect(getTextureOffset().x + textdeath.x * 64, getTextureOffset().y + textdeath.y * 64, m_size.x, m_size.y));

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

};


void DeadBody::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void DeadBody::Update(float deltaTime)
{
    // Update if any actions of the player on dead body 
    // According to terrain play 
    // 
    // 
    // m_plateform.setPosition


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

void DeadBody::Play(AnimAction anim_name, float deltaTime, bool loop)
{
    // Update frame texture 
    setFrameTexture(anim_name, deltaTime);
    // set current
    setCurrentAnim(anim_name);
    setPlaying(true);
}


void DeadBody::setFrameTexture(AnimAction anim_name, float deltaTime)
{
    short unsigned int nb_frames_anim=1; 
    short unsigned int line_anim=0;
    short unsigned int a_offset = 0; // offset frame pour l'animation si besoin
    const sf::Vector2i sizetexture = {64,64};
	
	// Animation changes
	if (anim_name != a_current_anim)
	{
		// reset counters
        Stop();
	}
	
    switch (anim_name)
    {
        case AnimAction::Idle:
            nb_frames_anim = 4;
            line_anim = 0;
            a_offset = 0;
            break;
        case AnimAction::Stack:
            nb_frames_anim = 8;
            line_anim = 1;
            a_offset = 0;
            break;
        case AnimAction::Launch:
            nb_frames_anim = 8;
            line_anim = 2;
            a_offset = 0;
            break;
        case AnimAction::Fire:
            nb_frames_anim = 6;
            line_anim = 2;
            a_offset = 2;
            break;
        case AnimAction::Iced:
            nb_frames_anim = 7;
            line_anim = 4;
            a_offset = 0;
            break;
        case AnimAction::Slippy:
            nb_frames_anim = 6;
            line_anim = 15;
            a_offset = 0;
            break;
        default:
            nb_frames_anim = 4;
            line_anim = 4;
            a_offset = 0;
            break;
    };


    // DT = 1/ 60.0 APP FRAMERATE 
    a_sumdeltaTime += deltaTime;

    // si on dépasse 1/framerate(anim) on change de frame  
    if (a_sumdeltaTime > (1.0f / a_framerate))
    {
        // set texture rectangle
        int y = line_anim * sizetexture.y ;
        int x = (a_frametexture % (nb_frames_anim)) * sizetexture.x + ( a_offset * sizetexture.x);
        int leftrect = x + a_textsquare_offset.x;
        int sizex = sizetexture.x / 2;
        
        // flip texture according to facing direction
        if (!a_direction)
        {
            leftrect = x + sizetexture.x - a_textsquare_offset.x;
            sizex = - sizetexture.x / 2;
        }
        
        m_Sprite.setTextureRect(sf::IntRect(leftrect, y + a_textsquare_offset.y, sizex, sizetexture.y/2));
        // ++ number of frame in this specific animation modulo nb frames anim 
        a_frametexture++;
        a_sumdeltaTime = 0.0f;
    }


    // ToDo : reset counters before they it the maximum => call Pause
}

void DeadBody::InitAnimType()
{
    
    m_AllAnims.Idle = { 4, 0, 0, "Idle" };
    m_AllAnims.Stack = { 8, 1, 0, "Walk" };
    m_AllAnims.Launch = { 8, 2, 0, "Jump" };
    m_AllAnims.Fire = { 6, 2, 2, "DoubleJump" };
    m_AllAnims.Iced = { 7, 4, 0, "Die" };
    m_AllAnims.Slippy = { 2, 4, 1, "Hurt" };
    m_AllAnims.Smoked = { 6, 14, 0, "Dodge" };
    m_AllAnims.Swollen = { 6, 14, 0, "Surprise" };
    m_AllAnims.Ladder = { 4, 4, 7, "Reborn" };

}