#pragma once 

class VFX : public sf::Drawable, public Animation
{

	static sf::Texture* m_pTextureAtlas;

	struct AllAnims {
		struct AnimType Idle;
		struct AnimType DemiCircularActivation;
		struct AnimType DustJump;
		struct AnimType Reborn;
		struct AnimType Fire;
	};

	
public: 

	enum class AnimName {
		Idle, DemiCircularActivation,
		DustJump, Reborn, Fire
	};


	VFX();
	VFX(const sf::Vector2f position);
	~VFX() {};
	
	// Atlas texture
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// set position rot scale , move sprite also
	void setSpriteParameters(sf::Vector2f position, float rot, sf::Vector2f scale);

	// Update 
	void Update(float deltaTime, sf::Vector2f position, float rot, sf::Vector2f scale, AnimName vname, bool sidex); 
	void Update(float deltaTime, AnimName vname, bool sidex); 
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Animation 
	void Play(AnimName anim_name, float deltaTime);
	void setFrameTexture(AnimName anim_name, float deltaTime);
	void SetFacingDirection(float speedx);
	bool isPlaying() const { return a_isPlaying; }
	inline void setCurrentAnim(AnimName anim_name) { a_current_anim = anim_name; }
	void InitAnimType(); 
	inline AnimName getCurrentAnim() const { return a_current_anim; }
	
	// Following Elements 
	void setPosition(BoxCollideable* element) { 
		m_Position = element->GetCenter(); 
		m_Sprite.setPosition(m_Position); 
	}
	inline void setPosition(sf::Vector2f newpos) { 
		m_Position = newpos; 
		m_Sprite.setPosition(m_Position);
	}
	

protected: 

	sf::Sprite m_Sprite; 
	sf::Vector2f m_Size; 
	sf::Vector2f m_Position; 
	float m_Rotation; 
	sf::Vector2f m_Scale; 
	
	// animation 
	AnimName a_current_anim{ AnimName::Idle };
	std::map< VFX::AnimName, AnimType > m_dictvfxs;
	AllAnims m_vfxs; 

}; 