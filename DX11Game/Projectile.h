#pragma once
#include "GameObject.h"
#include "CharacterBase.h"
#include "Collision2D.h"

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

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void SetDamage(float _damage);
	void SetStats(const CharacterBase::Stats& _stats);

	void SetModeOwner(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	void SetHitBox(const Collider& _collider) { m_collider = _collider; }

	~Projectile();
protected:
	D3D& m_d3d;
	PlayMode* m_ptrPlayMode;

	//Collider m_hitBox;

	CharacterBase::Stats m_projStats;	// A copy of the weapon stats to be used
	float m_projDamage;					// How much damage this projectile does

	void UpdateProjStats();
	bool CheckForCollision();
};

