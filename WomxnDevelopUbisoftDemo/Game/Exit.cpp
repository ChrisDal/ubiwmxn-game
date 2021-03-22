#include <stdafx.h>
#include <Game/Exit.h>


Exit::Exit(float xCenterPos, float yCenterPos)
	: m_IsPlayingEndGame{ false }

{
	const auto center = sf::Vector2f(xCenterPos, yCenterPos);
	m_Rectangle.setPosition(center);

	m_Texture.loadFromFile(".\\Assets\\11-Door\\Idle.png");
	m_TextureSize = sf::Vector2f(m_Texture.getSize());
	m_Rectangle.setSize(m_TextureSize);
	SetBoundingBox(center, m_TextureSize);
	m_Rectangle.setTexture(&m_Texture);
	m_Rectangle.setTextureRect(sf::IntRect(0, 0, (int)m_TextureSize.x, (int)m_TextureSize.y));
	m_Rectangle.setOrigin(m_TextureSize * 0.5f);
	

	//m_Rectangle.setOutlineThickness(1);
	//m_Rectangle.setOutlineColor(sf::Color(255, 255, 255, 255));
}

Exit::~Exit()
{
}


void Exit::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Rectangle);
}

void Exit::StartEndGame()
{
	m_IsPlayingEndGame = true;
	// Animation door opening 
	sf::Texture end_texture;
	if (!end_texture.loadFromFile(".\\Assets\\11-Door\\Opening_46x56.png")) {
		m_Rectangle.setTexture(&end_texture);
		m_Rectangle.setTextureRect(sf::IntRect((int)m_TextureSize.x * 3, 0, (int)m_TextureSize.x, (int)m_TextureSize.y));
	}
	else {
		m_Rectangle.setFillColor(sf::Color(255, 255, 255, 128));
	}
}