#include "EnemyTypes.h"
#include "MainGame.h"
#include "GameConstants.h"

EnemyRedShip::EnemyRedShip()
	: Enemy() {
	Init();
}

void EnemyRedShip::Init() {
	D3D& d3d = MainGame::Get().GetD3D();

	LoadShipTexture(d3d, TxtrNames::ENEMY_RED_NAME);
	LoadThrustTexture(d3d);

	m_healthHandler.SetMaxHealth(100);
	m_healthHandler.SetCurHealth(100);

	m_sprite.SetScale({ 0.15f, 0.15f });
	m_sprite.SetOrigin(m_sprite.GetRadius());

	m_collider = Collider(m_sprite);
	m_collider.SetTag("ENEMY");
}

void EnemyRedShip::Update(float _deltaTime) {
	Enemy::Update(_deltaTime);
}

void EnemyRedShip::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	Enemy::Render(_deltaTime, _sprBatch);
}

EnemyGreenShip::EnemyGreenShip()
	: Enemy() {
	Init();
}

void EnemyGreenShip::Init() {
	D3D& d3d = MainGame::Get().GetD3D();

	LoadShipTexture(d3d, TxtrNames::ENEMY_GREEN_NAME);
	LoadThrustTexture(d3d);

	m_healthHandler.SetMaxHealth(100);
	m_healthHandler.SetCurHealth(100);

	m_sprite.SetScale({ .5f, .5f });
	m_sprite.SetOrigin(m_sprite.GetRadius());

	m_collider = Collider(m_sprite);
	m_collider.SetTag("ENEMY");
}

void EnemyGreenShip::Update(float _deltaTime) {
	Enemy::Update(_deltaTime);
}

void EnemyGreenShip::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	Enemy::Render(_deltaTime, _sprBatch);
}