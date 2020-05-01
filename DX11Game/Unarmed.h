#pragma once
#include "Weapon.h"


class Unarmed : public Weapon
{
public:
	Unarmed(CharacterBase& _owner);

	// Update and render methods of an unarmed item should not do anything
	void Update(float _deltaTime) override {};
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override {};
};