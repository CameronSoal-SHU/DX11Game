#pragma once
#include "GameObject.h"
#include "HealthHandler.h"

class PlayMode;

// Base class to contain playable and non playable character information
// that is shared between the objects.
class CharacterBase : public GameObject
{
public:
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

	HealthHandler GetHealthHandler() const { return m_healthHandler; }
	Stats GetStats() const { return m_characterStats; }

protected:
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object

	HealthHandler m_healthHandler;
	Stats m_characterStats;					// Stats to be used by items/weapons
};

