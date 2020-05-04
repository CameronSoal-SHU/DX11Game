#pragma once
#include "Enemy.h"

// Inherited enemy class for the red ship enemy type 
class EnemyRedShip : public Enemy
{
public:
	EnemyRedShip();
	void Init() override;
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;
private:
};

