#pragma once
#include "GameObject.h"

class PlayMode;

class Player : public GameObject
{
public:
	Player();
	Player(float _maxHealth, float _curHealth);
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;
	// Link object to owning mode
	void SetMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; };

	const float GetMaxHealth() const { return m_playerHealth.at(0); }
	const float GetrCurHealth() const { return m_playerHealth.at(1); }
private:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	DirectX::SimpleMath::Vector2 m_movementSpeed{ 250.f, 250.f };

	std::vector<float> m_playerHealth;	// Indices: 0 = Max Health | 1 = Current Health
	void PlayerInput(float _deltaTime);
};