#include <stdafx.h>
#include <Game/SequencePath.h>

SequencePath::SequencePath(sf::Vector2f A, sf::Vector2f B)
	: m_isNavigating{ false }, m_StepValidate{ false }, m_IsFinished{ false },
	  m_Point_BL{ A }, m_Point_ER{ B }
{
	// Determine on which axis moving 
	setAxisMove();
	// Direction of path
	setDirectionMove();
	
	if (m_NavX or m_NavY) 
	{
		// LinearSequencePath and init of m_it_path
		SetLinearSequencePath(m_Point_BL, m_Point_ER); 
	}
	else 
	{
		ProcessSequencePath(m_Point_BL, m_Point_ER); 
	}	
	
	// init flag reached
	m_FlagReached.assign(m_SequencePath.size(), false);
	// iterator initialisation on point done  in setLinearSequence
	m_it_path = m_SequencePath.begin();
	
	// Set Written Instructions 
	m_PathInstructions = WrittenInstructions(); 
	m_it_instr = m_PathInstructions.begin(); 
	m_CurrentInstruc = *m_it_instr; 
}


void SequencePath::setTargetPoint(sf::Vector2f target_point) { m_Point_ER = target_point; }
void SequencePath::setBeginingPoint(sf::Vector2f beg_point) { m_Point_BL = beg_point; }

// set NavX and Nav Y
void SequencePath::setAxisMove()
{
	// Determine on which axis moving 
	if (std::abs(m_Point_BL.y - m_Point_ER.y) < m_DPX)
	{
		m_NavX = true;
		m_NavY = false;
	}
	else if (std::abs(m_Point_BL.x - m_Point_ER.x) < m_DPX)
	{
		m_NavX = false;
		m_NavY = true;
	}
	else
	{
		// Not implemented => Nav Bool to false 
		m_NavX = false;
		m_NavY = false;
	}

}

// Direction of path
void SequencePath::setDirectionMove()
{
	if (m_NavX) { m_direction = m_Point_ER.x > m_Point_BL.x; }
	if (m_NavY) { m_direction = m_Point_ER.y > m_Point_BL.y; }

	// handling case not handle 
	if ((not m_NavX) and (not m_NavY)) {
		m_direction = false;
	}
}


void SequencePath::SetLinearSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint)
{
	// Init 
	float pointx = StartingPoint.x; 
	float pointy = StartingPoint.y; 
	// init member variables 
	m_SequencePath = std::vector<sf::Vector2f> { StartingPoint };
	setBeginingPoint(StartingPoint);
	setTargetPoint(EndingPoint);
	setAxisMove(); 
	setDirectionMove();
	// iterator initialisation on point
	m_it_path = m_SequencePath.begin();
	
	if (m_NavX) 
	{
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
		// else do nothing 
	}
	else if (m_NavY)
	{
		// StartingPoint.y -> EndingPoint.y 
		if (StartingPoint.y < EndingPoint.y ) 
		{
			while (pointy < EndingPoint.y)
			{
				pointy  += m_resDPX;
				m_SequencePath.push_back(sf::Vector2f(StartingPoint.x, pointy)); 
			}
		}
		// EndingPoint.y <- StartingPoint.y  	
		else if (StartingPoint.y > EndingPoint.y) 
		{
			while (pointy > EndingPoint.y)
			{
				pointy  -= m_resDPX;
				m_SequencePath.push_back(sf::Vector2f(StartingPoint.x, pointy)); 
			}
		}
		// else do nothing 
	}
	else 
	{
		// not implemented 
		printf("Calling a non implemented functionnality: not handling XY movements\n");
	}
	
	m_SequencePath.push_back(EndingPoint); 
	

}	

// Not implemented 
void SequencePath::ProcessSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint) {};



bool SequencePath::isAtTargetPoint(BoxCollideable& CollObj)
{

	if (CollObj.Contains(m_Point_ER))
	{
		m_IsFinished = true; 
		m_isNavigating = false; 
	} 

	return m_IsFinished;
	
}


// Update if sprite reached point
bool SequencePath::UpdateFlags(BoxCollideable& CollObj)
{
	bool reached = false;  
	
	if ((not m_isNavigating) or m_IsFinished)
		
		return reached; 

	// Sprite contains intermediate point then update flag
	if (CollObj.Contains(*m_it_path))
	{
		m_FlagReached[std::distance(m_SequencePath.begin(), m_it_path) - 1] = true; 
		m_it_path++; 
		reached = true;
	}
	return reached; 
}

// Get a written instructions for each update of our sprite 
SequencePath::Instructions SequencePath::GetUpdateInstructions(BoxCollideable& CollObj)
{
	// check if reach next intermediate point 
	bool reached_point = UpdateFlags(CollObj);
	// Reach ending point 
	if(isAtTargetPoint(CollObj))
	{
		m_it_path = m_SequencePath.end() ; 
		m_CurrentInstruc = Instructions::Idle;
		m_it_instr = m_PathInstructions.end(); 
		return m_CurrentInstruc;
	}
	
	// get next intermediate point instructions
	if (reached_point)
	{
		m_it_instr++; 
		m_CurrentInstruc = *m_it_instr; 
	}
	// else keep previous instructions until we reached next point 

	return m_CurrentInstruc; 

}


// Compute Instructions
std::vector<SequencePath::Instructions> SequencePath::WrittenInstructions()
{
	
	std::vector<Instructions> SequencePathInstructions; 
	
	for (int i=0; i < m_SequencePath.size()-1; i++)
	{
		if (m_NavX)
		{
			// A => B 
			if (m_direction)
				SequencePathInstructions.push_back(Instructions::GoToRight); 
			
			else // B <= A  
				SequencePathInstructions.push_back(Instructions::GoToLeft);
			
		}
		else if (m_NavY)
		{
			// A => B 
			if (m_direction)
				SequencePathInstructions.push_back(Instructions::GoToDown); 
			
			else // B <= A  
				SequencePathInstructions.push_back(Instructions::GoToRight);
		}
		else 
		{
			// diagonal movement not handle 
			SequencePathInstructions.push_back(Instructions::NA); 
		}
	}
		
	return SequencePathInstructions;
}




