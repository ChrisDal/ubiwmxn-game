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
	State getState() const { return m_state; }

	// Pure Virtual Functions => Actions to make : override
	virtual void Act(MovableEnnemies* enm) = 0;
	virtual void Reset() = 0;

	// Routine State 
	bool testState(State state) { return m_state == state; }
	bool isRunning() { return testState(State::Running); }
	bool isFailed() { return testState(State::Failure); }
	bool isSuccessfull() { return testState(State::Success); }
	bool isNone() { return testState(State::NA); }

protected:
	State m_state{ State::NA };

	void Start() { m_state = State::Running; }
	void Succeed() { m_state = State::Success; }
	void Fail() { m_state = State::Failure; }
};