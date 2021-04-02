#include <stdafx.h>
#include <Game/Ennemie.h>
#include <Game/EnumElements.h>


sf::Texture*  Ennemie::m_pTextureAtlas = nullptr;

Ennemie::Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy)
	: e_Position(spawn_pos), moving(canmove), _colliding_plateforms(false), e_IsPlayingEndGame(false)
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


void Ennemie::Update(float deltaTime) 
{
	if (moving)
	{
		// Do something to update
	}
}; 