#pragma once
#include "Sprite.h"
#include "Collision2D.h"

class GameObject
{
public:
	GameObject(D3D& _d3d) : m_sprite(_d3d) {};
	virtual ~GameObject() {};

	virtual void Update(float _deltaTime) = 0;
	virtual void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
		if (m_isActive) {
			m_sprite.Draw(_sprBatch);
		}
	};

	Sprite& GetSprite() { return m_sprite; }
	Collider& GetCollider() { return m_collider; }

	bool IsActive() { return m_isActive; }
	void SetActive(bool _isActive) { m_isActive = _isActive; }
protected:
	Sprite m_sprite;
	Collider m_collider;
	bool m_isActive = false;
};

