#include "Projectile.h"
#include "MainGame.h"
#include "PlayMode.h"


Projectile::Projectile(const std::string& _txtrName)
	: GameObject(MainGame::Get().GetD3D()), m_d3d(MainGame::Get().GetD3D()), m_projDamage(1) {
	m_sprite.SetTexture(_txtrName, *m_d3d.GetTextureCache().GetData(_txtrName).ptrTexture);
	m_sprite.SetOrigin(m_sprite.GetDimRadius());
}

void Projectile::Update(float _deltaTime) {
	m_sprite.Update(_deltaTime);
	m_projStats.lifeTime -= _deltaTime;

	if (m_projStats.lifeTime <= 0.f) {
		m_ptrPlayMode->RemoveObj(this->to_shared_ptr());
	}
}

void Projectile::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);	// Call base class method
}

void Projectile::SetDamage(float _damage) {
	m_projDamage = _damage;
}

void Projectile::SetStats(const CharacterBase::Stats & _stats) {
	m_projStats = _stats;
	UpdateProjStats();
}

Projectile::~Projectile()
{}

void Projectile::UpdateProjStats() {
	m_projDamage = m_projStats.damage;
	m_moveSpeed = DirectX::SimpleMath::Vector2(m_projStats.projSpeed, m_projStats.projSpeed);
}
