#include "MainGame.h"
#include "D3DUtil.h"
#include "CommonStates.h"
#include "WindowUtil.h"
#include "GameConstants.h"

// Static member variable instances
Input MainGame::mouseKeyboardInput;
GamePadInput MainGame::gamePad;


MainGame::MainGame(D3D& _d3d) 
	: m_d3d(_d3d), m_ptrSprBatch(nullptr) {
	// Set up main window for KB+M inputs
	mouseKeyboardInput.Init(WindowUtil::Get().GetMainWindow(), MOUSE_VISIBLE, MOUSE_CONFINED);
	gamePad.Init();
	m_ptrSprBatch = std::make_unique<DirectX::SpriteBatch>(DirectX::SpriteBatch(&m_d3d.GetDeviceCtx()));

	PreLoadAssets();

	m_modeManager.AddMode(std::make_shared<PlayMode>());
	m_modeManager.ChangeMode(PlayMode::MODE_NAME);
}

void MainGame::Update(float _deltaTime) {
	gamePad.Update();
	m_modeManager.Update(_deltaTime);
}

void MainGame::Render(float _deltaTime) {
	m_d3d.BeginRender(Colours::Black);

	DirectX::CommonStates dxState(&m_d3d.GetDevice());
	m_ptrSprBatch->Begin(DirectX::SpriteSortMode_Deferred, dxState.NonPremultiplied(), &m_d3d.GetWrapSampler());

	m_modeManager.Render(_deltaTime, *m_ptrSprBatch);
	m_menuManager.Render(_deltaTime, *m_ptrSprBatch, m_d3d.GetTextureCache(), mouseKeyboardInput);

	m_ptrSprBatch->End();

	m_d3d.EndRender();
	mouseKeyboardInput.PostProcess();
}

void MainGame::Release() {
	m_modeManager.Release();
	m_ptrSprBatch = nullptr;
}

void MainGame::PreLoadAssets() {
#pragma omp parallel sections
	{
#pragma omp section
		{ LoadFontAssets(); }
#pragma omp section
		{ LoadBackgroundAssets(); }
#pragma omp section
		{ LoadUIAssets(); }
#pragma omp section
		{ LoadPlayerAssets(); }
#pragma omp section
		{ LoadItemAssets(); }
	}
}

void MainGame::LoadBackgroundAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	// Scrolling space background and stars
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::SPACE_BG_PATH, TxtrNames::SPACE_BG_NAME, APPEND_PATH);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::SPACE_BG_STARS_PATH, TxtrNames::SPACE_BG_STARS_NAME, APPEND_PATH);

	// Item Shop background
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::SHOP_BG_PATH, TxtrNames::SHOP_BG_NAME, APPEND_PATH);
}

void MainGame::LoadFontAssets() {
	// Load up fonts to be used in game
	m_menuManager.LoadFont(L"fonts/comicSansMS_43.spritefont", "comicsans", 43, APPEND_PATH);
	m_menuManager.LoadFont(L"fonts/bauhaus93Regular_43.spritefont", "bauhaus", 43, APPEND_PATH);
	m_menuManager.LoadFont(L"fonts/algerian.spritefont", "algerian", 12, APPEND_PATH);
}

void MainGame::LoadUIAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	// UI Health bar
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::HEALTH_BAR_BG_PATH, TxtrNames::HEALTH_BAR_BG_NAME, APPEND_PATH, &TxtrFrames::HEALTH_BAR_FRAMES);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::HEALTH_BAR_FG_PATH, TxtrNames::HEALTH_BAR_FG_NAME, APPEND_PATH, &TxtrFrames::HEALTH_BAR_FRAMES);

	// UI Item Hotbar
	//txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::ITEM_HOTBAR_PATH, TxtrNames::ITEM_HOTBAR_NAME, APPEND_PATH, &TxtrFrames::ITEM_HOTBAR_FRAMES);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::ITEM_HOTBAR_BG_PATH, TxtrNames::ITEM_HOTBAR_BG_MAME, APPEND_PATH, &TxtrFrames::ITEM_HOTBAR_FRAMES);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::ITEM_HOTBAR_SLOT_PATH, TxtrNames::ITEM_HOTBAR_SLOT_NAME, APPEND_PATH, &TxtrFrames::ITEM_HOTBAR_FRAMES);
}

void MainGame::LoadPlayerAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	// Player sprites
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::PLAYER_PATH, TxtrNames::PLAYER_NAME, APPEND_PATH);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::THRUST_PATH, TxtrNames::THRUST_NAME, APPEND_PATH, &TxtrFrames::THRUST_TXTR_FRAMES);
}

void MainGame::LoadItemAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	// Energy ball item
	txtrCache.LoadTexture(&m_d3d.GetDevice(), TxtrDirs::ENERGY_BALL_PATH, TxtrNames::ENERGY_BALL_NAME, APPEND_PATH, &TxtrFrames::ENERGY_BALL_FRAMES);
}
