#include "stdafx.h"
#include "Animation.h"

// Animations : Play, Pause, Stop

// Play Animation
void Animation::Play(AnimName anim_name, float deltaTime)
{
	// Update frame texture 
    setFrameTexture(anim_name, deltaTime);
	// set current
	setCurrentAnim(anim_name); 
    setPlaying(true);
}

// Stop Animation
void Animation::Stop()
{
    // no playing 
    setPlaying(false);
    // reset counters
    a_framecount = 0; 
    a_frametexture = 0;
    a_sumdeltaTime = 0.0f; 

}
	
// Animation on Pause	
void Animation::Pause()
{
    setPlaying(false);
}


void Animation::setFrameTexture(AnimName anim_name, float deltaTime)
{
    // Do something 
    // see below 
    return;
}

/* Exemple of setFrameTexture 
void Animation::setFrameTexture(AnimName anim_name, float deltaTime)
{
    short unsigned int nb_frames_anim=1; 
    short unsigned int line_anim=0;
    short unsigned int a_offset = 0; // offset frame pour l'animation si besoin
    const sf::Vector2i sizetexture = {64,64};
	const float deltaSecond = 1.0f;
	
	// Animation changes
	if (anim_name != a_current_anim)
	{
		// reset counters
		Stop();
	}
	
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
    if (sumdeltaTime > (1.0f / a_framerate))
    {
        // set texture 
        int y = line_anim * sizetexture.y ;
        int x = (a_frametexture % (nb_frames_anim)) * sizetexture.x + ( a_offset * sizetexture.x);
        int leftrect = x + a_textsquare_offset.x;
        int sizex = sizetexture.x / 2;
        
        if (!a_direction)
        {
            leftrect = x + sizetexture.x - a_textsquare_offset.x;
            sizex = - sizetexture.x / 2;
        }
        
        m_Sprite.setTextureRect(sf::IntRect(leftrect, y + a_textsquare_offset.y, sizex, sizetexture.y/2));
        // ++ number of frame in this specific animation modulo nb frames anim 
        a_frametexture++;
		sumdeltaTime = 0.0f; 
    }


    a_framecount++;
}
*/

void Animation::setFacingDirection(float speedx)
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
    // else ==0.0f : relies on previous value

}



// Get a string describing current animation
std::string Animation::getAnimName()
{

    std::string animname;
    switch (a_current_anim)
    {
    case AnimName::Idle:
        animname = "Idle";
        break;
    case AnimName::Walk:
        animname = "Walk";
        break;
    case AnimName::Jump:
        animname = "Jump";
        break;
    case AnimName::DoubleJump:
        animname = "DoubleJump";
        break;
    case AnimName::Die:
        animname = "Die";
        break;
    case AnimName::Attack:
        animname = "Attack";
        break;
    case AnimName::Hurt:
        animname = "Hurt";
        break;
    case AnimName::Dodge:
        animname = "Dodge";
        break;
    case AnimName::Surprise:
        animname = "Surprise";
        break;
    case AnimName::Reborn:
        animname = "Reborn";
        break;

    default:
        animname = "No anim";
        break;
    }

    return animname;
}