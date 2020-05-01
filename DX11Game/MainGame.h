#pragma once
#include "PlayMode.h"
#include "ItemShopMode.h"

#include "GamePadInput.h"
#include "Singleton.h"
#include "ModeManager.h"
#include "MenuManager.h"

// Main game wrapper
class MainGame : public Singleton<MainGame>
{
public:
	enum class GameState { PLAY };
	GameState m_gameState = GameState::PLAY;

	MainGame(D3D& _d3d);

	D3D& GetD3D() { return m_d3d; };
	ModeManager& GetModeManager() { return m_modeManager; };
	MenuManager& GetMenuManager() { return m_menuManager; }
	
	// Called every frame
	void Update(float _deltaTime);
	// Called after every frame to render elements
	void Render(float _deltaTime);

	// Call all releaseCOMs to free up memory when not in use
	void Release();

	// Ratio of screen dimensions to render dimensions to keep 
	// positioning/scaling constant throughout resizing the window
	float GetScreenDimScaleX() const { return Settings::GAME_RES.x / WindowUtil::Get().GetClientWidth(); }
	float GetScreenDimScaleY() const { return Settings::GAME_RES.y / WindowUtil::Get().GetClientHeight(); }

	// Input devices
	static Input mouseKeyboardInput;
	static GamePadInput gamePad;
private:
	// D3D reference
	D3D& m_d3d;
	TextureCache& m_txtrCache;
	// Sprite batch for sprite data (deletes itself when out of scope)
	std::unique_ptr<DirectX::SpriteBatch> m_ptrSprBatch = nullptr;

	ModeManager m_modeManager;
	MenuManager m_menuManager;
	
	// Pre load all assets into the texture cache for easy use in game/menus
	void PreLoadAssets();
	void LoadBackgroundAssets();
	void LoadFontAssets();
	void LoadUIAssets();
	void LoadPlayerAssets();
	void LoadEnemyAssets();
	void LoadItemAssets();
};
