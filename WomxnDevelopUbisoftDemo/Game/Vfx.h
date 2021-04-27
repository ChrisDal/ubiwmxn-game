#pragma once 

class VFX : public sf::Drawable, public Animation
{

	static sf::Texture* m_pTextureAtlas;

	struct AllAnims {
		struct AnimType EmptyFrame;
		struct AnimType DemiCircularActivation;
		struct AnimType DustJump;
		struct AnimType Reborn;
		struct AnimType Death;
		struct AnimType Fire;
		struct AnimType HitPurple;
		struct AnimType HitCyan;
		struct AnimType DustTrail;
	};

	
public: 

	enum class AnimName {
		EmptyFrame, DemiCircularActivation,
		DustJump, Reborn, Death, Fire, 
		HitPurple, HitCyan, DustTrail
	};


	VFX();
	VFX(const sf::Vector2f position, bool animate_once);
	~VFX() {};
	
	// Atlas texture
	static const sf::Texture* GetTextureAtlas()  { return m_pTextureAtlas;  }
    static void SetTextureAtlas(sf::Texture* _Tex) { m_pTextureAtlas = _Tex; }
	
	// set position rot scale , move sprite also
	void setSpriteParameters(sf::Vector2f position, float rot, sf::Vector2f scale);

	// Update 
	void Update(float deltaTime, sf::Vector2f position, float rot, sf::Vector2f scale, AnimName vname, bool sidex); 
	void Update(float deltaTime, AnimName vname, bool sidex);
	void Update(float deltaTime); // anim is set keep playing it 

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Animation 
	void Play(AnimName anim_name, float deltaTime);
	void setFrameTexture(AnimName anim_name, float deltaTime);
	// Direction
	void SetFacingDirection(float speedx);
	void SetDirection(bool sidex) { a_direction = sidex; }
	inline bool isPlaying() const { return a_isPlaying; }	// not playing and empty frame
	void resetCurrentAnim(AnimName anim_name);
	void setCurrentAnim(AnimName anim_name) { a_current_anim = anim_name; }
	void InitAnimType(); 
	inline AnimName getCurrentAnim() const { return a_current_anim; }
	inline std::string getCurrentAnimName() { return m_dictvfxs[a_current_anim].name; }
	
	// Following Elements 
	void setPosition(BoxCollideable* element) { 
		m_Position = element->GetCenter(); 
		m_Sprite.setPosition(m_Position); 
	}
	inline void setPosition(sf::Vector2f newpos) { 
		m_Position = newpos; 
		m_Sprite.setPosition(m_Position);
	}
	

	sf::Vector2f getPosition() const { return m_Position;  }
	// true = animated once over the object life time
	void setOneTimeAnim(bool onetime) { a_one_time_anim = onetime; }
	bool getOneTimeAnim() const { return a_one_time_anim; }

protected: 

	sf::Sprite m_Sprite; 
	sf::Vector2f m_Size; 
	sf::Vector2f m_Position; 
	float m_Rotation; 
	sf::Vector2f m_Scale; 
	
	// animation 
	AnimName a_current_anim{ AnimName::EmptyFrame };
	std::map< VFX::AnimName, AnimType > m_dictvfxs;
	AllAnims m_vfxs; 
}; 