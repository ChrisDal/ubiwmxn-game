#include <stdafx.h>
#include <Game/MainCharacter.h>

using namespace sf;

// Joystick helpers
namespace
{
    bool GetFirstJoystickIndex(unsigned int& index)
    {
        index = 0;
        while (index < Joystick::Count)
        {
            if (Joystick::isConnected(index) && Joystick::hasAxis(index, Joystick::Axis::X) && Joystick::hasAxis(index, Joystick::Axis::Y))
                return true;

            index++;
        }

        return false;
    }

    float GetScaledAxis(unsigned int index, sf::Joystick::Axis axis, float deadZone, float scale)
    {
        float value = (Joystick::getAxisPosition(index, axis) / 100.0f) * scale;
        if (value >= -deadZone && value <= deadZone)
            return 0.0f;

        return value;
    }
}


MainCharacter::MainCharacter(sf::Vector2u WIN_LIMITS)
    : m_IsPlayingEndGame(false), m_Position(250.0f, 250.0f), m_IsUsingJoystick(false), m_JoystickIndex(0), m_WasButtonPressed(false)
{
    m_Texture.loadFromFile(".\\Assets\\bulle.png");

    const sf::Vector2f size(static_cast<float>(m_Texture.getSize().x), static_cast<float>(m_Texture.getSize().y));

    m_Sprite.setTexture(m_Texture);
    m_Sprite.setOrigin(size * 0.5f);
    m_Sprite.setPosition(m_Position);

    SetBoundingBox(m_Position, size);

    m_IsUsingJoystick = GetFirstJoystickIndex(m_JoystickIndex);

    // sf vectors for min and max of windows app 
    WIN_LIMIT_X.x = 0;
    WIN_LIMIT_X.y = WIN_LIMITS.x;
    WIN_LIMIT_Y.x = 0;
    WIN_LIMIT_Y.y = WIN_LIMITS.y;

}


void MainCharacter::Update(float deltaTime, TileMap &Tm)
{
    if (m_IsPlayingEndGame)
    {
        return;
    }

    const float SPEED_MAX = 150.0f;
    const float SPEED_INC = 10.0f;
    const float DEAD_ZONE = 5.0f;
    const float SLOWDOWN_RATE = 0.9f;

    if (m_IsUsingJoystick)
    {
        m_Velocity.x = GetScaledAxis(m_JoystickIndex, Joystick::Axis::X, DEAD_ZONE, SPEED_MAX);
        m_Velocity.y = GetScaledAxis(m_JoystickIndex, Joystick::Axis::Y, DEAD_ZONE, SPEED_MAX);

        if (Joystick::isButtonPressed(m_JoystickIndex, 0))
        {
            if (!m_WasButtonPressed)
            {
                m_Sprite.setScale(0.8f, 0.8f);
                m_WasButtonPressed = true;
            }
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_Sprite.setScale(1.0f, 1.0f);
                m_WasButtonPressed = false;
            }
        }
    }
    else
    {
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
            m_Velocity.x = fmin(m_Velocity.x + SPEED_INC, SPEED_MAX);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Left))
        {
            m_Velocity.x = fmax(m_Velocity.x - SPEED_INC, -SPEED_MAX);
        }
        else
        {
            m_Velocity.x *= SLOWDOWN_RATE;
        }

        if (Keyboard::isKeyPressed(Keyboard::Down))
        {
            m_Velocity.y = fmin(m_Velocity.y + SPEED_INC, SPEED_MAX);
        }
        else if (Keyboard::isKeyPressed(Keyboard::Up))
        {
            m_Velocity.y = fmax(m_Velocity.y - SPEED_INC, -SPEED_MAX);
        }
        else
        {
            m_Velocity.y *= SLOWDOWN_RATE;
        }

        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            if (!m_WasButtonPressed)
            {
                m_Sprite.setScale(0.8f, 0.8f);
                m_WasButtonPressed = true;
            }
        }
        else
        {
            if (m_WasButtonPressed)
            {
                m_Sprite.setScale(1.0f, 1.0f);
                m_WasButtonPressed = false;
            }
        }
    }

    m_Position += m_Velocity * deltaTime;

    // Handling out window 
    if (m_Position.x < WIN_LIMIT_X.x or m_Position.x > WIN_LIMIT_X.y) {
        m_Position.x -= m_Velocity.x * deltaTime;
    }
    if (m_Position.y < WIN_LIMIT_Y.x   or m_Position.y > WIN_LIMIT_Y.y) {
        m_Position.y -= m_Velocity.y * deltaTime;
    }

    // handling out not walkable blocks 
    // To Do modify test on 4 corners of the bouding box rather than pos=center
    if (!Tm.walkable_tile(m_Position)) {
        m_Velocity.x = 0;
        m_Velocity.y = 0;
        m_Position -= m_Velocity * deltaTime;
    }

    m_Sprite.setPosition(m_Position);
    SetCenter(m_Position);
}


void MainCharacter::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_Sprite);
}

void MainCharacter::StartEndGame()
{
    m_IsPlayingEndGame = true;
}


sf::Vector2f MainCharacter::getVelocity() const
{
    return MainCharacter::m_Velocity;
}