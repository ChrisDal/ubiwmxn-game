#include <Engine/Collision.h>

class Neighbourhood : public BoxCollideable
{
public:
	// Set Neighbourhood
	inline void setNeighbourhood(BoxCollideable* chara, float dpx)
	{
		sf::Vector2f new_size{chara->GetBoundingBox().width + dpx, 
							  chara->GetBoundingBox().height + dpx };
		this->SetBoundingBox(chara->GetCenter(), new_size);
	}
	
	inline void setNeighbourhood(BoxCollideable* chara, float dpx_x, float dpx_y)
	{
		sf::Vector2f new_size{chara->GetBoundingBox().width + dpx_x, 
							  chara->GetBoundingBox().height + dpx_y };
		this->SetBoundingBox(chara->GetCenter(), new_size);
	}
	
	inline void setNeighbourhoodX(BoxCollideable* chara, float dpx_x)
	{
		sf::Vector2f new_size{chara->GetBoundingBox().width + dpx_x, 
							  chara->GetBoundingBox().height};
		this->SetBoundingBox(chara->GetCenter(), new_size);
	}
	
	inline void setNeighbourhoodY(BoxCollideable* chara, float dpx_y)
	{
		sf::Vector2f new_size{chara->GetBoundingBox().width, 
							chara->GetBoundingBox().height + dpx_y };
		this->SetBoundingBox(chara->GetCenter(), new_size);
	}	

	inline void setCircularNeighbourhood(BoxCollideable* chara, float radius)
	{
		float _radius {std::fmax(chara->GetBoundingBox().width, chara->GetBoundingBox().height) + radius };
		this->SetCircularBoundingBox(chara->GetCenter(), _radius);
	}

	inline const sf::Vector2f GetCircularCenter() const
	{
		return sf::Vector2f(m_CircularBoundingBox.getPosition());
	}


protected: 

	inline void SetCircularBoundingBox(const sf::Vector2f& center, const float radius)
	{
		m_CircularBoundingBox.setRadius(radius);
		m_CircularBoundingBox.setPosition(center);
	}

	inline void SetCircularCenter(const sf::Vector2f& center)
	{
		m_CircularBoundingBox.setPosition(center);
	}

	inline const bool ContainsCircular(const sf::Vector2f& pos) const 
	{
		return std::sqrt(std::pow(GetCircularCenter().x - pos.x, 2) + std::pow(GetCircularCenter().y - pos.y, 2)) < m_CircularBoundingBox.getRadius();
	}

	sf::CircleShape m_CircularBoundingBox;
};
