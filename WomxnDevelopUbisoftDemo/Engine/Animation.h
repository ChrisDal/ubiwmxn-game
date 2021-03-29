#pragma once

class Animation
{
	
public:

	struct AnimType {
		short unsigned int nb_frames_anim; 
		short unsigned int line_anim; 
		short unsigned int a_offset;
		std::string name; 
	};
	
	enum class AnimName {
		Idle, Walk, Jump, DoubleJump,
		Attack, Hurt, Die, Dodge,
		Surprise, Reborn
	};


	// entire file for animation
	void setMainTexture(const sf::Texture charatexture) { *a_spritesheet = charatexture; }
	sf::Texture* getMainTexture() const { return a_spritesheet; }

	// Animations : Play, Pause, Stop
	void Play(AnimName anim_name, float deltaTime);
	void Stop();
	void Pause();

	// Define frame to show : texture 
	void setFrameTexture(AnimName anim_name, float deltaTime);

	// Is a animation is playing 
	inline void setPlaying(const bool& playing) { a_isPlaying = playing; };
	// Facing to the right or to the left 
	void setFacingDirection(float speedx);
	// Set the framerate for animation 
	inline void setFrameRate(short unsigned int framer) { a_framerate = framer; };

	// Animation set and get 
	std::string getAnimName();
	inline AnimName getCurrentAnim() const { return a_current_anim; }
	inline void setCurrentAnim(AnimName anim_name) { a_current_anim = anim_name; }

	// Texture
	void setTextureOffset(sf::Vector2i offset_texture) { a_textsquare_offset  = offset_texture;}
	sf::Vector2i getTextureOffset() const { return a_textsquare_offset;}


protected:

	sf::Texture* a_spritesheet;
	short unsigned int a_framecount = 0; 			// frame counter
	short unsigned int a_framerate = 10; 			// anim = 10 fps
	unsigned int a_frametexture = 0; 			    // index of the frame texture
	sf::Vector2i a_textsquare_offset{ 0,0 };		// offset in frame texture 
	float a_sumdeltaTime = 0.0; 					// sum of time past in current anim
	bool a_direction{ true }; 						// direction=true:right, false:left
	bool a_isPlaying{ false }; 						// Animation is currently playing
	AnimName a_current_anim{ AnimName::Idle }; 		// animation currently playing
	bool a_one_time_anim{ false };					// To use for one time animation


};
