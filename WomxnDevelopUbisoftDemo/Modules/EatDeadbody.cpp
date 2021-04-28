#include <stdafx.h> 
#include <Modules/EatDeadbody.h>

const float EatDeadbody::EATING_TIMER = 3.0f;  

EatDeadbody::EatDeadbody(MovableEnnemies* enm, sf::Vector2f targetpoint)
{
	
	m_target = targetpoint; 
	enm->setPath(enm->GetCenter(), m_target); 
	Start(); 
	ResetTimer(); 
}


void EatDeadbody::Act(MovableEnnemies* enm, float deltaTime)
{
	if (isRunning())
	{
		// arrived to deadbody 
		if (enm->getIsFinished())
		{
			if (m_timer < EATING_TIMER)
			{
				UpdateTimer(deltaTime); 
			}
			else 
			{
				Succeed();
				ResetTimer(); 
			}
		}
	}

}


void EatDeadbody::Reset()
{
	Start();
}

void EatDeadbody::Start()
{
	m_state = State::Running; 
	ResetTimer(); 
}

void EatDeadbody::UpdateTimer(float deltaTime)
{
	m_timer += deltaTime; 
}

void EatDeadbody::ResetTimer()
{
	m_timer = 0.0f; 
}


