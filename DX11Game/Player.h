#pragma once
#include "GameObject.h"

class PlayMode;

class Player : public GameObject
{
public:
	Player(PlayMode& _mainGame);
	void Update(float _deltaTime);
	void Render(DirectX::SpriteBatch& _sprBatch);
private:
	DirectX::SimpleMath::Vector2 m_shipSpeed{ 250.f, 250.f };
	PlayMode& m_playMode;
	void PlayerInput(float _deltaTime);
};