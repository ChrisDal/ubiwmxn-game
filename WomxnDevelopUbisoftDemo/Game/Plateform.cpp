#include <stdafx.h>
#include <Game/Plateform.h>

Plateform::Plateform(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d)
	: walkable{ false }
{
	// size 
	m_RectSize = sf::Vector2f(b.x - a.x, a.y - d.y);
	m_Rectangle.setSize(m_RectSize);
	// center
	const auto center = sf::Vector2f(a.x + (b.x - a.x)/2 , d.y + (a.y - d.y)/2);
	m_Rectangle.setPosition(center);
	//bounding box
	SetBoundingBox(center, m_RectSize);
	m_Rectangle.setOrigin(m_RectSize * 0.5f);
	// for debug draw rectangle 
	m_Rectangle.setFillColor(sf::Color(125,116,65, 128));
}

Plateform::~Plateform()
{
};

Plateform::Plateform() {};

// Update position if needed
void Plateform::setPosition(sf::Vector2f new_center)
{
	m_Rectangle.setPosition(new_center);
	SetBoundingBox(new_center, m_RectSize);
}

void Plateform::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Rectangle);
};


sf::Vector2f Plateform::getPosition() const {return m_position; }


bool Plateform::getWalkable() const {return walkable; }



