#pragma once

#include <Game/EnumElements.h>

class MainCharacter;

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

    enum class Fire {None, Red, White, SemiTransp, Transp};

public:
    static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }

    Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft,
            unsigned int sx, unsigned int sy);
    ~Ennemie(); 

    void Update(float deltaTime, MainCharacter* mchara);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    bool getCanMove() const { return moving; }; 

    // Fire handling 
    void SetCollidingFire(MainCharacter* mchara);
    

protected:
    // Neighboorhood
    class Neighboorhood : public BoxCollideable 
    {
    public: 
        void setNeighboorhood(Ennemie* enm, float dpx);
    };
    

private:
    
    sf::Sprite e_Sprite;
    sf::Vector2f e_size;

    sf::Vector2f e_Position;
    
    bool e_IsPlayingEndGame;

    // attributes for all types of ennemies
    bool _colliding_plateforms;
    bool moving; 
    bool is_animated; 
    bool _colliding_fire;
    bool _colliding{ false };

    // Neighboorhood 
    Ennemie::Neighboorhood _neighb; 

    // Fire 
    float counter_fire = 0.0f;
    float a_counter_seconds = 1.0f / 10.0f;
    Fire m_current_fire = Fire::None;
    short unsigned int step_fire = 0;
    std::map<int, Fire> m_fire_steps{ {0, Fire::None}, {1, Fire::Red},  {2, Fire::White},
                                    {3, Fire::SemiTransp},{4, Fire::Transp}  };

    


};