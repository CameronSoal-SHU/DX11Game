#include "Player.h"
#include "PlayMode.h"
#include "MainGame.h"
#include "GameConstants.h"
#include "GameSettings.h"
#include <math.h>


Player::Player()
	: CharacterBase() {
	D3D& d3d = MainGame::Get().GetD3D();

	// Set up player textures
	LoadShipTexture(d3d, TxtrNames::PLAYER_NAME);
	LoadThrustTexture(d3d);

	/*m_collider = Collider(m_sprite.GetPosOrigin(), 
		{ m_sprite.GetScreenDimRadius().x / 2.f, m_sprite.GetScreenDimRadius().y });*/

	m_collider = Collider(m_sprite);
	m_collider.SetTag("PLAYER");

	MainGame::gamePad.SetDeadZone(0, { 0.15f, 0.15f });

	Init();
}

Player::~Player() {
	for (size_t i(0); i < m_weapons.size(); ++i)
		if (m_weapons.at(i))
			delete m_weapons.at(i);

	m_weapons.clear();
}

void Player::Init() {
	/// TEMP ///
	m_charStats.damage = 10.f;
	m_charStats.fireRate = 1.f;
	m_charStats.projSpeed = 1000.f;
	m_charStats.lifeTime = 1.f;
	m_moveSpeed = DirectX::SimpleMath::Vector2(300.f, 300.f);
}

void Player::Update(float _deltaTime) {
	GetPlayerInput(_deltaTime);
	CharacterBase::Update(_deltaTime);
	m_collider.Update(m_sprite);
	CheckForCollision();
	m_thrust.GetAnim().Update(_deltaTime);

	//DBOUT("Player pos: " << m_sprite.GetPos().x << ',' << m_sprite.GetPos().y);

	m_thrust.SetPos(m_sprite.GetPos());
	m_thrust.SetRotation(m_sprite.GetRotation());

	// Does the player have any weapons equipped to update?
	if (m_weapons.size() != 0) {
		// Update all of the players weapons
		for (size_t i(0); i < m_weapons.size(); ++i) {
			m_weapons.at(i)->Update(_deltaTime);
		}
	}

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
	EnergyBallWeapon* eBallWeap = new EnergyBallWeapon(this);
	Unarmed* unarmedSec = new Unarmed(this);

	eBallWeap->SetModeOwner(*m_ptrPlayMode);
	unarmedSec->SetModeOwner(*m_ptrPlayMode);

	m_weapons.push_back(eBallWeap);
	m_weapons.push_back(unarmedSec);
}

void Player::SetWeapon(Weapon::weap_type _type, Weapon* _weap) {
	delete m_weapons.at(_type);
	m_weapons.at(_type) = nullptr;

	m_weapons.at(_type) = _weap;
	m_weapons.at(_type)->SetOwner(*this);
}

void Player::GetPlayerInput(float _deltaTime) {
	m_sprite.SetVelocity({ 0.f, 0.f });
	DirectX::SimpleMath::Vector2 spriteVel = m_sprite.GetVelocity();
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
				FireWeapon(Weapon::weap_type::PRIMARY);
			}
			if (ctrlData.rightTrigger) {
				FireWeapon(Weapon::weap_type::SECONDARY);
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
			FireWeapon(Weapon::weap_type::PRIMARY);
		}
		if (MainGame::mouseKeyboardInput.GetMouseDown(Input::MouseButton::RMB)) {
			FireWeapon(Weapon::weap_type::SECONDARY);
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

void Player::LoadShipTexture(D3D& _d3d, const std::string& _txtrName)
{
	CharacterBase::LoadShipTexture(_d3d, _txtrName);	// Call base method
	m_sprite.SetScale({ 0.15f, 0.15f });				// Adjust player ships scale to a more reasonable size
	m_sprite.SetOrigin(m_sprite.GetRadius());		// Set the origin to the centre of the players texture
}

void Player::CheckForCollision() {
	std::vector<std::shared_ptr<GameObject>> enemies = m_ptrPlayMode->FindObjs(typeid(Enemy), true);

	for (size_t i(0); i < enemies.size(); ++i) {
		Hit hit = m_collider.IntersectAABB(enemies.at(i)->GetCollider());

		if (hit.Collided()) {
			DBOUT("COLLISION");
		}
	}
}

void Player::FireWeapon(Weapon::weap_type _weaponType) {
	Weapon& weapon = *m_weapons.at(_weaponType);

	if (weapon.CanFire()) {
		const float playerRot = m_sprite.GetRotation();
		const DirectX::SimpleMath::Vector2 offsetPos(m_sprite.GetScreenRadius().x * cosf(playerRot),
			m_sprite.GetScreenRadius().y * sinf(playerRot));

		// Primary weapon should offset the projectile spawn point to the left of the player, 
		// secondary weapon should be to the right
		weapon.FireProjectile(m_sprite.GetPos() + ((_weaponType == Weapon::weap_type::PRIMARY) ? -offsetPos : offsetPos));
	}
}

