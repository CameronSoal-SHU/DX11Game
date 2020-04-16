#include "GameObject.h"
#include "MainGame.h"

GameObject::GameObject(MainGame & _mainGame) 
	: m_sprite(_mainGame.GetD3D()), m_d3d(_mainGame.GetD3D())
{}

void GameObject::Update(float _deltaTime) {
	m_sprite.Update();
	m_collider.Update(m_sprite);
}

void GameObject::Render(DirectX::SpriteBatch& _sprBatch) {
	m_sprite.Draw(_sprBatch);
}

GameObject::~GameObject()
{
}
