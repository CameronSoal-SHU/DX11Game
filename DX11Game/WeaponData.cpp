#include "WeaponData.h"
#include "GameConstants.h"


Unarmed::Unarmed(CharacterBase* _owner)
	: Weapon(_owner) {
	SetProjTextureName(TxtrNames::NULL_TXTR_NAME);
	SetWeapName("Unarmed");
	// Set the fire rate delay to a high amount to ensure object can never be fired.
	// The delay remaining never decrements, but just to be safe
	m_fireDelayRemaining = 9999.f;
}

EnergyBallWeapon::EnergyBallWeapon(CharacterBase* _owner)
	: Weapon(_owner) {
	SetProjTextureName(TxtrNames::ENERGY_BALL_NAME);
	SetProjectileScale({ 0.05f, 0.05f });
	SetWeaponModifiers(m_energyBallMods);

	SetWeapName(m_weapName);
	SetWeapDesc(m_weapDesc);
	m_type = PRIMARY;
}


EnergyBallWeapon::~EnergyBallWeapon() {
	Weapon::~Weapon();
}
