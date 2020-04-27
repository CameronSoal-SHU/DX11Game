#include "Weapon.h"
#include "PlayMode.h"


Weapon::Weapon(CharacterBase& _owner)
	: Item(), m_weapModifiers() {
	m_owner = &_owner;
}

Weapon::Weapon(const WeaponStats& _stats, CharacterBase& _owner)
	: m_weapModifiers(_stats) {
	m_owner = &_owner;
}

Weapon::Weapon(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed, CharacterBase& _owner)
	: m_weapModifiers(_dmgMult, _fRateMult, _lifeTime, _projSpeed) {
	m_owner = &_owner;
}


Weapon::~Weapon() {
	delete ptrProjectile;
	ptrProjectile = nullptr;
}

void Weapon::Update(float _deltaTime) {
	if (m_fireDelayRemaining > 0.f)
		m_fireDelayRemaining -= _deltaTime;
}

void Weapon::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	Item::Render(_deltaTime, _sprBatch);
}

void Weapon::FireProjectile(const DirectX::SimpleMath::Vector2& _pos) {
	// Make a copy of the projectile to be used
	std::shared_ptr<Projectile> projectileCopy = std::make_shared<Projectile>(*ptrProjectile);
	const float ownerRot = m_owner->GetSprite().GetRotation();
	const DirectX::SimpleMath::Vector2 projSpeed(m_weapStats.projSpeed, m_weapStats.projSpeed);

	// Set the owner mode of the projectile
	projectileCopy->SetModeOwner(*m_ptrPlayMode);
	// Set the projectiles movement speed
	projectileCopy->SetMoveSpeed(projSpeed);
	// Set the projectiles position to passed argument
	projectileCopy->GetSprite().SetPos(_pos);

	// The velocity of the projectile based on the owners rotation so it fires in a straight line
	const DirectX::SimpleMath::Vector2 rotVel(projSpeed.x * sinf(ownerRot), projSpeed.y * -cosf(ownerRot));
	projectileCopy->SetActive(true);
	// Set the velocity
	projectileCopy->GetSprite().SetVelocity(rotVel);
	projectileCopy->GetSprite().SetScale(m_projectileScale);

	// Store the object in the game object container
	m_ptrPlayMode->AddObj(projectileCopy);
	// Reset the delay
	OnUse();

	// destroy copy reference, it's no longer 
	projectileCopy = nullptr;
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

Weapon::WeaponStats::WeaponStats()
	: damageMult(1.f), fireRateMult(1.f),
	lifeTimeMult(1.f)
{}

Weapon::WeaponStats::WeaponStats(const WeaponStats & _stats)
	: damageMult(_stats.damageMult), fireRateMult(_stats.fireRateMult),
	lifeTimeMult(_stats.lifeTimeMult)
{}

Weapon::WeaponStats::WeaponStats(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed)
	: damageMult(_dmgMult), fireRateMult(_fRateMult), lifeTimeMult(_lifeTime), projSpeedMult(_projSpeed)
{}
