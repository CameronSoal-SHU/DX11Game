#include "PlayMode.h"
#include "GameConstants.h"
#include "MainGame.h"
#include "GameSettings.h"
#include "ItemShopMode.h"
#include <algorithm>

const std::string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode()
	: m_menuManager(MainGame::Get().GetMenuManager()), 
	screenDimScaleX((Settings::GAME_RES.x * Settings::GAME_RES.x) / WindowUtil::Get().GetClientWidth()),
	screenDimScaleY((Settings::GAME_RES.y * Settings::GAME_RES.y) / WindowUtil::Get().GetClientHeight()),
	m_playerUI(*this) {
	m_gameObjs.reserve(1000);

	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->SetParentMode(*this);
	player->SetActive(true);
	player->SetupWeapons();
	AddObj(player);

	m_ptrItemShop = std::make_shared<ItemShopMode>(MainGame::Get().GetD3D());
	MainGame::Get().GetModeManager().AddMode(m_ptrItemShop);

	Start();
}

PlayMode::~PlayMode() {
	m_parallaxBGLayers.clear();
	m_gameObjs.clear();
}

void PlayMode::Enter() {
	MainGame::Get().GetMenuManager().ShowMenu("menu_player_UI");
}

bool PlayMode::Exit() {
	MainGame::Get().GetMenuManager().HideMenu();
	return true;
}

void PlayMode::Start() {
	SetupBackground();
	SetupPlayerUI();
}

void PlayMode::Update(float _deltaTime) {
	GetPlayerInput(_deltaTime);

	m_inGameClock.Update(_deltaTime);

	ScrollBackground(_deltaTime);

	// Call all game objects update methods
	for (auto it = m_gameObjs.begin(); it < m_gameObjs.end(); ++it) {
		(*it)->Update(_deltaTime);
	}

	// Update player UI
	m_playerUI.Update(_deltaTime);

	UpdateScreenDimScale();
}

void PlayMode::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	// Draw each background element back-most first
	for (auto it = m_parallaxBGLayers.begin(); it < m_parallaxBGLayers.end(); ++it) {
		it->Draw(_sprBatch);
	}

	// Draw each game object on screen
	for (auto it = m_gameObjs.begin(); it < m_gameObjs.end(); ++it) {
		(*it)->Render(_deltaTime, _sprBatch);
	}
}

void PlayMode::AddObj(std::shared_ptr<GameObject> _ptrObj) {
	assert(_ptrObj);

	m_gameObjs.push_back(_ptrObj);
}

void PlayMode::RemoveObj(std::shared_ptr<GameObject> _ptrObj) {
	size_t objSize = m_gameObjs.size();
	assert(objSize > 0);					// Make sure there's object that can be deleted

	m_gameObjs.erase(std::remove(m_gameObjs.begin(), m_gameObjs.end(), _ptrObj));
	assert(objSize != m_gameObjs.size());	// Check if the object has been deleted
}

std::shared_ptr<GameObject> PlayMode::FindObj(const std::type_info & _type, bool _isActive) {
	size_t i(0);

	// Search for object in list
	while (i < m_gameObjs.size() && 
		(typeid(*m_gameObjs.at(i)) != _type || m_gameObjs.at(i)->IsActive() != _isActive)) {
		++i;
	}

	// Return the object if anything was found
	return (i < m_gameObjs.size() ? m_gameObjs.at(i) : nullptr);
}

void PlayMode::GetPlayerInput(float _deltaTime) {
	if (MainGame::mouseKeyboardInput.IsPressed(VK_TAB)) {
		if (m_canSwitchMode) {
			m_canSwitchMode = false;
			MainGame::Get().GetModeManager().ChangeMode(ItemShopMode::MODE_NAME);
		}
	}
	else {
		m_canSwitchMode = true;
	}
}

void PlayMode::UpdateScreenDimScale() {
	screenDimScaleX = (Settings::GAME_RES.x * Settings::GAME_RES.x) / WindowUtil::Get().GetClientWidth();
	screenDimScaleY = (Settings::GAME_RES.y * Settings::GAME_RES.y) / WindowUtil::Get().GetClientHeight();
	DBOUT(screenDimScaleX << ", " << screenDimScaleY);
}

void PlayMode::SetupBackground() {
	D3D& d3d = MainGame::Get().GetD3D();

	// Allocate memory for each background sprite
	for (unsigned i(0); i < 2; ++i) {
		m_parallaxBGLayers.push_back(Sprite(d3d));
	}

	// Load parallax layer textures
	m_parallaxBGLayers.at(0).SetTexture(TxtrNames::SPACE_BG_NAME, *d3d.GetTextureCache().GetData(TxtrNames::SPACE_BG_NAME).ptrTexture);
	m_parallaxBGLayers.at(1).SetTexture(TxtrNames::SPACE_BG_STARS_NAME, *d3d.GetTextureCache().GetData(TxtrNames::SPACE_BG_STARS_NAME).ptrTexture);

	// Set scale to fit window size
	for (auto it = m_parallaxBGLayers.begin(); it < m_parallaxBGLayers.end(); ++it) {
		it->SetScale({ 3.5f, 3.5f });
	}
}

