#pragma once 
#include <Modules/Routine.h> 

class EatDeadbody : public Routine
{

	static float constexpr EATING_TIMER{3.0f};

public:

	EatDeadbody() {};
	EatDeadbody(MovableEnnemies* enm, sf::Vector2f targetpoint);
	~EatDeadbody() {};

	void Act(MovableEnnemies* enm, float deltaTime);
	void Act(MovableEnnemies* enm) {}; 
	void Reset() override;
	void Start();

protected:
	inline void UpdateTimer(float deltaTime) { m_timer += deltaTime; }
	inline void ResetTimer() { m_timer = 0.0f; } 

private:
	float m_timer{ 0.0f };
	sf::Vector2f m_target;
};