#pragma once
#include "GameObject.h"
#include "CharacterBase.h"

// Forward declaration
class PlayMode;

// Projectile class to be created by a Weapon
// On initialisation, a projectile should inherit the item stats
// Passed from the Weapon class
class Projectile : public GameObject
{
public:
	Projectile(const std::string& _txtrName);

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void SetDamage(float _damage);
	void SetStats(const CharacterBase::Stats& _stats) { m_projStats = _stats; }

	void SetModeOwner(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	~Projectile();
protected:
	D3D& m_d3d;
	PlayMode* m_ptrPlayMode;
	CharacterBase::Stats m_projStats;	// A copy of the weapon stats to be used
	float m_projDamage;					// How much damage this projectile does
};

