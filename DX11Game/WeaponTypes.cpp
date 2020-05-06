#include "WeaponTypes.h"
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

	SetWeapName("Plasma Cannon");
	SetWeapDesc("\nFires a volley of plasma projectiles\nat a fast rate,\nat the cost of damage and range.");
	m_type = PRIMARY;
}


EnergyBallWeapon::~EnergyBallWeapon() {
	Weapon::~Weapon();
}

MissileWeapon::MissileWeapon(CharacterBase* _owner) 
	: Weapon(_owner) {
	SetProjTextureName(TxtrNames::MISSILE_NAME);
	SetProjectileScale({ 1.f, 1.f });
	SetWeaponModifiers(m_missileMods);

	SetWeapName("Missile Launcher");
	SetWeapDesc("\nFires a slow moving,\nHigh damaging missile.");

	m_type = PRIMARY;
}

MissileWeapon::~MissileWeapon() {
	Weapon::~Weapon();
}
