#include <stdafx.h>
#include <Game/Ennemie.h>
#include <Game/MainCharacter.h> 



sf::Texture*  Ennemie::m_pTextureAtlas = nullptr;

Ennemie::Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy, bool weakness_fire)
	: e_Position(spawn_pos), moving(canmove), is_animated(is_animated), 
	_colliding_plateforms(false), e_IsPlayingEndGame(false),
	_colliding_fire(false), e_Dead(false)
{
	e_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	e_Sprite.setTexture(*m_pTextureAtlas);
	e_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y, static_cast<int>(e_size.x), static_cast<int>(e_size.y)));
	// Set Origin
	e_Sprite.setOrigin(e_size * 0.5f);
	// Set Position
	e_Sprite.setPosition(e_Position);
	// Bounding box
	SetBoundingBox(e_Position, e_size);

	// neighboorhood : delta pixels = 5 px
	_neighb.setNeighboorhood(this, 5.0f);

	// Weak against : Fire 
	e_weak_fire = weakness_fire;

};

Ennemie::~Ennemie() {};

void Ennemie::StartEndGame()
{
	e_IsPlayingEndGame = true;
}

void Ennemie::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(e_Sprite);
}


void Ennemie::SetDead()
{
	if (m_current_fire == Fire::Transp)
	{
		e_Dead = true;
	}
	else 
	{
		// Alive 
		e_Dead = false; 
	}
}

void Ennemie::SetCollidingFire(MainCharacter* mchara)
{
	// colliding with neighboorhood 
	// as ennemie can be solid or not

	if (_neighb.IsColliding(*mchara))
	{
		_colliding = true;

		if (mchara->OnFire())
		{
			_colliding_fire = true;
		}
		else 
		{
			_colliding_fire = false;
		}
		
	}
	else 
	{
		_colliding_fire = false; 
		_colliding = false;
	}

}

void Ennemie::Neighboorhood::setNeighboorhood(Ennemie *enm, float dpx)
{

	sf::Vector2f new_size{enm->GetBoundingBox().width + dpx, enm->GetBoundingBox().height + dpx };
	this->SetBoundingBox(enm->GetCenter(), new_size);

}

void Ennemie::Update(float deltaTime, MainCharacter* mchara) 
{
	
	// Check if its alive or not 
	SetDead();

	if (e_Dead)
	{
		// Destroy Ennemies 
		return;
	}

	if (moving)
	{
		// Do something to update
	}

	if (is_animated)
	{
		if (e_weak_fire)
		{
			// check if main character collide neighboorhood with fire
			SetCollidingFire(mchara);
			// Handling colliding with fire
			if (_colliding_fire)
			{
				counter_fire += deltaTime;
				if (counter_fire > a_counter_seconds)
				{
					step_fire += 1;
					counter_fire = 0.0f;
				}

			}
			else
			{
				counter_fire = 0.0f;
				step_fire = 0;
			}
			// Steps for fire animation 
			SetFireAnimation(step_fire);

		}

	}

}; 




void Ennemie::SetFireAnimation(short unsigned int& steps)
{
	// Fire animation 
	m_current_fire = m_fire_steps[step_fire];

	// Animation 
	switch (m_current_fire)
	{
	case(Fire::Red):
		e_Sprite.setColor(sf::Color(255, 0, 0));
		break;
	case(Fire::White):
		e_Sprite.setColor(sf::Color(255, 255, 0, 200));
		break;
	case(Fire::SemiTransp):
		e_Sprite.setColor(sf::Color(255, 255, 255, 128));
		break;
	case(Fire::Transp):
		e_Sprite.setColor(sf::Color(255, 255, 255, 0));
		// Prepare to destruction if object is not display anymore
		break;
	default:
		break;
	}

}