#pragma once
#include <cmath>
#include <vector>

#include <Engine/Collision.h> 
#include <Game/Tilemap.h> 

class SequencePath
{
protected:
	// get by instructions
	enum class Instructions {
		GoToRight, GoToLeft,
		GoToUp, GoToDown, 
		GoToJumpLeft, GoToJumpRight, 
		Idle, NA,
	};

public:
	SequencePath(sf::Vector2f A, sf::Vector2f B);
	~SequencePath() {};

	void setTargetPoint(sf::Vector2f target_point);
	sf::Vector2f getTargetPoint() const { return m_Point_ER; }	
	void setBeginingPoint(sf::Vector2f beg_point);
	sf::Vector2f getBeginingPoint() const { return m_Point_BL; }

	std::vector<sf::Vector2f> GetSequencePath() const { return m_SequencePath; }
	std::vector<SequencePath::Instructions> WrittenInstructions();
	std::vector<SequencePath::Instructions> GetSequenceInstruc() { return m_PathInstructions; }
	// Set the predicted SequencePath between 2 points, Linear => one axis = const
	void ProcessSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint);
	void SetLinearSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint);

	// Conditions checked 
	inline bool isNaviguating() const { return m_isNavigating; }
	// Check if the position is at a specific distance of the ending point
	bool isAtTargetPoint(BoxCollideable& CollObj);

	// get Naviguation axis and direction 
	void setAxisMove(); 
	void setDirectionMove(); 
	
	// Follow BoxCollideable and get next instructions
	SequencePath::Instructions GetUpdateInstructions(BoxCollideable& CollObj); 
	bool UpdateFlags(BoxCollideable& CollObj);

	// Distance Calculation 
	inline float DistanceEuclidienne(sf::Vector2f A, sf::Vector2f B)
	{
		return static_cast<float>(std::sqrt(pow(static_cast<double>(A.x) - static_cast<double>(B.x), 2)
			+ pow(static_cast<double>(A.y) - static_cast<double>(B.y), 2)));
	};

	float DistanceManhattan(sf::Vector2f A, sf::Vector2f B, TileMap Tilemap);
	void setDeltaPixels(float pixels) { m_DPX = pixels; }
	


protected:
	sf::Vector2f m_Point_BL; // begining point
	sf::Vector2f m_Point_ER; // ending point 
	// SequencePath
	std::vector<sf::Vector2f> m_SequencePath; 			// SequencePath = a list of point
	std::vector<Instructions> m_PathInstructions;		// SequencePath = a list of point
	std::vector<bool> m_FlagReached; 		  			// is each point reached

	// Naviguation 
	bool m_isNavigating;	// Do not reach point but is ok
	bool m_StepValidate;	// NA 
	bool m_IsFinished;		// Reached point
	bool m_NavX{true}; 		// naviguate on x axis only 
	bool m_NavY{false}; 	// naviguate on y axis only 
	bool m_direction; 		// true: left to right, false : right to left
	
	// Naviguating 
	std::vector<sf::Vector2f>::iterator m_it_path; 
	std::vector<Instructions>::iterator m_it_instr; 
	Instructions m_CurrentInstruc;
	
	// Delta Pixels 
	float m_DPX = 10.0f;	// Delta pixels 
	float m_resDPX = 32.0f;	// Game TileSize
	
};