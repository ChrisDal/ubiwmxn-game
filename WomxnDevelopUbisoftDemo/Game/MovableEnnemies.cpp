#include <stdafx.h>
#include <Game/MovableEnnemies.h> 

sf::Texture*  MovableEnnemies::m_pTextureAtlas = nullptr;

// constructor 
MovableEnnemies::MovableEnnemies(sf::Vector2f SpawnPosition, unsigned int sx, unsigned sy, sf::Vector2u& upperleft)
	: m_Position(SpawnPosition), m_weak_fire{ true },
	_colliding_fire(false), _colliding_plateforms(false), m_Dead(false) 
{
	m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	m_Sprite.setTexture(*m_pTextureAtlas);
	m_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y)));
	// Set Origin
	m_Sprite.setOrigin(m_size * 0.5f);
	// Set Position
	m_Sprite.setPosition(m_Position);
	// Bounding box
	SetBoundingBox(m_Position, m_size);
	// velocity 
	m_Velocity = { 0.0f, 0.0f };
	// Neighbourhood : size character + deltapixels/2 
	m_neighb.setNeighbourhood(this, 64.0f, 5.0f);


};



bool MovableEnnemies::setPath(sf::Vector2f beginpoint, sf::Vector2f endpoint)
{
	
	m_Path = SequencePath(beginpoint, endpoint);
	return true;
	
}


void MovableEnnemies::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void MovableEnnemies::Update(float deltaTime)
{
	static const float VELOCITY_MAX = 100.0f; 
	static const float NO_VELOCITY  = 0.0f; 

	SequencePath::Instructions instruc =  m_Path.GetUpdateInstructions(*this); 
	
	switch (instruc)
	{
	case(SequencePath::Instructions::GoToRight):
		m_Velocity = { VELOCITY_MAX, NO_VELOCITY };
		break;
	case(SequencePath::Instructions::GoToLeft):
		m_Velocity.x = -VELOCITY_MAX;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::GoToUp):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = -VELOCITY_MAX;
		break;
	case(SequencePath::Instructions::GoToDown):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = VELOCITY_MAX;
		break;
	case(SequencePath::Instructions::Idle):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::NA):
		// problem
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::WaitingInstruc):
		// pause
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	default:
		// not handle 
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	}

	// new position 
	m_Position += m_Velocity * deltaTime; 
	// Set New position
	m_Sprite.setPosition(m_Position);
	SetCenter(m_Position);
	
}

