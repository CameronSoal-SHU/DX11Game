#pragma once
#include "CharacterBase.h"
#include "WeaponTypes.h"

class Player : public CharacterBase
{
public:
	Player();
	~Player();
	
	void Init() override;
	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void SetupWeapons();
	std::vector<Weapon*> GetWeapons() const { return m_weapons; }
	void SetWeapon(Weapon::weap_type _type, Weapon* _weap);
private:
	std::vector<Weapon*> m_weapons;

	//Sprite m_thrust;

	void GetPlayerInput(float _deltaTime);

	void LoadShipTexture(D3D& _d3d, const std::string& _txtrName) override;

	void CheckForCollision();

	void FireWeapon(Weapon::weap_type _weaponType);
};