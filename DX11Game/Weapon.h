#pragma once
#include "Item.h"
#include "Projectile.h"
#include <memory>

// Weapon class is an extension of the item class
// And can be stored in the players/enemies item slots to be used
// Can spawn a projectile that copies this items stats
class Weapon : public Item
{
public:
	// Container for weapon statistics
	struct WeaponStats {
		WeaponStats();								// Default constructor that initialises "Dummy" stats (Generally everything is just set to 1)
		WeaponStats(const WeaponStats& _stats);		// Copy Constructor to copy over Weapon Stat
		WeaponStats(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed);

		float damageMult;			// A damage multiplier of the player/enemies total damage dealt
		float fireRateMult;			// A delay between uses, stops spam every single frame
		float lifeTimeMult;			// How long the item stays in-game for before being deleted from PlayModes GameObject container
		float projSpeedMult;		// How fast the projectile can travel
	};

	Weapon(CharacterBase& _owner);
	Weapon(const WeaponStats& _stats, CharacterBase& _owner);
	Weapon(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed, CharacterBase& _owner);
	~Weapon();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void FireProjectile(const DirectX::SimpleMath::Vector2& _pos);

	DirectX::SimpleMath::Vector2 GetProjectileScale() const { return m_projectileScale; }
	void SetProjectileScale(const DirectX::SimpleMath::Vector2& _scale) { m_projectileScale = _scale; }

	void UpdateWeaponStats();
	void ResetFireRate() { m_fireDelayRemaining = 1 / m_weapStats.fireRate; }

	void OnUse() override;
	bool CanUse() { return m_fireDelayRemaining <= 0; }

	Projectile* ptrProjectile;			// A link to the base Projectile class that this weapon will fire

protected:
	WeaponStats m_weapModifiers;		// Holds weapon's stat modifiers

	CharacterBase::Stats m_weapStats;	// Holds weapon's stats after modifiers have been applied
	float m_fireDelayRemaining;			// How long is left of the total delay before it can be used again

	std::string m_projTxtrName;			// Projectile texture name in TextureCache
	DirectX::SimpleMath::Vector2 m_projectileScale;
};

