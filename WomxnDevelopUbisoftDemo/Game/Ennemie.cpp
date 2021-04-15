#include <stdafx.h>
#include <Game/Ennemie.h>
#include <Game/MainCharacter.h> 



sf::Texture*  Ennemie::m_pTextureAtlas = nullptr;

Ennemie::Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy, bool weakness_fire)
	: m_Position(spawn_pos), moving(canmove), is_animated(is_animated), 
	_colliding_plateforms(false), m_IsPlayingEndGame(false),
	_colliding_fire(false), m_Dead(false)
{
	m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	m_Sprite.setTexture(*m_pTextureAtlas);
	m_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y, static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
	// Set Origin
	m_Sprite.setOrigin(m_size * 0.5f);
	// Set Position
	m_Sprite.setPosition(m_Position);
	// Bounding box
	SetBoundingBox(m_Position, m_size);

	// neighboorhood : delta pixels = 5 px
	_neighb.setNeighboorhood(this, 5.0f);

	// Weak against : Fire 
	m_weak_fire = weakness_fire;

};

Ennemie::~Ennemie() {};

void Ennemie::StartEndGame()
{
	m_IsPlayingEndGame = true;
}

void Ennemie::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Sprite);
}


void Ennemie::SetDead()
{
	if (m_current_fire == Fire::Transp)
	{
		m_Dead = true;
	}
	else 
	{
		// Alive 
		m_Dead = false; 
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

// --------------- //
// Neighboorhood 
// --------------- //
void Ennemie::Neighboorhood::setNeighboorhood(Ennemie *enm, float dpx)
{

	sf::Vector2f new_size{enm->GetBoundingBox().width + dpx, enm->GetBoundingBox().height + dpx };
	this->SetBoundingBox(enm->GetCenter(), new_size);

}

void Ennemie::Neighboorhood::setNeighboorhoodX(Ennemie *enm, float dpx_x)
{

	sf::Vector2f new_size{enm->GetBoundingBox().width + dpx_x, enm->GetBoundingBox().height};
	this->SetBoundingBox(enm->GetCenter(), new_size);

}

void Ennemie::Neighboorhood::setNeighboorhoodY(Ennemie *enm, float dpx_y)
{

	sf::Vector2f new_size{enm->GetBoundingBox().width, enm->GetBoundingBox().height + dpx_y };
	this->SetBoundingBox(enm->GetCenter(), new_size);

}

// Update 
void Ennemie::Update(float deltaTime, MainCharacter* mchara) 
{
	
	// Check if its alive or not 
	SetDead();

	if (m_Dead)
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
		if (m_weak_fire)
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
		m_Sprite.setColor(sf::Color(255, 0, 0));
		break;
	case(Fire::White):
		m_Sprite.setColor(sf::Color(255, 255, 0, 200));
		break;
	case(Fire::SemiTransp):
		m_Sprite.setColor(sf::Color(255, 255, 255, 128));
		break;
	case(Fire::Transp):
		m_Sprite.setColor(sf::Color(255, 255, 255, 0));
		// Prepare to destruction if object is not display anymore
		break;
	default:
		break;
	}

}