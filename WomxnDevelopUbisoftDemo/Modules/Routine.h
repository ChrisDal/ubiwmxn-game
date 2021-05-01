#pragma once 
#include <Game/MovableEnnemies.h>

// Routine is an abstract class for Actions  
// based on Obviam :  Game AI – An Introduction to Behaviour Trees
// https://obviam.net/posts/2014/02.game-ai-introduction-behaviour-trees/
class Routine
{
public:

	enum class State { Running, Success, Failure, NA };

	Routine() {};
	~Routine() {};

	// Get
	inline State getState() const { return m_state; }

	// Virtual Functions => Actions in derived class: override
	virtual void Act(MovableEnnemies* enm, float deltaTime) {};
	virtual void Reset() {};

	// Routine State 
	inline bool testState(State state) { return m_state == state; }
	inline bool isRunning() { return testState(State::Running); }
	inline bool isFailed() { return testState(State::Failure); }
	inline bool isSuccessfull() { return testState(State::Success); }
	inline bool isNone() { return testState(State::NA); }

protected:
	State m_state{ State::NA };

	inline void Start() { m_state = State::Running; }
	inline void Succeed() { m_state = State::Success; }
	inline void Fail() { m_state = State::Failure; }
};