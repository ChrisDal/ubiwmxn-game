#pragma once

class Ennemie : public sf::Drawable, public BoxCollideable, public Animation
{
    static sf::Texture* m_pTextureAtlas;

    struct AllAnims {
        struct AnimType Idle;
        struct AnimType Walk;
        struct AnimType Jump;
        struct AnimType DoubleJump;
        struct AnimType Die;
        struct AnimType Hurt;
        struct AnimType Dodge;
    };

public:
    static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }

    Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy);
    ~Ennemie(); 

    void Update(float deltaTime);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    bool getCanMove() const { return moving; }; 

private:
    
    sf::Sprite e_Sprite;
    sf::Vector2f e_size;

    sf::Vector2f e_Position;
    
    bool e_IsPlayingEndGame;

    // attributes for all types of ennemies
    bool _colliding_plateforms;
    bool moving; 


};