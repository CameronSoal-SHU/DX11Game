#pragma once
#include "GameObject.h"
#include "HealthHandler.h"
#include "Collision2D.h"

class PlayMode;

// Base class to contain playable and non playable character information
// that is shared between the objects.
class CharacterBase : public GameObject, public std::enable_shared_from_this<CharacterBase>
{
public:
	std::shared_ptr<CharacterBase> to_shared_ptr() { return shared_from_this(); }

	struct Stats {
		float damage;
		float fireRate;
		float projSpeed;
		float lifeTime;
	};

	CharacterBase();
	~CharacterBase();

	virtual void Init() = 0;

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	// Link object to owning mode
	void SetParentMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; };

	HealthHandler& GetHealthHandler() { return m_healthHandler; }
	Stats& GetStats() { return m_charStats; }

protected:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	Sprite m_thrust;						// Sprite to hold the thrust animation

	HealthHandler m_healthHandler;
	Stats m_charStats;						// Stats to be used by items/weapons

	void virtual LoadShipTexture(D3D& _d3d, const std::string& _txtrName);
	void LoadThrustTexture(D3D& _d3d);		// Load the thrust texture for each ship

	void CheckPlayableBounds();				// Has the character gone out-of-bounds?
};

