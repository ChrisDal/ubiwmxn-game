#include <stdafx.h>
#include <Game/Ennemie.h>
#include <Game/EnumElements.h>
#include <Game/MainCharacter.h> 



sf::Texture*  Ennemie::m_pTextureAtlas = nullptr;

Ennemie::Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy)
	: e_Position(spawn_pos), moving(canmove), is_animated(is_animated), 
	_colliding_plateforms(false), e_IsPlayingEndGame(false),
	_colliding_fire(false)
{
	e_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	e_Sprite.setTexture(*m_pTextureAtlas);
	e_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y, e_size.x, e_size.y));
	// Set Origin
	e_Sprite.setOrigin(e_size * 0.5f);
	// Set Position
	e_Sprite.setPosition(e_Position);
	// Bounding box
	SetBoundingBox(e_Position, e_size);

	// neighboorhood 
	_neighb.setNeighboorhood(this, 5.0f);

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

void Ennemie::Neighboorhood::setNeighboorhood(Ennemie *enm,float dpx)
{

	sf::Vector2f new_size{enm->GetBoundingBox().width + dpx, enm->GetBoundingBox().height + dpx };
	this->SetBoundingBox(enm->GetCenter(), new_size);

}

void Ennemie::Update(float deltaTime, MainCharacter* mchara) 
{
	if (moving)
	{
		// Do something to update
	}

	if (is_animated)
	{
		const float TIMER_FIRE = 2.0f; // Time in seconds after when destroying objects 
		SetCollidingFire(mchara);
		// Do something to update
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

		m_current_fire = m_fire_steps[step_fire];
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
				break;
			default:
				break;
		}

	}
}; 