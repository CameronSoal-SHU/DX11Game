#include "Player.h"
#include "PlayMode.h"
#include "MainGame.h"
#include "GameConstants.h"
#include "GameSettings.h"
#include <math.h>


Player::Player()
	: CharacterBase(),
	m_thrust(MainGame::Get().GetD3D()) {
	D3D& d3d = MainGame::Get().GetD3D();

	/// TEMP ///
	m_characterStats.damage = 10.f;
	m_characterStats.fireRate = 1.f;
	m_characterStats.projSpeed = 1000.f;
	m_characterStats.lifeTime = 1.f;
	m_moveSpeed = DirectX::SimpleMath::Vector2(300.f, 300.f);

	// Set up player textures
	LoadShipTexture(d3d);
	LoadThrustTexture(d3d);

	m_collider = Collider(m_sprite.GetPos() + m_sprite.GetOrigin(), 
		{ m_sprite.GetScreenDimRadius().x / 2.f, m_sprite.GetScreenDimRadius().y });

	MainGame::gamePad.SetDeadZone(0, { 0.15f, 0.15f });
}

Player::~Player() {
	for (size_t i(0); i < m_weapons.size(); ++i)
		delete m_weapons.at(i);

	m_weapons.clear();
}

void Player::Update(float _deltaTime) {
	CharacterBase::Update(_deltaTime);
	m_thrust.GetAnim().Update(_deltaTime);

	m_thrust.SetPos(m_sprite.GetPos());
	m_thrust.SetRotation(m_sprite.GetRotation());

	// Does the player have any weapons equipped to update?
	if (m_weapons.size() != 0) {
		// Update all of the players weapons
		for (size_t i(0); i < m_weapons.size(); ++i) {
			m_weapons.at(i)->Update(_deltaTime);
		}
	}

	GetPlayerInput(_deltaTime);

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

void Player::SetupWeapons() {
	// Set up the primary and secondary weapons, linked to the player object
	//EnergyBall* energyBallWeapPrim = new EnergyBall(*this);
	Weapon::WeaponStats energyBallStats{ 1.1f, 7.5f, 1.f, 1.f };

	Weapon* energyBallWeap = new Weapon(*this, energyBallStats);
	Unarmed* unarmedSec = new Unarmed(*this);


	//energyBallWeapPrim->SetModeOwner(*m_ptrPlayMode);
	energyBallWeap->SetModeOwner(*m_ptrPlayMode);
	unarmedSec->SetModeOwner(*m_ptrPlayMode);

	//energyBallWeapPrim->SetProjectileScale({ 0.05f, 0.05f });
	energyBallWeap->SetItemName("Plasma Cannon");
	energyBallWeap->SetProjTextureName(TxtrNames::ENERGY_BALL_NAME);
	energyBallWeap->SetProjectileScale({ 0.05f, 0.05f });

	m_weapons.push_back(energyBallWeap);
	m_weapons.push_back(unarmedSec);
}

void Player::GetPlayerInput(float _deltaTime) {
	m_sprite.SetVelocity({ 0.f, 0.f });
	DirectX::SimpleMath::Vector2& spriteVel = m_sprite.GetVelocity();
	const GamePadInput::ControllerData ctrlData = MainGame::gamePad.GetGamePadData(0);

	if (ctrlData.port != -1) {
		if (MainGame::Get().gamePad.IsEnabled(ctrlData.port)) {
			// Set velocity to stick direction (invert Y-axis)
			m_sprite.SetVelocity(DirectX::SimpleMath::Vector2(
				ctrlData.leftStick.x,	// X-axis movement
				-ctrlData.leftStick.y	// Y-axis movement
			) * m_moveSpeed);

			const float rightStickRotation = atan2f(ctrlData.rightStick.x,
				ctrlData.rightStick.y);

			if (ctrlData.rightStick.Length() >= ctrlData.stickDeadZone.Length()) {
				m_sprite.SetRotation(rightStickRotation);
			}

			if (ctrlData.leftTrigger) {
				FireWeapon(Weapon::item_type::PRIMARY);
			}
			if (ctrlData.rightTrigger) {
				FireWeapon(Weapon::item_type::SECONDARY);
			}
		}
	}
	else {
		// Player Movement
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_W)) {
			m_sprite.SetVelocity(spriteVel -= { 0.f, m_moveSpeed.y });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_S)) {
			m_sprite.SetVelocity(spriteVel += { 0.f, m_moveSpeed.y });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_A)) {
			m_sprite.SetVelocity(spriteVel -= { m_moveSpeed.x, 0.f });
		}
		if (MainGame::mouseKeyboardInput.IsPressed(GameConsts::KEY_D)) {
			m_sprite.SetVelocity(spriteVel += { m_moveSpeed.x, 0.f });
		}

		if (MainGame::mouseKeyboardInput.GetMouseDown(Input::MouseButton::LMB)) {
			FireWeapon(Weapon::item_type::PRIMARY);
		}
		if (MainGame::mouseKeyboardInput.GetMouseDown(Input::MouseButton::RMB)) {
			FireWeapon(Weapon::item_type::SECONDARY);
		}

		// Angle in radians between the sprites position and the mouse position
		const DirectX::SimpleMath::Vector2 mousePos = MainGame::mouseKeyboardInput.GetMousePosScaled(ABSOLUTE);
		const DirectX::SimpleMath::Vector2 spritePos = m_sprite.GetPos();
		const float deltaY = mousePos.y - spritePos.y;
		const float deltaX = mousePos.x - spritePos.x;
		// Get the angle the player has to rotate to face the mouse, (adjusted 90 degrees)
		const float playerToMouseAngle = atan2f(deltaY, deltaX) + 1.57f;

		m_sprite.SetRotation(playerToMouseAngle);
	}
}

void Player::LoadShipTexture(D3D& _d3d) {
	// Set player texture to ship
	m_sprite.SetTexture(TxtrNames::PLAYER_NAME, *_d3d.GetTextureCache().GetData(TxtrNames::PLAYER_NAME).ptrTexture);

	m_sprite.SetScale({ 0.15f, 0.15f });
	m_sprite.SetOrigin(m_sprite.GetDimRadius());	// Set origin to centre of texture
}

void Player::LoadThrustTexture(D3D & _d3d) {
	// Texture for ship thrust
	m_thrust.SetTexture(TxtrNames::THRUST_NAME, *_d3d.GetTextureCache().GetData(TxtrNames::THRUST_NAME).ptrTexture);
	m_thrust.GetAnim().Init(0, 3, 15, LOOP);
	m_thrust.SetScale({ 4.f,4.f });

	const float textureCentre = m_thrust.GetDimRadius().x / m_thrust.GetTextureData().frames.size();

	m_thrust.SetOrigin({ textureCentre, -11.f });
	m_thrust.SetRotation(m_sprite.GetRotation());
	m_thrust.GetAnim().Play(true);
}

void Player::FireWeapon(const Weapon::item_type& _weaponType) {
	Weapon& weapon = *m_weapons.at(_weaponType);

	if (weapon.CanUse()) {
		const float playerRot = m_sprite.GetRotation();
		const DirectX::SimpleMath::Vector2 offsetPos(m_sprite.GetScreenDimRadius().x * cosf(playerRot),
			m_sprite.GetScreenDimRadius().y * sinf(playerRot));

		// Primary weapon should offset the projectile spawn point to the left of the player, 
		// secondary weapon should be to the right
		weapon.FireProjectile(m_sprite.GetPos() + ((_weaponType == Weapon::item_type::PRIMARY) ? -offsetPos : offsetPos));
	}
}

