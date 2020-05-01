#pragma once
#include "CharacterBase.h"
#include "HealthHandler.h"

class PlayMode;

class Enemy : public CharacterBase
{
public:
	Enemy();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	// Link object to owning mode
	void SetParentMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	void SetLookAtPlayer(bool _lookAtPlayer) { m_lookAtPlayer = _lookAtPlayer; }
	~Enemy();
protected:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	Sprite m_thrust;

	bool m_lookAtPlayer = false;

	void LoadShipTexture(D3D& _d3d);
	void LoadThrustTexture(D3D& _d3d);

	void RotateTowardsPlayer();
};

