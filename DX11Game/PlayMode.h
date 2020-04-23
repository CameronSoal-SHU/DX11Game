#pragma once
#include "ModeBase.h"
#include "Input.h"
#include "Player.h"
#include "GameSettings.h"
#include "MenuNode.h"
#include "GameClock.h"

#include <vector>
#include <memory>

class MainGame;

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

	void AddObj(GameObject* _ptrObj);
	void RemoveObj(GameObject* _ptrObj);

	// Find the first object in container that matches the typeid and active status
	GameObject* FindObj(const std::type_info& _type, bool _isActive);
private:
	// Parallax background layers
	std::vector<Sprite> m_parallaxBGLayers;
	float m_scrollSpeed = 50.f;

	float m_fpsUpdateDelay = 0.5f;
	float m_fpsUpdateDelayRemaining = 0.5f;

	std::vector<GameObject*> m_gameObjs;
	MenuManager& m_menuManager;				// Holds menu manager reference
	MenuNode* m_uiRoot;						// Holds the menu page for the UI root

	GameClock m_inGameClock;				// Track current play time for scaling difficulty

	float screenDimScaleX;
	float screenDimScaleY;
	void UpdateScreenDimScale();

	void SetupBackground();
	void ScrollBackground(float _deltaTime);

	// Create a menu root node for the player UI
	// And setup UI elements
	void SetupPlayerUI();
	void SetupGameClock();
	void SetupHealthbar(D3D& _d3d);
	void SetupHotBar(D3D& _d3d);

	void UpdatePlayerUI(float _deltaTime);
	void UpdateItemHotbar();
	void UpdateHealthBar();
	void UpdateGameClock(float _deltaTime);
};
