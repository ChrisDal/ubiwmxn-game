#include <stdafx.h>
#include <Game/SequencePath.h>

SequencePath::SequencePath(sf::Vector2f A, sf::Vector2f B)
	: m_isNavigating{false}, m_StepValidate{false}, m_IsFinished{false}, 
	  m_Point_BL{A}, m_Point_ER{B}
{
	if (std::abs(m_Point_BL.y - m_Point_ER.y) < m_DPX)
	{
		// LinearSequencePath 
		SetLinearSequencePath(m_Point_BL, m_Point_ER); 
	}
	else 
	{
		ProcessSequencePath(m_Point_BL, m_Point_ER); 
	}	
}


void SequencePath::setTargetPoint(sf::Vector2f target_point)
{
	m_Point_ER = target_point; 
}

void SequencePath::SetLinearSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint)
{
	// Init 
	float pointx = StartingPoint.x; 
	int i = 0; 
	m_SequencePath = std::vector<sf::Vector2f> { StartingPoint };
	
	// StartingPoint.x -> EndingPoint.x 
	if (StartingPoint.x < EndingPoint.x ) 
	{
		while (pointx < EndingPoint.x)
		{
			pointx  += m_resDPX;
			m_SequencePath.push_back(sf::Vector2f(pointx, StartingPoint.y)); 
		}
	}
	// EndingPoint.x <- StartingPoint.x  	
	else if (StartingPoint.x > EndingPoint.x) 
	{
		while (pointx > EndingPoint.x)
		{
			pointx  -= m_resDPX;
			m_SequencePath.push_back(sf::Vector2f(pointx, StartingPoint.y)); 
		}
	}
	else
	{
		// do nothing 
	}
	
	m_SequencePath.push_back(EndingPoint); 

}	

// Not implemented 
void SequencePath::ProcessSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint) {};

// Written Instructions
std::vector<SequencePath::Instructions> SequencePath::GetWrittenInstructions()
{
	
	std::vector<SequencePath::Instructions> SequencePathInstructions; 
	
	for (int i=0; i < m_SequencePath.size()-1 ; i++)
	{
		sf::Vector2f diffpoint = m_SequencePath[i] - m_SequencePath[i+1]; 
		
		if (std::abs(diffpoint.y) < 1e-3) 
		{
			// horizontal movement
			if (diffpoint.x < 0.0f)
			{
				// A => B 
				SequencePathInstructions.push_back(Instructions::GoToRight); 
			}
			else if (diffpoint.x > 0.0f) 
			{
				// B <= A  
				SequencePathInstructions.push_back(Instructions::GoToLeft);
			}
			else 
			{
				SequencePathInstructions.push_back(Instructions::NA); 
			}
			
		}
		else if (std::abs(diffpoint.x) < 1e-3) 
		{
			// vertical movement
			if (diffpoint.y < 0.0f)
			{
				// A => B 
				SequencePathInstructions.push_back(Instructions::GoToDown); 
			}
			else if (diffpoint.y > 0.0f) 
			{
				// B <= A 
				SequencePathInstructions.push_back(Instructions::GoToUp);
			}
			else 
			{
				SequencePathInstructions.push_back(Instructions::NA); 
			}	
		}
		else 
		{
			// diagonal movement not handle 
			SequencePathInstructions.push_back(Instructions::NA); 
		}
	}
	
	return SequencePathInstructions;
}


// If point is at m_DPX of Ending Point
bool SequencePath::isAtTargetPoint(sf::Vector2f position) 
{ 
	m_IsFinished = DistanceEuclidienne(position, getTargetPoint()) < m_DPX; 
	return m_IsFinished ; 
}

