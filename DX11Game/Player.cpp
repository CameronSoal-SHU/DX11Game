#include "Player.h"
#include "PlayMode.h"
#include "MainGame.h"
#include "GameConstants.h"
#include "GameSettings.h"

Player::Player()
	: GameObject(MainGame::Get().GetD3D()) {

	std::string shipPath = "Ship/ship_test.dds";
	std::string shipTextureName = "player_ship";

	D3D& d3d = MainGame::Get().GetD3D();

	m_sprite.SetTexture(shipTextureName, *d3d.GetTextureCache().LoadTexture(&d3d.GetDevice(),
		shipPath, shipTextureName, true));

	m_sprite.SetScale({ 0.15f, 0.15f });
	m_sprite.SetOrigin(m_sprite.GetTextureData().dimensions / 2.f);

	m_collider = Collider(m_sprite.GetPos() + m_sprite.GetOrigin(), 
		{ m_sprite.GetScreenDimRadius().x / 2.f, m_sprite.GetScreenDimRadius().y });

	MainGame::gamePad.SetDeadZone(0, { 0.15f, 0.15f });
}

void Player::Update(float _deltaTime) {
	PlayerInput(_deltaTime);
	m_sprite.Update();

	//Hit hit = m_collider.IntersectAABB(m_ptrPlayMode->GetBox().GetCollider());
	//if (hit.Collided()) {	// Was there a collision?
	//	// Move player back outside of collider hitbox
	//	m_sprite.SetVelocity(m_sprite.GetVelocity() += { 
	//		(hit.delta.x * hit.normal.x) * (hit.normal.x == 1.f ? -1.f : 1.f), 
	//		(hit.delta.y * hit.normal.y) * (hit.normal.y == 1.f ? -1.f : 1.f)
	//	});
	//}
	//DBOUT("Player Pos: (" << m_sprite.GetTransform().position.x << ", " << m_sprite.GetTransform().position.y << ")");
}

void Player::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	GameObject::Render(_deltaTime, _sprBatch);	// Call base class method
}

void Player::PlayerInput(float _deltaTime) {
	m_sprite.SetVelocity({ 0.f, 0.f });

	const GamePadInput::ControllerData ctrlData = MainGame::gamePad.GetGamePadData(0);

	if (MainGame::gamePad.IsEnabled(0)) {
		// Set velocity to stick direction (invert Y-axis)
		m_sprite.SetVelocity(DirectX::SimpleMath::Vector2 { 
			ctrlData.leftStick.x,	// X-axis movement
			-ctrlData.leftStick.y	// Y-axis movement
		} * m_shipSpeed * _deltaTime);

		float rightStickRotation = atan2f(ctrlData.rightStick.x,
			ctrlData.rightStick.y);

		if (ctrlData.rightStick.Length() >= ctrlData.stickDeadZone.Length()) {
			m_sprite.SetRotation(rightStickRotation);
		}
	}
	else {
		const float shipSpeedXFixed = m_shipSpeed.x * _deltaTime;
		const float shipSpeedYFixed = m_shipSpeed.y * _deltaTime;

		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_W)) {
			m_sprite.SetVelocity(m_sprite.GetVelocity() -= { 0.f, shipSpeedYFixed });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_S)) {
			m_sprite.SetVelocity(m_sprite.GetVelocity() += { 0.f, shipSpeedYFixed });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_A)) {
			m_sprite.SetVelocity(m_sprite.GetVelocity() -= { shipSpeedXFixed, 0.f });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_D)) {
			m_sprite.SetVelocity(m_sprite.GetVelocity() += { shipSpeedXFixed, 0.f });
		}

		// Angle in radians between the sprites position and the mouse position
		DirectX::SimpleMath::Vector2 mousePos = MainGame::mouseKeyboardInput.GetMousePosScaled(true);
		// Get the angle the player has to rotate to face the mouse, (adjusted 90 degrees)
		float playerToMouseAngle = atan2f(mousePos.y - m_sprite.GetPos().y, mousePos.x - m_sprite.GetPos().x) + 1.570796f;
		m_sprite.SetRotation(playerToMouseAngle);
		//DBOUT("player pos: " << m_sprite.GetPos().x << ", " << m_sprite.GetPos().y << ", mouse pos: " << mousePos.x << ", " << mousePos.y);
	}
}
