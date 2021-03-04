#pragma once

class Plateform : public sf::Drawable, public BoxCollideable
{
// main class for plateform 
// plateform can be immobile or movable 

public: 
    Plateform(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d);
    ~Plateform(); 
    
    void Update(float deltaTime); // update si animation
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    bool getWalkable() const;
    sf::Vector2f getPosition() const; 


protected: 
    sf::RectangleShape m_Rectangle;
    sf::Texture m_Texture;
    sf::Vector2f m_RectSize;
    sf::Vector2f m_position; 

    bool walkable; 
};