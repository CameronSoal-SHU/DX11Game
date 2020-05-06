#include "CharacterBase.h"
#include "PlayMode.h"
#include "MainGame.h"
#include "GameConstants.h"


CharacterBase::CharacterBase()
	: GameObject(MainGame::Get().GetD3D()), 
	m_thrust(MainGame::Get().GetD3D()) {
}


CharacterBase::~CharacterBase() {
	m_ptrPlayMode = nullptr;
}

void CharacterBase::Update(float _deltaTime) {
	// Update sprite every frame
	m_sprite.Update(_deltaTime);
	CheckPlayableBounds();
}

void CharacterBase::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);
}

void CharacterBase::LoadShipTexture(D3D & _d3d, const std::string & _txtrName) {
	m_sprite.SetTexture(_txtrName, *_d3d.GetTextureCache().GetData(_txtrName).ptrTexture);
}

void CharacterBase::LoadThrustTexture(D3D & _d3d) {
	// Texture for ship thrust
	m_thrust.SetTexture(TxtrNames::THRUST_NAME, *_d3d.GetTextureCache().GetData(TxtrNames::THRUST_NAME).ptrTexture);
	m_thrust.GetAnim().Init(0, 3, 15, LOOP);
	m_thrust.SetScale({ 4.f,4.f });

	const float textureCentre = m_thrust.GetRadius().x / m_thrust.GetTextureData().frames.size();

	m_thrust.SetOrigin({ textureCentre, -11.f });
	m_thrust.SetRotation(m_sprite.GetRotation());
	m_thrust.GetAnim().Play(true);
}

void CharacterBase::CheckPlayableBounds() {
	const RECTF playArea = m_ptrPlayMode->GetPlayArea();

	if (m_sprite.GetPos().y < playArea.top) {
		m_sprite.SetPos({ m_sprite.GetPos().x, playArea.top });
	}
	else if (m_sprite.GetPos().y > playArea.bottom) {
		m_sprite.SetPos({ m_sprite.GetPos().x, playArea.bottom });
	}
	if (m_sprite.GetPos().x < playArea.left) {
		m_sprite.SetPos({ playArea.left, m_sprite.GetPos().y });
	}
	else if (m_sprite.GetPos().x > playArea.right) {
		m_sprite.SetPos({ playArea.right, m_sprite.GetPos().y });
	}
}
