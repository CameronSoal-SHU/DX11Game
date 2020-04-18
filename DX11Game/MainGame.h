#pragma once
#include "PlayMode.h"
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

	// Input devices
	static Input mouseKeyboardInput;
	static GamePadInput gamePad;
private:
	// D3D reference
	D3D& m_d3d;
	// Sprite batch for sprite data
	DirectX::SpriteBatch* m_ptrSprBatch = nullptr;
	ModeManager m_modeManager;
	MenuManager m_menuManager;
};
