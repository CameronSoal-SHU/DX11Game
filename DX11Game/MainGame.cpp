#include "MainGame.h"
#include "D3DUtil.h"
#include "CommonStates.h"
#include "WindowUtil.h"

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
	gamePad.IsEnabled(0);

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
	LoadFontAssets();
	LoadUIAssets();
	LoadPlayerAssets();
	LoadItemAssets();
}

void MainGame::LoadFontAssets() {
	// Load up fonts to be used in game
	m_menuManager.LoadFont(L"fonts/comicSansMS_43.spritefont", "comicsans", 43, APPEND_PATH);
	m_menuManager.LoadFont(L"fonts/bauhaus93Regular_43.spritefont", "bauhaus", 43, APPEND_PATH);
	m_menuManager.LoadFont(L"fonts/algerian.spritefont", "algerian", 12, APPEND_PATH);
}

void MainGame::LoadUIAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();
	const std::vector<TextureCache::Data::Sprite> hpBarFrames{
		{{0,0}, STRETCHED, {0,0,32,32}}
	};

	txtrCache.LoadTexture(&m_d3d.GetDevice(), "UI/health_bar_bg.dds", "ui_health_bar_bg", APPEND_PATH, &hpBarFrames);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), "UI/health_bar_fg.dds", "ui_health_bar_fg", APPEND_PATH, &hpBarFrames);
}

void MainGame::LoadPlayerAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();
	const std::string playerShipPath = "Ship/ship_test.dds";
	const std::string playerTxtrName = "player_ship";
	const std::string thrustPath = "Ship/thrust.dds";
	const std::string thrustTxtrName = "ship_thrust";
	const std::vector<TextureCache::Data::Sprite> thrustFrames {
		{{0, 0}, NOT_STRETCHED, { 0,  0, 15, 16} },
		{{0, 0}, NOT_STRETCHED, { 16, 0, 31, 16} },
		{{0, 0}, NOT_STRETCHED, { 32, 0, 47, 16} },
		{{0, 0}, NOT_STRETCHED, { 48, 0, 64, 16} },
	};

	txtrCache.LoadTexture(&m_d3d.GetDevice(), playerShipPath, playerTxtrName, APPEND_PATH);
	txtrCache.LoadTexture(&m_d3d.GetDevice(), thrustPath, thrustTxtrName, APPEND_PATH, &thrustFrames);
}

void MainGame::LoadItemAssets() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();
	const std::string energyBallPath = "Projectiles/energy_ball.dds";
	const std::string energyBallTxtrName = "proj_energy_ball";

	txtrCache.LoadTexture(&m_d3d.GetDevice(), energyBallPath, energyBallTxtrName, APPEND_PATH);
}
