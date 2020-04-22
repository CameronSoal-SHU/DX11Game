#pragma once
#include "Sprite.h"
#include "Collision2D.h"
#include <memory>

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

	DirectX::SimpleMath::Vector2 GetMoveSpeed() const { return m_moveSpeed; }
	void SetMoveSpeed(const DirectX::SimpleMath::Vector2& _newSpeed) { m_moveSpeed = _newSpeed; }

	bool IsActive() const { return m_isActive; }
	void SetActive(bool _isActive) { m_isActive = _isActive; }
protected:
	Sprite m_sprite;

	DirectX::SimpleMath::Vector2 m_moveSpeed;	// How fast is the GameObject travelling/sec?

	Collider m_collider;
	bool m_isActive = false;
};

