#include <stdafx.h>
#include <Modules/MoveToAR.h>

// to Move to cpp 
MoveToAR::MoveToAR(sf::Vector2f startpoint, sf::Vector2f targetpoint, MovableEnnemies* enm)
{
	m_start = startpoint; 
	m_target = targetpoint;
	enm->setPath(m_start, m_target); 
	Start(); // launch routine 
}


void MoveToAR::Act(MovableEnnemies* enm, float deltaTime)
{
	
	// works if routine is running 
	if (isRunning())
	{
		// if dead stops
		if (! enm->GetIsDead())
		{
			this->Fail(); 
			return; 
		}
		
		// if target point reached
		if (enm->getIsFinished())
		{
			setNewPath(enm);
		}
	}
}


void MoveToAR::setNewPath(MovableEnnemies* enm)
{
	if (m_invertpath)
	{
		// B towards A 
		enm->setPath(m_target, m_start); 
	}
	else 
	{
		// A towards B 
		enm->setPath(m_start, m_target); 
	}
	m_invertpath = not m_invertpath; 
}


void MoveToAR::Reset()
{
	Start();
}