#pragma once
#include "PlayMode.h"
#include "GamePadInput.h"

class MainGame
{
public:
	enum class GameState { PLAY };
	GameState m_gameState = GameState::PLAY;

	MainGame(D3D& _d3d);

	D3D& GetD3D();
	
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

	PlayMode m_playMode;
};
