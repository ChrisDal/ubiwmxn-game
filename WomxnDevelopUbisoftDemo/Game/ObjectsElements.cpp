#include <iostream>
#include <stdafx.h>
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
	// Sound and Visual assignment 
	setFXType();

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

void ObjectsElements::setFXType()
{
	switch(m_objtype)
	{
		case(ObjectType::checkpoint): 
			// SOUND
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
			// VISUAL 
			m_vfx = VFX(m_Position, true);
			m_vfx_mirrored = true;
			if (m_vfx_mirrored) { m_vfx_mirror = VFX(m_Position, true); }
			m_vfxname = VFX::AnimName::DemiCircularActivation; 
			break;
		default: 
			m_NoSound = true;
			// empty fx
			m_vfx = VFX(m_Position, true);
			m_vfx_mirrored = false;
			m_vfxname = VFX::AnimName::EmptyFrame;
			
	}
	
}


void ObjectsElements::StartEndGame()
{
	m_IsPlayingEndGame = true;
}

void ObjectsElements::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_Sprite);
	target.draw(m_vfx);
	if (m_vfx_mirrored)
	{
		target.draw(m_vfx_mirror);
	}
}

// Play Sound FX
void ObjectsElements::playSFX()
{
	if (m_NoSound or (m_soundfx.getStatus() == sf::SoundSource::Status::Playing)) { return; }
	m_soundfx.setBuffer(m_sbuff); // re-set buffer 
	m_soundfx.play(); 
}

// Play vfx animations associated to m_vfx (mirrored)
void ObjectsElements::setVFX()
{
	sf::Vector2f right_pos = { 0.0f, 0.0f };
	sf::Vector2f left_pos = { 0.0f, 0.0f };
	// visual
	right_pos.x = this->GetCenter().x + this->m_BoundingBox.width;
	right_pos.y = this->GetCenter().y - this->m_BoundingBox.height / 3.0f;

	m_vfx.setSpriteParameters(right_pos, 0.0f, sf::Vector2f(2.0f, 2.0f));

	if (m_vfx_mirrored)
	{
		left_pos.x = this->GetCenter().x - this->m_BoundingBox.width;
		left_pos.y = this->GetCenter().y - this->m_BoundingBox.height / 3.0f;

		m_vfx_mirror.setSpriteParameters(left_pos, 0.0f, sf::Vector2f(2.0f, 2.0f));
	}
};

// Activate Elements : One time activation
void ObjectsElements::Activate(float deltaTime)
{
	bool vfx_call = (not m_activated) or (m_vfx.getCurrentAnim() != VFX::AnimName::EmptyFrame);
	
	switch (m_objtype)
	{
	case(ObjectType::checkpoint):
		
		if (not m_activated)
		{
			m_Sprite.setColor(sf::Color(0, 255, 0));
			// sound 
			playSFX();
			// set parameters for VFX
			setVFX();
			m_activated = true;
		}

		if (vfx_call)
		{
			// VFX time dependant
			m_vfx.Update(deltaTime, m_vfxname, true);
			if (m_vfx_mirrored)
			{
				m_vfx_mirror.Update(deltaTime, m_vfxname, false);
			}
		}
		break;
	default:
		m_Sprite.setColor(sf::Color(0, 255, 0));
	};
};


void ObjectsElements::Update(float deltaTime, bool activated) 
{
	if (moving)
	{
		// Do something to update
	}

	if (m_animated)
	{
		if (activated or m_activated)
		{
			// Call activation of animation
			Activate(deltaTime);
		}
	}
}; 