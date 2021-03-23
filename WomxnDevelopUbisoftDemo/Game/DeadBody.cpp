#include <stdafx.h>
#include <Game/DeadBody.h>


sf::Texture* DeadBody::m_pTextureAtlas = nullptr;

// constructor 
DeadBody::DeadBody(sf::Vector2f& position, unsigned int sx, unsigned int sy, MainCharacter mchara)
{

    sf::Vector2i offset_texture = sf::Vector2i(12, 22);
    setTextureOffset(offset_texture);
    m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

    // Set texture 
    m_Sprite.setTexture(*m_pTextureAtlas);
    m_Sprite.setTextureRect(sf::IntRect(getTextureOffset().x, getTextureOffset().y, m_size.x, m_size.y));
    // Set Origin
    m_Sprite.setOrigin(m_size * 0.5f);
    // Set Position
    m_Sprite.setPosition(position);
    // Bounding box : neighboorhood
    SetBoundingBox(position, m_size * 1.5f);

};


void DeadBody::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void DeadBody::Update(float deltaTime)
{
	// Update if any actions of the player on dead body 

	
}


//////////////////
//   Animation ///
//////////////////
void DeadBody::Play(AnimAction anim_name, float deltaTime)
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