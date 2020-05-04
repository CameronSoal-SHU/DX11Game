#pragma once
#include "CharacterBase.h"
#include "HealthHandler.h"
#include "Weapon.h"

class Enemy : public CharacterBase
{
public:
	Enemy();
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void SetLookAtPlayer(bool _lookAtPlayer) { m_lookAtPlayer = _lookAtPlayer; }
protected:
	Weapon* m_weapon;

	bool m_lookAtPlayer = false;

	void RotateTowardsPlayer();
};

