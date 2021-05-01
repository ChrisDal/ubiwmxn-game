#include <stdafx.h>
#include <Game/MovableEnnemies.h> 

sf::Texture*  MovableEnnemies::m_pTextureAtlas = nullptr;
float MovableEnnemies::m_SFX_volume = 18.0f;

// constructors
MovableEnnemies::MovableEnnemies(sf::Vector2f SpawnPosition, unsigned int sx, unsigned sy, sf::Vector2u& upperleft, E_Direction pathDirection = E_Direction::UP, int pathLengthInTiles = 0)
	: m_Position(SpawnPosition), m_weak_fire{ false },
	_colliding_fire(false), _colliding_plateforms(false), 
	m_Dead(false) , m_PathDirection(pathDirection), m_PathLengthInTiles(pathLengthInTiles)
{
	m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	m_Sprite.setTexture(*m_pTextureAtlas);
	m_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y,
		static_cast<int>(m_size.x),
		static_cast<int>(m_size.y)));
	// Set Origin
	m_Sprite.setOrigin(m_size * 0.5f);
	// Set Position
	m_Sprite.setPosition(m_Position);
	// Bounding box
	SetBoundingBox(m_Position, m_size);
	// velocity 
	m_Velocity = { 0.0f, 0.0f };
	// Neighbourhood : size character + deltapixels/2 (as it is centered ) 
	m_neighb.setNeighbourhood(this, 64.0f, 10.0f);

	// Sound init 
	InitSoundType(); 

};

MovableEnnemies::MovableEnnemies()
	: m_weak_fire{ false }, m_Dead(false),
	_colliding_fire(false), _colliding_plateforms(false)
{};

sf::Vector2f MovableEnnemies::ComputeTargetPoint()
{
	sf::Vector2f TargetPoint{ GetCenter() };

	switch(m_PathDirection)
	{
		case E_Direction::UP:		TargetPoint.y -= 32.0f * m_PathLengthInTiles;  break;
		case E_Direction::RIGHT:	TargetPoint.x += 32.0f * m_PathLengthInTiles;  break;
		case E_Direction::DOWN:		TargetPoint.y += 32.0f * m_PathLengthInTiles;  break;
		case E_Direction::LEFT:		TargetPoint.x -= 32.0f * m_PathLengthInTiles;  break;
	}

	return TargetPoint;
}


bool MovableEnnemies::setPath(sf::Vector2f beginpoint, sf::Vector2f endpoint)
{
	
	m_Path = SequencePath(beginpoint, endpoint);
	return true;
	
}


void MovableEnnemies::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void MovableEnnemies::Update(float deltaTime)
{
	static const float VELOCITY_MAX = 100.0f; 
	static const float NO_VELOCITY  = 0.0f; 

	SequencePath::Instructions instruc =  m_Path.GetUpdateInstructions(*this); 
	
	switch (instruc)
	{
	case(SequencePath::Instructions::GoToRight):
		m_Velocity = { VELOCITY_MAX, NO_VELOCITY };
		break;
	case(SequencePath::Instructions::GoToLeft):
		m_Velocity.x = -VELOCITY_MAX;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::GoToUp):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = -VELOCITY_MAX;
		break;
	case(SequencePath::Instructions::GoToDown):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = VELOCITY_MAX;
		break;
	case(SequencePath::Instructions::Idle):
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::NA):
		// problem
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	case(SequencePath::Instructions::WaitingInstruc):
		// pause
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	default:
		// not handle 
		m_Velocity.x = NO_VELOCITY;
		m_Velocity.y = NO_VELOCITY;
		break;
	}

	// new position 
	m_Position += m_Velocity * deltaTime; 
	// Set New position
	m_Sprite.setPosition(m_Position);
	SetCenter(m_Position);

	if (m_rotated)
	{
		m_Sprite.rotate(-15.0f);
	}
	
}

void MovableEnnemies::EatPerso()
{
	if (m_soundfx.getStatus() == sf::SoundSource::Status::Stopped)
	{
		setSoundType(AnimName::Eat);
		playSFX(AnimName::Eat);
	}
}

// Object in neighbourhood 
bool MovableEnnemies::ObjectInRange(const BoxCollideable& dbd)
{
	if (m_neighb.IsColliding(dbd))
	{
		return true; 
	}
	else
	{
		return false; 
	}
}

void MovableEnnemies::configurePatrol(sf::Vector2f start, sf::Vector2f end)
{
	m_patrol_start = start; 
	m_patrol_end = end;
}

void MovableEnnemies::StopEatingPerso()
{
	if (m_soundfx.getStatus() == sf::SoundSource::Status::Playing)
	{
		m_soundfx.stop(); 
	}
}

void MovableEnnemies::InitSoundType()
{
	/*
	struct SoundType {
		sf::SoundBuffer s_buffer;       // sound buffer
		bool is_playing{ false };       // sound is playing
		bool no_sound{ true };          // = sound not loaded
		std::string pathsound{ "" };    // path to wav sound file
		bool looping{ false };          // looping or not
	};
	*/

	LoadStructSound(m_AllSounds.Eat, "Assets\\Sounds\\13173.wav", true);
	
	// default initialisation for the others
	dictSound[AnimName::Idle] = m_AllSounds.Idle;
	dictSound[AnimName::Walk] = m_AllSounds.Walk;
	dictSound[AnimName::Jump] = m_AllSounds.Jump;
	dictSound[AnimName::Eat]  = m_AllSounds.Eat;
	dictSound[AnimName::Die]  = m_AllSounds.Die;

}

void MovableEnnemies::LoadStructSound(SoundType& onesound, const std::string soundpath, bool looping)
{
	onesound.no_sound = not onesound.s_buffer.loadFromFile(soundpath);
	onesound.is_playing = false;
	onesound.pathsound = soundpath;
	onesound.looping = looping;
}

inline void MovableEnnemies::setSoundType(AnimName anim)
{
	if (not dictSound[anim].no_sound)
	{
		m_soundfx.setBuffer(dictSound[anim].s_buffer);
		m_soundfx.setVolume(GetSFXVolume());
		m_soundfx.setLoop(dictSound[anim].looping);
	}
}

inline void MovableEnnemies::playSFX(AnimName anim)
{
	if (dictSound[anim].no_sound) { return; }
	m_soundfx.setBuffer(dictSound[anim].s_buffer); // re-set buffer 
	m_soundfx.play();
	dictSound[anim].is_playing = true;
}

void MovableEnnemies::resetPlaying()
{
	dictSound[AnimName::Idle].is_playing = false;
	dictSound[AnimName::Walk].is_playing = false;
	dictSound[AnimName::Jump].is_playing = false;
	dictSound[AnimName::Eat].is_playing  = false;
	dictSound[AnimName::Die].is_playing  = false;
}

