#pragma once
#include "Sprite.h"
#include "Collision2D.h"

class MainGame;

class GameObject
{
public:
	GameObject(MainGame& _mainGame);
	void Update(float _deltaTime);
	void Render(DirectX::SpriteBatch& _sprBatch);

	Sprite& GetSprite() {
		return m_sprite;
	}
	Collider& GetCollider() {
		return m_collider;
	}
	~GameObject();
protected:
	D3D& m_d3d;

	Sprite m_sprite;
	Collider m_collider;
private:
};

