#include "Player.h"
#include "PlayMode.h"
#include "MainGame.h"
#include "GameConstants.h"
#include "GameSettings.h"

Player::Player(float _maxHealth, float _curHealth)
	: GameObject(MainGame::Get().GetD3D()), m_healthHandler(_maxHealth, _curHealth),
	m_thrust(MainGame::Get().GetD3D()) {
	D3D& d3d = MainGame::Get().GetD3D();

	m_thrustFrames = {
		{{0, 0}, NOT_STRETCHED, { 0,  0, 15, 16} },
		{{0, 0}, NOT_STRETCHED, { 16, 0, 31, 16} },
		{{0, 0}, NOT_STRETCHED, { 32, 0, 47, 16} },
		{{0, 0}, NOT_STRETCHED, { 48, 0, 64, 16} },
	};

	LoadShipTexture(d3d);
	LoadThrustTexture(d3d);

	m_collider = Collider(m_sprite.GetPos() + m_sprite.GetOrigin(), 
		{ m_sprite.GetScreenDimRadius().x / 2.f, m_sprite.GetScreenDimRadius().y });

	MainGame::gamePad.SetDeadZone(0, { 0.15f, 0.15f });
}

void Player::Update(float _deltaTime) {
	PlayerInput(_deltaTime);
	m_sprite.Update();
	m_thrust.GetAnim().Update(_deltaTime);

	m_thrust.SetPos(m_sprite.GetPos());
	m_thrust.SetRotation(m_sprite.GetRotation());

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
	if (m_sprite.IsMoving()) {
		m_thrust.Draw(_sprBatch);
	}
	GameObject::Render(_deltaTime, _sprBatch);	// Call base class method
}

HealthHandler& Player::GetHealthHandler() {
	return m_healthHandler;
}

void Player::PlayerInput(float _deltaTime) {
	m_sprite.SetVelocity({ 0.f, 0.f });

	const GamePadInput::ControllerData ctrlData = MainGame::gamePad.GetGamePadData(0);

	if (MainGame::gamePad.IsEnabled(0)) {
		// Set velocity to stick direction (invert Y-axis)
		m_sprite.SetVelocity(DirectX::SimpleMath::Vector2 { 
			ctrlData.leftStick.x,	// X-axis movement
			-ctrlData.leftStick.y	// Y-axis movement
		} * m_moveSpeed * _deltaTime);

		const float rightStickRotation = atan2f(ctrlData.rightStick.x,
			ctrlData.rightStick.y);

		if (ctrlData.rightStick.Length() >= ctrlData.stickDeadZone.Length()) {
			m_sprite.SetRotation(rightStickRotation);
		}
	}
	else {
		const float shipSpeedXFixed = m_moveSpeed.x * _deltaTime;
		const float shipSpeedYFixed = m_moveSpeed.y * _deltaTime;

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
		const float playerToMouseAngle = atan2f(mousePos.y - m_sprite.GetPos().y, mousePos.x - m_sprite.GetPos().x) + 1.570796f;
		m_sprite.SetRotation(playerToMouseAngle);
	}
}

void Player::LoadShipTexture(D3D& _d3d) {
	// File path and texture name for ship
	const std::string shipPath = "Ship/ship_test.dds";
	const std::string shipTextureName = "player_ship";

	// Load up the texture
	ID3D11ShaderResourceView* ptrShipSpr = _d3d.GetTextureCache().LoadTexture(&_d3d.GetDevice(),
		shipPath, shipTextureName, APPEND_PATH);

	// Set player texture to ship
	m_sprite.SetTexture(shipTextureName, *ptrShipSpr);

	m_sprite.SetScale({ 0.15f, 0.15f });
	m_sprite.SetOrigin(m_sprite.GetDimRadius());	// Set origin to centre of texture
}

void Player::LoadThrustTexture(D3D & _d3d) {
	// File path and texture name for ship thrust
	const std::string thrustPath = "Ship/thrust.dds";
	const std::string thrustTextureName = "ship_thrust";

	ID3D11ShaderResourceView* ptrShipThrust = _d3d.GetTextureCache().LoadTexture(&_d3d.GetDevice(),
		thrustPath, thrustTextureName, APPEND_PATH, &m_thrustFrames);

	m_thrust.SetTexture(thrustTextureName, *ptrShipThrust);
	m_thrust.GetAnim().Init(0, 3, 15, LOOP);
	m_thrust.SetScale({ 4.f,4.f });

	const float textureCentre = m_thrust.GetDimRadius().x / m_thrust.GetTextureData().frames.size();

	m_thrust.SetOrigin({ textureCentre, -11.f });
	m_thrust.SetRotation(m_sprite.GetRotation());
	m_thrust.GetAnim().Play(true);
}
