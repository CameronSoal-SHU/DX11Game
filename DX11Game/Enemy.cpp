#include "Enemy.h"
#include "MainGame.h"
#include "PlayMode.h"


Enemy::Enemy(float _maxHealth, float _curHealth) 
	: GameObject(MainGame::Get().GetD3D()), m_healthHandler(_maxHealth, _curHealth)
{}

void Enemy::Update(float _deltaTime) {
	m_sprite.Update();
}

void Enemy::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);
}


Enemy::~Enemy()
{}
