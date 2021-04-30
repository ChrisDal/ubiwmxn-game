#include <stdafx.h> 
#include <Modules/EatDeadbody.h>


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
			// launch timer 
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



