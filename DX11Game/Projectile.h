#pragma once
#include "GameObject.h"
#include "CharacterBase.h"

// Forward declaration
class PlayMode;

// Projectile class to be created by a Weapon
// On initialisation, a projectile should inherit the item stats
// Passed from the Weapon class
class Projectile : public GameObject, public std::enable_shared_from_this<Projectile>
{
public:
	std::shared_ptr<Projectile> to_shared_ptr() { return shared_from_this(); }
	Projectile(const std::string& _txtrName);
	//Projectile(const Projectile& _proj);

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	float GetDamage() const { return m_projDamage; }
	void SetDamage(float _damage);

	CharacterBase::Stats GetStats() const { return m_projStats; }
	void SetStats(const CharacterBase::Stats& _stats);

	void SetModeOwner(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	void SetHitBox(const Collider& _collider) { m_collider = _collider; }

protected:
	D3D& m_d3d;
	PlayMode* m_ptrPlayMode;

	CharacterBase::Stats m_projStats;	// A copy of the weapon stats to be used
	float m_projDamage;					// How much damage this projectile does

	void UpdateProjStats();

	bool CheckForCollision();
};

