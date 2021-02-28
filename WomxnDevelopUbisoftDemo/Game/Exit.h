#pragma once

class Exit : public sf::Drawable, public BoxCollideable
{
public:
	Exit(float xCenterPos, float yCenterPos);
	~Exit();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void StartEndGame();

private:
	sf::RectangleShape m_Rectangle;
	sf::Texture m_Texture; 
	sf::Vector2f m_TextureSize;

	bool m_IsPlayingEndGame;
};