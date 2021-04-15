#pragma once
#include <cmath>
#include <vector>

#include <Game/Tilemap.h> 

class SequencePath
{
protected:
	// get by instructions
	enum class Instructions {
		GoToRight, GoToLeft,
		GoToUp, GoToDown, NA
	};

public:
	SequencePath(sf::Vector2f A, sf::Vector2f B);
	~SequencePath() {};

	void setTargetPoint(sf::Vector2f target_point);
	sf::Vector2f getTargetPoint() const { return m_Point_ER; }

	std::vector<sf::Vector2f> GetSequencePath() const { return m_SequencePath; }
	std::vector<SequencePath::Instructions> GetWrittenInstructions();
	// Set the predicted SequencePath between 2 points, Linear => Y = const
	void ProcessSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint);
	void SetLinearSequencePath(sf::Vector2f StartingPoint, sf::Vector2f EndingPoint);

	// Conditions checked 
	inline bool isNaviguating() const { return m_isNavigating; }
	// Check if the position is at a specific distance of the ending point
	bool isAtTargetPoint(sf::Vector2f position);

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
	std::vector<sf::Vector2f> m_SequencePath; // SequencePath = a list of point

	// Naviguation 
	bool m_isNavigating;
	bool m_StepValidate;
	bool m_IsFinished;
	// Delta Pixels 
	float m_DPX = 10.0f;
	float m_resDPX = 32.0f;
};