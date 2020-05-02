#include "CharacterBase.h"
#include "PlayMode.h"
#include "MainGame.h"


CharacterBase::CharacterBase()
	: GameObject(MainGame::Get().GetD3D()) {
}


CharacterBase::~CharacterBase() {
	m_ptrPlayMode = nullptr;
}

void CharacterBase::Update(float _deltaTime) {
	// Update sprite every frame
	m_sprite.Update(_deltaTime);
}

void CharacterBase::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);
}
