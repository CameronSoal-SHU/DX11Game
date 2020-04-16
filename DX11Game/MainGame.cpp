#include "MainGame.h"
#include "D3DUtil.h"
#include "CommonStates.h"
#include "WindowUtil.h"

// Static member variable instances
Input MainGame::mouseKeyboardInput;
GamePadInput MainGame::gamePad;


MainGame::MainGame(D3D& _d3d) 
	: m_playMode(*this), m_d3d(_d3d), m_ptrSprBatch(nullptr) {
	m_ptrSprBatch = new DirectX::SpriteBatch(&m_d3d.GetDeviceCtx());
	// Set up main window for KB+M inputs
	mouseKeyboardInput.Init(WindowUtil::Get().GetMainWindow(), MOUSE_VISIBLE, MOUSE_CONFINED);
	gamePad.Init();
}

D3D& MainGame::GetD3D() {
	return m_d3d;
}

void MainGame::Update(float _deltaTime) {
	gamePad.Update();
	gamePad.IsEnabled(0);

	switch (m_gameState)
	{ 
	case GameState::PLAY:
		m_playMode.Update(_deltaTime);
		break;
	default:
		break;
	}
}

void MainGame::Render(float _deltaTime) {
	m_d3d.BeginRender(Colours::Black);

	DirectX::CommonStates dxState(&m_d3d.GetDevice());
	m_ptrSprBatch->Begin(DirectX::SpriteSortMode_Deferred, dxState.NonPremultiplied(), &m_d3d.GetWrapSampler(true));

	switch (m_gameState)
	{
	case GameState::PLAY:
		m_playMode.Render(_deltaTime, *m_ptrSprBatch);
		break;
	default:
		break;
	}

	m_ptrSprBatch->End();

	m_d3d.EndRender();
	mouseKeyboardInput.PostProcess();
}

void MainGame::Release() {
	delete m_ptrSprBatch;
	m_ptrSprBatch = nullptr;
}