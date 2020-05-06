#pragma once
#include "Projectile.h"


// Weapon class is an extension of the item class
// And can be stored in the players/enemies item slots to be used
// Can spawn a projectile that copies this items stats
class Weapon : public GameObject
{
public:
	enum weap_type { PRIMARY = 0, SECONDARY = 1 };

	// Container for weapon statistics
	struct WeaponModifiers {
		WeaponModifiers();									// Default constructor that initialises "Dummy" stats (Generally everything is just set to 1)
		WeaponModifiers(const WeaponModifiers& _stats);		// Copy Constructor to copy over Weapon Stat
		WeaponModifiers(float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed);

		float damageMult;			// A damage multiplier of the player/enemies total damage dealt
		float fireRateMult;			// A delay between uses, stops spam every single frame
		float lifeTimeMult;			// How long the item stays in-game for before being deleted from PlayModes GameObject container
		float projSpeedMult;		// How fast the projectile can travel
	};

	Weapon(CharacterBase* _owner = nullptr);	// Creates a weapon with zeroed stats
	Weapon(CharacterBase* _owner, const WeaponModifiers& _stats);	// Creates a weapon with given stats
	Weapon(CharacterBase* _owner, float _dmgMult, float _fRateMult, float _lifeTime, float _projSpeed);
	~Weapon();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	// Set the weapons modifiers and update the weapons stats
	void SetWeaponModifiers(const WeaponModifiers& _weapMod);

	// Reset the fire rate delay after every shot
	void ResetFireRate() { m_fireDelayRemaining = 1 / m_weapStats.fireRate; }

	void FireProjectile(const DirectX::SimpleMath::Vector2& _pos);
	bool CanFire() const { return m_fireDelayRemaining <= 0; }	// If the weapon's firerate delay is at or below 0, the weapon can fire again

	std::string GetWeapName() const { return m_weapName; }
	void SetWeapName(const std::string& _name) { m_weapName = _name; }

	weap_type GetWeapType() const { return m_type; }
	std::string GetWeapTypeToString() const { return m_type == PRIMARY ? "PRIMARY" : "SECONDARY"; }
	void SetWeapType(weap_type _type) { m_type = _type; }
	void SetWeapType(int _type) { m_type = static_cast<weap_type>(_type); }

	Projectile* GetProjectile() const { return m_ptrProjectile; }
	void SetProjectile(Projectile& _proj) { m_ptrProjectile = &_proj; }

	std::string GetProjTextureName() const { return m_projTxtrName; }
	void SetProjTextureName(const std::string& _projName);

	// Public accessors for projectile scale
	DirectX::SimpleMath::Vector2 GetProjectileScale() const { return m_projScale; }
	void SetProjectileScale(const DirectX::SimpleMath::Vector2& _scale) { m_projScale = _scale; }

	std::string GetWeapDesc() const { return m_weapDesc; }
	void SetWeapDesc(const std::string& _desc) { m_weapDesc = _desc; }

	CharacterBase& GetOwner() const { return *m_ptrOwner; }
	void SetOwner(CharacterBase& _newOwner) { m_ptrOwner = &_newOwner; UpdateWeaponStats(); }

	void SetModeOwner(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	bool IsCurrentlyOwned() const { return m_ptrOwner != nullptr; }

	// Public accessor for the weapons modifiers
	const WeaponModifiers& GetWeapMods() const { return m_weapModifiers; }
protected:
	weap_type m_type;					 // Does this weapon belong in the primary or secondary slot?

	CharacterBase::Stats m_weapStats;	 // Holds weapon's stats after modifiers have been applied
	float m_fireDelayRemaining;			 // How long is left of the total delay before it can be used again

	Projectile* m_ptrProjectile;		 // A link to the base Projectile class that this weapon will fire
private:
	D3D& m_d3d;
	PlayMode* m_ptrPlayMode = nullptr;	 // Mode owner of object;
	CharacterBase* m_ptrOwner = nullptr; // What Character currently owns this item? (nullptr when owned by no one)

	std::string m_weapName;				 // Name of the weapon
	std::string m_weapDesc;				 // A nice item description
	WeaponModifiers m_weapModifiers;	 // Holds weapon's stat modifiers (These are applied to the weapon stats based on the owners stats)

	std::string m_projTxtrName;					// Projectile texture name in TextureCache
	DirectX::SimpleMath::Vector2 m_projScale;	// Projectile scaling

	// Set the weapon stats based on the owners base stats
	void UpdateWeaponStats();

	void OnUse();
};

