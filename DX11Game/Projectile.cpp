#include "Projectile.h"
#include "MainGame.h"
#include "PlayMode.h"

Projectile::Projectile(const std::string& _txtrName)
	: GameObject(MainGame::Get().GetD3D()), m_d3d(MainGame::Get().GetD3D()), m_projDamage(1) {
	m_sprite.SetTexture(_txtrName, *m_d3d.GetTextureCache().GetData(_txtrName).ptrTexture);
	m_sprite.SetOrigin(m_sprite.GetRadius());

	// Give projectile a hitbox
	m_collider = Collider(m_sprite);
}

//Projectile::Projectile(const Projectile& _proj) 
//	: GameObject(MainGame::Get().GetD3D()), m_d3d(MainGame::Get().GetD3D()),
//	m_projDamage(_proj.GetDamage()), m_projStats(_proj.GetStats()) {
//	m_sprite = _proj.GetSprite();
//}

void Projectile::Update(float _deltaTime) {
	m_sprite.Update(_deltaTime);
	m_collider.Update(m_sprite);

	if (!CheckForCollision()) {
		m_projStats.lifeTime -= _deltaTime;

		if (m_projStats.lifeTime <= 0.f) {
			m_ptrPlayMode->RemoveObj(this->to_shared_ptr());
		}
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

void Projectile::UpdateProjStats() {
	m_projDamage = m_projStats.damage;
	m_moveSpeed = DirectX::SimpleMath::Vector2(m_projStats.projSpeed, m_projStats.projSpeed);
}

bool Projectile::CheckForCollision() {
	// Get every enemy in-game currently
	std::vector<std::shared_ptr<GameObject>> redShipEnemies = m_ptrPlayMode->FindObjs(typeid(EnemyRedShip), true);
	std::vector<std::shared_ptr<GameObject>> greenShipEnemies = m_ptrPlayMode->FindObjs(typeid(EnemyGreenShip), true);

	std::vector<std::shared_ptr<GameObject>> allEnemies;
	allEnemies.reserve(redShipEnemies.size() + greenShipEnemies.size());
	allEnemies.insert(allEnemies.begin(), redShipEnemies.begin(), redShipEnemies.end());
	allEnemies.insert(allEnemies.begin(), greenShipEnemies.begin(), greenShipEnemies.end());

	bool collision = false;

	// Check each one for a collision
	for (int i(0); i < (int)allEnemies.size(); ++i) {
		Hit hit = m_collider.IntersectAABB(allEnemies.at(i)->GetCollider());

		if (hit.Collided()) {
			collision = true;
			Enemy& collidedEnemy = *std::dynamic_pointer_cast<Enemy>(allEnemies.at(i));

			collidedEnemy.GetHealthHandler().TakeDamage(m_projDamage);
			m_ptrPlayMode->RemoveObj(this->to_shared_ptr());

			DBOUT("COLLISION, Enemy health: " << collidedEnemy.GetHealthHandler().GetCurHealth());
		}
	}

	return collision;
}
