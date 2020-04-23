#include "EnergyBall.h"
#include "GameConstants.h"


EnergyBall::EnergyBall(CharacterBase& _owner)
	: Weapon(1.1f, 1.4f, 1.f, 1.f, _owner)
{
	UpdateWeaponStats();

	m_projTxtrName = TxtrNames::ENERGY_BALL_NAME;
	m_itemName = "Plasma Cannon";

	ptrProjectile = new EnergyBallProj(m_projTxtrName, m_weapStats);

	m_type = PRIMARY;
}


EnergyBall::~EnergyBall() {
	delete ptrProjectile;
	ptrProjectile = nullptr;
}

void EnergyBall::Update(float _deltaTime) {
	Weapon::Update(_deltaTime);
}

void EnergyBall::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	Weapon::Render(_deltaTime, _sprBatch);
}


EnergyBallProj::EnergyBallProj(const std::string& _txtrName, const CharacterBase::Stats& _stats)
 : Projectile(_txtrName) {
	m_projStats = _stats;
}