#pragma once
#include "GameObject.h"

class PlayMode;

class Player : public GameObject
{
public:
	Player();
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;
	// Link object to owning mode
	void SetMode(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; };
private:
	DirectX::SimpleMath::Vector2 m_shipSpeed{ 250.f, 250.f };
	PlayMode* m_ptrPlayMode = nullptr;		// Mode owner of object
	void PlayerInput(float _deltaTime);
};