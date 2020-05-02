#include "Enemy.h"
#include "MainGame.h"
#include "PlayMode.h"
#include "GameConstants.h"

Enemy::Enemy()
	: CharacterBase(), m_thrust(MainGame::Get().GetD3D()) {
	D3D& d3d = MainGame::Get().GetD3D();

	LoadShipTexture(d3d);
	LoadThrustTexture(d3d);

	m_healthHandler.SetMaxHealth(100);
	m_healthHandler.SetCurHealth(100);

	m_sprite.SetScale({0.15f, 0.15f});
	m_sprite.SetOrigin(m_sprite.GetDimRadius());

	m_collider = Collider(m_sprite);
	m_collider.SetTag("ENEMY");
}

void Enemy::Update(float _deltaTime) {
	CharacterBase::Update(_deltaTime);
	m_collider.Update(m_sprite);
	DBOUT("Enemy Collider Pos: " << m_collider.GetPosition().x << ',' << m_collider.GetPosition().y);

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


Enemy::~Enemy()
{}

void Enemy::LoadShipTexture(D3D & _d3d) {
	m_sprite.SetTexture(TxtrNames::ENEMY_RED_NAME, *_d3d.GetTextureCache().GetData(TxtrNames::ENEMY_RED_NAME).ptrTexture);
}

void Enemy::LoadThrustTexture(D3D & _d3d) {
	// Texture for ship thrust
	m_thrust.SetTexture(TxtrNames::THRUST_NAME, *_d3d.GetTextureCache().GetData(TxtrNames::THRUST_NAME).ptrTexture);
	m_thrust.GetAnim().Init(0, 3, 15, LOOP);
	m_thrust.SetScale({ 4.f,4.f });

	const float textureCentre = m_thrust.GetDimRadius().x / m_thrust.GetTextureData().frames.size();

	m_thrust.SetOrigin({ textureCentre, -11.f });
	m_thrust.SetRotation(m_sprite.GetRotation());
	m_thrust.GetAnim().Play(true);
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
