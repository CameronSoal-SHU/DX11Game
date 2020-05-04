#include "Weapon.h"
#include "MainGame.h"
#include "PlayMode.h"


Weapon::Weapon(CharacterBase* _owner)
	: GameObject(MainGame::Get().GetD3D()),
	m_d3d(MainGame::Get().GetD3D()), m_weapModifiers() {
	m_owner = _owner;

	UpdateWeaponStats();
	m_fireDelayRemaining = 0.f;
}

Weapon::Weapon(CharacterBase* _owner, const WeaponModifiers& _stats)
	: GameObject(MainGame::Get().GetD3D()),
	m_d3d(MainGame::Get().GetD3D()), m_weapModifiers(_stats) {
	m_owner = _owner;

	UpdateWeaponStats();
	m_fireDelayRemaining = 0.f;
}

Weapon::Weapon(CharacterBase* _owner, float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed)
	: GameObject(MainGame::Get().GetD3D()),
	m_d3d(MainGame::Get().GetD3D()),
	m_weapModifiers(_dmgMult, _fRateMult, _lifeTime, _projSpeed) {
	m_owner = _owner;

	UpdateWeaponStats();
	m_fireDelayRemaining = 0.f;
}

// Clean up weapon stats to avoid memory leakage
Weapon::~Weapon() {
	delete m_ptrProjectile;
	m_ptrProjectile = nullptr;
}

void Weapon::Update(float _deltaTime) {
	if (m_fireDelayRemaining > 0.f)
		m_fireDelayRemaining -= _deltaTime;
}

void Weapon::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);
}

void Weapon::FireProjectile(const DirectX::SimpleMath::Vector2& _pos) {
	// Make a copy of the projectile to be used
	std::shared_ptr<Projectile> projectileCopy = std::make_shared<Projectile>(*m_ptrProjectile);
	projectileCopy->GetSprite().SetTextureData(m_ptrProjectile->GetSprite().GetTextureData());

	const float ownerRot = m_owner->GetSprite().GetRotation();
	const DirectX::SimpleMath::Vector2 projSpeed(m_weapStats.projSpeed, m_weapStats.projSpeed);
	// The velocity of the projectile based on the owners rotation so it fires in a straight line
	const DirectX::SimpleMath::Vector2 rotVel(projSpeed.x * sinf(ownerRot), projSpeed.y * -cosf(ownerRot));

	// Set the owner mode of the projectile
	projectileCopy->SetModeOwner(*m_ptrPlayMode);
	// Set the projectiles stats
	projectileCopy->SetStats(m_weapStats);
	// Set the projectiles position to passed argument
	projectileCopy->GetSprite().SetPos(_pos);
	// Set the velocity
	projectileCopy->GetSprite().SetVelocity(rotVel);

	projectileCopy->GetSprite().SetScale(m_projectileScale);

	projectileCopy->SetActive(true);

	projectileCopy->GetCollider().SetHitboxRadius(projectileCopy->GetSprite().GetScreenDimRadius());

	// Store the object in the game object container
	m_ptrPlayMode->AddObj(projectileCopy);
	// Reset the delay
	OnUse();

	//// destroy copy reference, it's no longer needed
	//projectileCopy = nullptr;
}

void Weapon::SetProjTextureName(const std::string& _projName) {
	m_projTxtrName = _projName;

	m_ptrProjectile = new Projectile(_projName);
}

void Weapon::SetWeaponModifiers(const WeaponModifiers & _weapMod) {
	m_weapModifiers = _weapMod;
	UpdateWeaponStats();
}

void Weapon::UpdateWeaponStats() {
	if (m_owner != nullptr) {
		const CharacterBase::Stats& ownerStats = m_owner->GetStats();

		m_weapStats.damage = ownerStats.damage * m_weapModifiers.damageMult;
		m_weapStats.fireRate = ownerStats.fireRate * m_weapModifiers.fireRateMult;
		m_fireDelayRemaining = 1.f / m_weapStats.fireRate;
		m_weapStats.projSpeed = ownerStats.projSpeed * m_weapModifiers.projSpeedMult;
		m_weapStats.lifeTime = ownerStats.lifeTime * m_weapModifiers.lifeTimeMult;
	}
}

// Resets the fire rate delay when called
void Weapon::OnUse() {
	ResetFireRate();
}

Weapon::WeaponModifiers::WeaponModifiers()
	: damageMult(1.f), fireRateMult(1.f),
	lifeTimeMult(1.f), projSpeedMult(1.f)
{}

Weapon::WeaponModifiers::WeaponModifiers(const WeaponModifiers & _stats)
	: damageMult(_stats.damageMult), fireRateMult(_stats.fireRateMult),
	lifeTimeMult(_stats.lifeTimeMult), projSpeedMult(_stats.projSpeedMult)
{}

Weapon::WeaponModifiers::WeaponModifiers(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed)
	: damageMult(_dmgMult), fireRateMult(_fRateMult), lifeTimeMult(_lifeTime), projSpeedMult(_projSpeed)
{}
