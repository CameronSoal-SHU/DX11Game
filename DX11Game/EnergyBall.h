#pragma once
#include "Weapon.h"
#include "Projectile.h"

class EneryBall;

class EnergyBallProj : public Projectile
{
public:
	EnergyBallProj(const std::string& _txtrName, const CharacterBase::Stats& _stats);
};


class EnergyBall : public Weapon
{
public:
	EnergyBall(CharacterBase& _owner);
	~EnergyBall();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;
};

