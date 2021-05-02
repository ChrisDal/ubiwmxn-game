#pragma once
#include <Engine/Neighbourhood.h>
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


public:
    static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }

    Ennemie(sf::Vector2f& spawn_pos, bool canmove, bool is_animated, sf::Vector2u& upperleft,
            unsigned int sx, unsigned int sy, bool weakness_fire);
    ~Ennemie();
    Ennemie();

    void Update(float deltaTime, MainCharacter* mchara);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void StartEndGame();
    bool getCanMove() const { return moving; }; 

    // Death 
    bool GetIsDead() const { return m_Dead; }
    void SetDead(); // Return if dead or not

    // Fire handling 
    void SetCollidingFire(MainCharacter* mchara);
    void SetFireAnimation(short unsigned int& steps);
    

protected:
    // Neighboorhood 
    Neighbourhood m_neighb; 
    // attributes for all types of ennemies
    bool _colliding_plateforms;
    bool moving; 
    bool is_animated; 
    bool _colliding_fire{ false };
    bool _colliding{ false };
    

private:
    
    sf::Sprite m_Sprite;
    sf::Vector2f m_size;
    sf::Vector2f m_Position;
    
    // Dead 
    bool m_IsPlayingEndGame;
    bool m_Dead;

    // Fire 
    bool m_weak_fire;
    float counter_fire = 0.0f;
    float a_counter_seconds = 1.0f / 10.0f;
    Fire m_current_fire = Fire::None;
    short unsigned int step_fire = 0;
    std::map<int, Fire> m_fire_steps{ {0, Fire::None}, {1, Fire::Red},  {2, Fire::White},
                                    {3, Fire::SemiTransp},{4, Fire::Transp}  };

    


};