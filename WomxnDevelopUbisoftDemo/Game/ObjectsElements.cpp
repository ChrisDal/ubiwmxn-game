#include <stdafx.h>
#include <iostream>
#include <Game/ObjectsElements.h>


sf::Texture*  ObjectsElements::m_pTextureAtlas = nullptr;
float ObjectsElements::m_SFX_volume = 20.0f;

ObjectsElements::ObjectsElements(sf::Vector2f& spawn_pos, bool canmove, bool animated, sf::Vector2u& upperleft, unsigned int sx, unsigned int sy, unsigned int elem_type)
	: m_Position(spawn_pos), moving(canmove), _colliding_plateforms(false), m_IsPlayingEndGame(false)
{
	m_size = sf::Vector2f(static_cast<float>(sx), static_cast<float>(sy));

	// Set texture 
	m_Sprite.setTexture(*m_pTextureAtlas);
	m_Sprite.setTextureRect(sf::IntRect(upperleft.x, upperleft.y, 
							static_cast<int>(m_size.x), static_cast<int>(m_size.y)));
	// Set Origin
	m_Sprite.setOrigin(m_size * 0.5f);
	// Set Position
	m_Sprite.setPosition(m_Position);
	// Bounding box
	SetBoundingBox(m_Position, m_size);

	// Animated or not 
	m_animated = animated;
	
	// Object Type : checkpoint or bomb or box
	setObjType(elem_type);
	// Sound assignment 
	setSoundType();

};

ObjectsElements::~ObjectsElements() {};


void ObjectsElements::setObjType(unsigned int elemtype)
{
	switch(elemtype)
	{
		case(101): 
			m_objtype = ObjectType::checkpoint;
			break;
		case(102): 
			m_objtype = ObjectType::checkpoint; 	
			break;
		case(103): 
			m_objtype = ObjectType::checkpoint; 
			break;
		default: 
			m_objtype = ObjectType::decor; 
			break;

	}
	
}

void ObjectsElements::setSoundType()
{
	switch(m_objtype)
	{
		case(ObjectType::checkpoint): 
			
			m_NoSound = false;
			m_soundrpath = "Assets\\Sounds\\checkpoint_01.wav";
			// load sound 
			if (! m_sbuff.loadFromFile(m_soundrpath))
			{
				m_NoSound = true;
				std::cout << "Failed loading sound checkpoint: " << m_soundrpath << "\n";
			}
			// Assign if sound buffer filled
			if (not m_NoSound) 
			{
				m_soundfx.setBuffer(m_sbuff); 
				m_soundfx.setVolume(GetSFXVolume());
				m_soundfx.setLoop(false);
			}
			break;
			
		default: 
			m_NoSound = true;

	}
	
}


void ObjectsElements::StartEndGame()
{
	m_IsPlayingEndGame = true;
}

void ObjectsElements::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Sprite);
}

// Play Sound FX
void ObjectsElements::playSFX()
{
	if (m_NoSound or (m_soundfx.getStatus() == sf::SoundSource::Status::Playing)) { return; }
	m_soundfx.setBuffer(m_sbuff); // re-set buffer 
	m_soundfx.play(); 
}

// Activate Elements : One time activation
void ObjectsElements::Activate()
{
	switch(m_objtype)
	{
	case(ObjectType::checkpoint): 
		m_Sprite.setColor(sf::Color(0, 255, 0));
		playSFX();
		m_activated = true;
		break;
	default: 
		m_Sprite.setColor(sf::Color(0, 255, 0));
		break;
	}	
}


void ObjectsElements::Update(float deltaTime, bool activated) 
{
	if (moving)
	{
		// Do something to update
	}

	if (m_animated)
	{
		if (activated and (not m_activated))
		{
			Activate();
		}
	}
}; 