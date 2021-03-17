#pragma once

class Ennemie : public sf::Drawable, public BoxCollideable
{
public:
    Ennemie(sf::Vector2f& spawn_pos, bool canmove, sf::Vector2u& upperleft, const sf::Image& spriteimage, unsigned int sx, unsigned int sy);
    ~Ennemie(); 

    void Update(float deltaTime);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    bool getCanMove() const { return moving; }; 

private:
    sf::Texture e_Texture;
    sf::Sprite e_Sprite;
    sf::Vector2f e_size;

    sf::Vector2f e_Position;
    
    bool e_IsPlayingEndGame;

    // attributes for all types of ennemies
    bool _colliding_plateforms;
    bool moving; 
};