#pragma once
#include <Game/Vfx.h>

class ObjectsElements : public sf::Drawable, public BoxCollideable, public Animation
{
    static sf::Texture* m_pTextureAtlas;
    static float m_SFX_volume; 

public:
    static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }

    static void SetSFXVolume(float percentage) { m_SFX_volume = percentage;  }
    static const float GetSFXVolume() { return m_SFX_volume;  }

    ObjectsElements(sf::Vector2f& position, bool canmove, bool animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy, unsigned int elem_type);
    ObjectsElements() {};
    ~ObjectsElements(); 

    void Update(float deltaTime, bool activated);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	void Activate(float deltaTime); 
	void setObjType(unsigned int elemtype); 
    void setVFXmirrored(bool is_symetric) { m_vfx_mirrored = is_symetric;  }

    void StartEndGame();
    bool getCanMove() const { return moving; }; 
	
	// Sound handling 
	void setSoundType(); 
	inline void playSFX();

    // Visual 
    void playVFX(float deltaTime);

	bool getPlayStatusSFX() const { return m_soundfx.getStatus(); }

protected:
	enum class ObjectType { checkpoint, panneau, box, decor }; 

private:
    
    sf::Sprite m_Sprite;
    sf::Vector2f m_size;
    sf::Vector2f m_Position;
	
	ObjectType m_objtype; 
	
	// sound 
	sf::Sound m_soundfx; 
	sf::SoundBuffer m_sbuff; 
	bool m_NoSound{true};  // buffer not assigned 
    std::string m_soundrpath = "";

    // VFX 
    VFX m_vfx; 
    VFX m_vfx_mirror;
    bool m_vfx_mirrored{ true }; // if vfx is mirrored 

    bool m_IsPlayingEndGame{ false };

    // attributes for all types of ennemies
    bool _colliding_plateforms{ false };
    bool moving{ false };
    bool m_animated{ false };
    bool m_activated{ false };  // one time activation
};