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
};
