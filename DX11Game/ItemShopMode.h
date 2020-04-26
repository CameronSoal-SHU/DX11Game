#pragma once
#include "ModeBase.h"
#include "MenuManager.h"
#include "Player.h"

class MainGame;

class ItemShopMode : public ModeBase
{
public:
	static const std::string MODE_NAME;

	ItemShopMode(D3D& _d3d);
	~ItemShopMode();
	void Enter() override;
	bool Exit() override;
	// Called once at the very start
	void Start();
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	std::string GetModeName() const override { return MODE_NAME; }

	void SetPlayerRef(Player& _player) { m_ptrPlayer = &_player; }
private:
	void GetPlayerInput();

	void SetUpBackground();

	Sprite m_background;

	D3D& m_d3d;

	MenuManager& m_menuManager;		// Holds menu manager reference
	MenuNode* m_uiRoot;				// Holds the menu page for the UI root

	Player* m_ptrPlayer;			// Player reference from MainGame to get and modify stats
};

