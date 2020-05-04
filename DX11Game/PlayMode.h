#pragma once
#include "EnemyRedShip.h"
#include "GameSettings.h"
#include "PlayerUI.h"
#include "ItemShopMode.h"

#include <vector>
#include <memory>

class PlayMode : public Singleton<PlayMode>, public ModeBase
{
public:
	std::string GetModeName() const override { return MODE_NAME; }

	PlayMode();
	~PlayMode();
	void Enter() override;
	bool Exit() override;
	// Called once at the very start
	void Start() override;
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	void AddObj(std::shared_ptr<GameObject> _ptrObj);
	void RemoveObj(std::shared_ptr<GameObject> _ptrObj);

	// Find the first object in container that matches the typeid and active status
	// Shared pointer is used to more easily handle allocation/deallocation of memory
	std::shared_ptr<GameObject> FindObj(const std::type_info& _type, bool _isActive);
	// Find all objects in container that matches the typeid and active status
	// Shared pointer is used to more easily handle allocation/deallocation of memory
	std::vector<std::shared_ptr<GameObject>> FindObjs(const std::type_info& _type, bool _isActive);
private:
	static const std::string MODE_NAME;

	MenuManager& m_menuManager;		// Holds menu manager reference
	MenuNode* m_ptrUIRoot;				// Holds the menu page for the UI root

	PlayerUI m_playerUI;			// Players UI

	// Parallax background layers
	std::vector<Sprite> m_parallaxBGLayers;
	float m_scrollSpeed = 50.f;

	std::vector<std::shared_ptr<GameObject>> m_gameObjs;

	void GetPlayerInput(float _deltaTime);

	void SetupBackground();
	void ScrollBackground(float _deltaTime);
};
