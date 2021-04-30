#pragma once
#include <Modules/Routine.h> 

// Routine send enm to a point , GoTo Instructions for Sequence Path
class MoveToAR : public Routine
{

public:

	MoveToAR() {};
	MoveToAR(sf::Vector2f startpoint, sf::Vector2f targetpoint, MovableEnnemies* enm);
	~MoveToAR() {};

	// override virtual function in Routine 
	void Act(MovableEnnemies* enm, float deltaTime) override;
	void Reset() override;

	// set new ennemie target point 
	void setNewPath(MovableEnnemies* enm);

protected:
	// attributes for moving point 
	sf::Vector2f m_target;
	sf::Vector2f m_start;

private:
	bool m_invertpath{ false };

};