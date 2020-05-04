#include "EnemyRedShip.h"
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
	m_sprite.SetOrigin(m_sprite.GetDimRadius());

	m_collider = Collider(m_sprite);
	m_collider.SetTag("ENEMY");
}

void EnemyRedShip::Update(float _deltaTime) {
	Enemy::Update(_deltaTime);
}

void EnemyRedShip::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	Enemy::Render(_deltaTime, _sprBatch);
}