void PlayMode::ScrollBackground(float _deltaTime) {
	// Scroll each background at different rates(closer = faster) to add a parallax effect
	for (size_t i(0); i < m_parallaxBGLayers.size(); ++i) {
		Sprite& bgElement = m_parallaxBGLayers.at(i);
		const float paraScrollRate = _deltaTime * m_scrollSpeed * (i + 1);

		RECTF scrollRect{ 0, paraScrollRate, 0, paraScrollRate };

		bgElement.SetTextureRect(bgElement.GetTextureRect() - scrollRect);
	}
}

void PlayMode::SetupPlayerUI() {
	D3D& d3d = MainGame::Get().GetD3D();

	//SetupHealthbar(d3d);
	//SetupHotBar(d3d);
	//SetupGameClock();
}

void PlayMode::SetupGameClock() {
	// Create text for in game time clock
	MenuText* ptrClockDisplay = dynamic_cast<MenuText*>(&m_menuManager.CreateNode(MenuNode::Type::TEXT));
	ptrClockDisplay->m_nodeName = "ui_clock_display";
	// Set parent to root node
	ptrClockDisplay->SetParent(*m_uiRoot);
	ptrClockDisplay->m_text = m_inGameClock.GetMinutesFormatted() + ':' + m_inGameClock.GetSecondsFormatted(2);
	ptrClockDisplay->m_width = 512;
	ptrClockDisplay->m_height = 64;
	ptrClockDisplay->m_x = screenDimScaleX * .01f;
	ptrClockDisplay->m_y = screenDimScaleY * 0.01f;
	ptrClockDisplay->m_font = "bauhaus";
	ptrClockDisplay->m_pitch = 43;
}

void PlayMode::SetupHotBar(D3D & _d3d) {

	MenuImage* ptrItemHotBarBG = dynamic_cast<MenuImage*>(&m_menuManager.CreateNode(MenuNode::Type::IMAGE));

	ptrItemHotBarBG->m_nodeName = "ui_hotbar_bg";
	ptrItemHotBarBG->m_textureName = TxtrNames::ITEM_HOTBAR_BG_MAME;
	ptrItemHotBarBG->SetParent(*m_uiRoot);
	ptrItemHotBarBG->m_frameID = 0;
	ptrItemHotBarBG->m_width = 288;
	ptrItemHotBarBG->m_height = 96;

	// Create Root Menu for player UI
	MenuImage* ptrItemHotBar = dynamic_cast<MenuImage*>(&m_menuManager.CreateNode(MenuNode::Type::IMAGE));

	ptrItemHotBar->m_nodeName = "ui_primary_hotbar";
	ptrItemHotBar->m_textureName = TxtrNames::ITEM_HOTBAR_NAME;
	ptrItemHotBar->SetParent(*ptrItemHotBarBG);
	ptrItemHotBar->m_frameID = 1;
	ptrItemHotBar->m_width = 96;
	ptrItemHotBar->m_height = 96;

	MenuImage* ptrPrimaryItem = dynamic_cast<MenuImage*>(&m_menuManager.CreateNode(MenuNode::Type::IMAGE));

	ptrPrimaryItem->m_nodeName = "ui_primary_item";
	ptrPrimaryItem->m_textureName = TxtrNames::ENERGY_BALL_NAME;
	ptrPrimaryItem->SetParent(*ptrItemHotBar);
	ptrPrimaryItem->m_frameID = 0;
	ptrPrimaryItem->m_width = 64;
	ptrPrimaryItem->m_height = 64;
}

void PlayMode::UpdatePlayerUI(float _deltaTime) {
	UpdateScreenDimScale();
	UpdateItemHotbar();
	//UpdateHealthBar();
	UpdateGameClock(_deltaTime);
}

void PlayMode::UpdateItemHotbar() {
	MenuManager& menuManager = MainGame::Get().GetMenuManager();

	MenuNode& itemHotBarBG = menuManager.FindNode("menu_player_UI", "ui_hotbar_bg");
	MenuNode& itemHotBar = *itemHotBarBG.FindNode("ui_primary_hotbar");

	itemHotBarBG.m_x = (screenDimScaleX * .9f);
	itemHotBarBG.m_y = screenDimScaleY - (itemHotBar.m_height / 2.f) - 66.f;
}

void PlayMode::UpdateGameClock(float _deltaTime) {
	m_fpsUpdateDelayRemaining -= _deltaTime;

	MenuText& clockDisplay = dynamic_cast<MenuText&>(MainGame::Get().GetMenuManager().FindNode("menu_player_UI", "ui_clock_display"));

	clockDisplay.m_text = m_inGameClock.GetMinutesFormatted() + ':' + m_inGameClock.GetSecondsFormatted(2);
	clockDisplay.m_x = screenDimScaleX * .01f;
	clockDisplay.m_y = screenDimScaleY * 0.01f;
}
