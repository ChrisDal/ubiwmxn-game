#pragma once 
#include <Modules/Routine.h> 

class EatDeadbody : public Routine
{

	static const float EATING_TIMER;

public:

	EatDeadbody() {};
	EatDeadbody(MovableEnnemies* enm, sf::Vector2f targetpoint);
	~EatDeadbody() {};

	void Act(MovableEnnemies* enm, float deltaTime);
	void Act(MovableEnnemies* enm) {}; 
	void Reset() override;
	void Start();

protected:
	void UpdateTimer(float deltaTime);
	void ResetTimer();

private:
	float m_timer{ 0.0f };
	sf::Vector2f m_target;
};