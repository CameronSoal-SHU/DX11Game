#pragma once
#include "Weapon.h"

/*
A weapon that does the incredible thing of literally nothing.
*/
struct Unarmed : public Weapon
{
	Unarmed(CharacterBase* _owner = nullptr);

	// Update and render methods of an unarmed item should not do anything
	void Update(float _deltaTime) override {};
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override {};
};

/* 
Specific struct dedicated to creating the EnergyBall Weapon.
Hold's the weapons stats and automatically assigns them when created.
*/
class EnergyBallWeapon : public Weapon
{
public:
	EnergyBallWeapon(CharacterBase* _owner = nullptr);
	~EnergyBallWeapon();
private:
	WeaponModifiers m_energyBallMods { 
		1.1f, // Damage Multiplier
		7.5f, // Firerate Multiplier
		.33f,  // Lifetime Multiplier
		1.f   // Projectile Speed Multiplier
	};
};


