#include "Item.h"
#include "MainGame.h"


Item::Item()
	: GameObject(MainGame::Get().GetD3D()),
	m_d3d(MainGame::Get().GetD3D())
{}


Item::~Item()
{}

void Item::Update(float _deltaTime) {
}

void Item::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);	// Call base class method
}
