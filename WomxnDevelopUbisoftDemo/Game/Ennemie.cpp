#include <stdafx.h>
#include <Game/Ennemie.h>

Ennemie::Ennemie(sf::Vector2f& spawn_pos, bool canmove, sf::Vector2u& upperleft, const std::string& spriteimage, unsigned int sx, unsigned int sy)
	: e_Position(spawn_pos), moving(canmove), _colliding_plateforms(false), e_IsPlayingEndGame(false)
{
	e_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));
	printf("Coord Texture x,y : %d %d \n", upperleft.x, upperleft.y);
	// Get texture 
	e_Texture.loadFromFile(".\\Assets\\ennemies_all_black.png");
	//if (!e_Texture.loadFromImage(spriteimage, sf::IntRect(upperleft.x, upperleft.y, e_size.x, e_size.y)))
	//	printf("Texture for Ennemies didnt properly load\n");
	// Set texture 
	e_Sprite.setTexture(e_Texture);
	e_Sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
	// Set Origin
	e_Sprite.setOrigin(e_size * 0.5f);
	// Set Position
	e_Sprite.setPosition(e_Position);
	// Bounding box
	//SetBoundingBox(e_Position, e_size);

};

Ennemie::~Ennemie() { printf("Free Ennemie\n"); };

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