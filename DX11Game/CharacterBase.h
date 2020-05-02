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

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	// Link object to owning mode
	void SetParentMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; };

	HealthHandler& GetHealthHandler() { return m_healthHandler; }
	Stats GetStats() const { return m_characterStats; }

protected:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	Collider m_hitBox;

	HealthHandler m_healthHandler;
	Stats m_characterStats;					// Stats to be used by items/weapons
};

