#pragma once
#include "ModeBase.h"
#include "Input.h"
#include "Player.h"
#include "Enemy.h"
#include "GameSettings.h"
#include "MenuNode.h"
#include "GameClock.h"
#include "PlayerUI.h"

#include <vector>
#include <memory>

class MainGame;
class ItemShopMode;

class PlayMode : public ModeBase
{
public:
	static const std::string MODE_NAME;

	PlayMode();
	~PlayMode();
	void Enter() override;
	bool Exit() override;
	// Called once at the very start
	void Start();
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	std::string GetModeName() const override { return MODE_NAME; }

	void AddObj(std::shared_ptr<GameObject> _ptrObj);
	void RemoveObj(std::shared_ptr<GameObject> _ptrObj);

	// Find the first object in container that matches the typeid and active status
	// Shared pointer is used to more easily handle allocation/deallocation of memory
	std::shared_ptr<GameObject> FindObj(const std::type_info& _type, bool _isActive);
private:
	void GetPlayerInput(float _deltaTime);

	// Parallax background layers
	std::vector<Sprite> m_parallaxBGLayers;
	float m_scrollSpeed = 50.f;

	float m_fpsUpdateDelay = 0.5f;
	float m_fpsUpdateDelayRemaining = 0.5f;

	std::vector<std::shared_ptr<GameObject>> m_gameObjs;
	MenuManager& m_menuManager;						// Holds menu manager reference
	std::shared_ptr<ItemShopMode> m_ptrItemShop;	// Holds reference to in-game item shop
	MenuNode* m_uiRoot;								// Holds the menu page for the UI root

	PlayerUI m_playerUI;	// Players UI

	void SetupBackground();
	void ScrollBackground(float _deltaTime);
};
