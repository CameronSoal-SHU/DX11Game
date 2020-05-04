#include "Enemy.h"
#include "MainGame.h"
#include "PlayMode.h"
#include "GameConstants.h"

Enemy::Enemy()
	: CharacterBase() 
{}

void Enemy::Update(float _deltaTime) {
	CharacterBase::Update(_deltaTime);
	m_collider.Update(m_sprite);
	//DBOUT("Enemy Collider Pos: " << m_collider.GetPosition().x << ',' << m_collider.GetPosition().y);

	if (m_lookAtPlayer) {
		RotateTowardsPlayer();
	}
	m_sprite.Update(_deltaTime);

	m_thrust.GetAnim().Update(_deltaTime);

	m_thrust.SetPos(m_sprite.GetPos());
	m_thrust.SetRotation(m_sprite.GetRotation());

	if (m_healthHandler.IsDead()) {
		m_ptrPlayMode->RemoveObj(this->to_shared_ptr());
	}
}

void Enemy::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	if (m_sprite.IsMoving()) {
		m_thrust.Draw(_sprBatch);
	}

	GameObject::Render(_deltaTime, _sprBatch);
}

void Enemy::RotateTowardsPlayer() {
	// Angle in radians between the sprites position and the player position
	const DirectX::SimpleMath::Vector2 playerPos = m_ptrPlayMode->FindObj(typeid(Player), true)->GetSprite().GetPos();
	const DirectX::SimpleMath::Vector2 spritePos = m_sprite.GetPos();
	const float deltaY = playerPos.y - spritePos.y;
	const float deltaX = playerPos.x - spritePos.x;
	// Get the angle the enemy has to rotate to face the player, (adjusted 90 degrees)
	const float enemyToPlayerAngle = atan2f(deltaY, deltaX) + 1.57f;

	m_sprite.SetRotation(enemyToPlayerAngle);
}
