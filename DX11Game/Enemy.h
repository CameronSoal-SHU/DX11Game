#pragma once
#include "GameObject.h"
#include "HealthHandler.h"

class PlayMode;

class Enemy : public GameObject
{
public:
	Enemy(float _maxHealth = 0.f, float _curHealth = 0.f);
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	// Link object to owning mode
	void SetParentMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; };
	~Enemy();
private:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	HealthHandler m_healthHandler;
};

