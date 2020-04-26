#pragma once
#include "CharacterBase.h"
#include "EnergyBall.h"

class PlayMode;

class Player : public CharacterBase
{
public:
	Player();
	~Player();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;
	// Link object to owning mode
	void SetParentMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	// std::vector <std::shared_ptr<ProjectileTEMP>> GetWeapons() const { return m_weaponsTEMP; }
	void SetupWeapons();
private:
	std::vector<Weapon*> m_weapons;

	Sprite m_thrust;

	void GetPlayerInput(float _deltaTime);

	void LoadShipTexture(D3D& _d3d);
	void LoadThrustTexture(D3D& _d3d);

	void FirePrimary();
	void FireSecondary();
};