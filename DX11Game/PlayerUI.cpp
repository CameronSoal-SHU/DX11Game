#include "PlayerUI.h"
#include "MainGame.h"
#include "PlayMode.h"
#include "GameConstants.h"


PlayerUI::PlayerUI(PlayMode& _playMode)
	: m_menuManager(MainGame::Get().GetMenuManager()), 
	m_d3d(MainGame::Get().GetD3D()), m_playMode(_playMode) {
	// Create root node for player UI
	m_rootUINode = &m_menuManager.AddMenu("menu_player_UI", Settings::GAME_RES);

	m_healthBar = new HealthBar();
	m_gameClock = new InGameClock();
	m_fpsDisplay = new FPSDisplay();
	m_itemHotBar = new HotBar();
}

void PlayerUI::Update(float _deltaTime) {
	// Update each UI element in parallel
	#pragma omp parallel sections
	{
		#pragma omp section
		{ m_healthBar->Update(_deltaTime); }
		#pragma omp section
		{ m_fpsDisplay->Update(_deltaTime); }
		#pragma omp section
		{ m_gameClock->Update(_deltaTime); }
		#pragma omp section
		{ m_itemHotBar->Update(_deltaTime); }
	}
}


PlayerUI::~PlayerUI() {
	delete m_healthBar;
	delete m_gameClock;
	delete m_fpsDisplay;
	delete m_itemHotBar;

	m_healthBar = nullptr;
	m_gameClock = nullptr;
	m_fpsDisplay = nullptr;
	m_itemHotBar = nullptr;
}

PlayerUI::HealthBar::HealthBar()
	: menuMgr(PlayerUI::Get().m_menuManager) {
	// Initialise nodes
	healthBarBG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	healthBarFG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	healthBarDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));

	// Node name and texture name referencing
	healthBarBG->m_nodeName = "ui_health_bar_bg";
	healthBarBG->m_textureName = TxtrNames::HEALTH_BAR_BG_NAME;

	// Sizing
	healthBarBG->m_width = 440;
	healthBarBG->m_height = 32;

	healthBarBG->m_frameID = 0;
	// Set the parent to the UI root node
	healthBarBG->SetParent(*PlayerUI::Get().m_rootUINode);

	// Node name and texture name referencing
	healthBarFG->m_nodeName = "ui_health_bar_fg";
	healthBarFG->m_textureName = TxtrNames::HEALTH_BAR_FG_NAME;

	// Positioning and sizing (relative to parent node)
	healthBarFG->m_x = 5;
	healthBarFG->m_y = 5;
	healthBarFG->m_width = 430;
	healthBarFG->m_height = 22;

	healthBarFG->m_frameID = 0;

	healthBarFG->SetParent(*healthBarBG);

	// Node name referencing
	healthBarDisplay->m_nodeName = "ui_health_bar_txt";

	// Set up font used and debug text
	// The text will be changed each frame, so this text should NOT display
	healthBarDisplay->m_font = "bauhaus";
	healthBarDisplay->m_text = "HP: NOT UPDATING";
	healthBarDisplay->m_pitch = 43;

	// Positioning and sizing (relative to parent node)
	healthBarDisplay->m_x = 0;
	healthBarDisplay->m_y = -64;
	healthBarDisplay->m_width = 512;
	healthBarDisplay->m_height = 64;

	healthBarDisplay->SetParent(*healthBarBG);
}


void PlayerUI::HealthBar::Update(float _deltaTime) {
	// Retrieve the players Health Handler from PlayMode
	HealthHandler& playerHealth = std::dynamic_pointer_cast<Player>(PlayerUI::Get().m_playMode.FindObj(typeid(Player), true))->GetHealthHandler();
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Scale health bar positioning with screen
	healthBarBG->m_x = 25 * screenScaleX;
	healthBarBG->m_y = (Settings::GAME_RES.y - (healthBarBG->m_height / screenScaleX) - 25) * screenScaleY;

	// Set health bar foreground width to match the players remaining health ratio
	healthBarFG->m_width = (430 * (playerHealth.GetHealthRatio()));
	// Update health bar display to show current player health stats
	healthBarDisplay->m_text = "HP: " + std::to_string((int)playerHealth.GetCurHealth()) + '/' + std::to_string((int)playerHealth.GetMaxHealth());
}

PlayerUI::InGameClock::InGameClock()
	: menuMgr(PlayerUI::Get().m_menuManager) {
	clockDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	clockDisplay->m_nodeName = "ui_clock_display";

	clockDisplay->m_width = 512;
	clockDisplay->m_height = 64;

	// Set up font used and debug message on failed updates
	clockDisplay->m_text = "CLOCK NOT UPDATING";
	clockDisplay->m_font = "bauhaus";
	clockDisplay->m_pitch = 43;

	clockDisplay->SetParent(*PlayerUI::Get().m_rootUINode);
}

void PlayerUI::InGameClock::Update(float _deltaTime) {
	// Tick the clock
	clock.Update(_deltaTime);

	// Update the clock display text to show the clock time
	clockDisplay->m_text = clock.GetMinutesFormatted() + ':' + clock.GetSecondsFormatted(2);

	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Scale clocks position and size on screen to current screen dimensions
	/*clockDisplay->m_width = 512 * screenScaleX;
	clockDisplay->m_height = 64 * screenScaleY;*/
	clockDisplay->m_x = screenScaleX * .01f;
	clockDisplay->m_y = screenScaleY * .01f;
}

PlayerUI::FPSDisplay::FPSDisplay() 
	: menuMgr(PlayerUI::Get().m_menuManager), 
	fpsUpdateDelay(1.f), delayRemaining(0.f) {
	frameCountDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	frameCountDisplay->m_nodeName = "ui_fps_display";

	frameCountDisplay->m_width = 200;
	frameCountDisplay->m_height = 64;

	// Set up font used and debug message on failed updates
	frameCountDisplay->m_text = "FPS: FAILED UPDATE";
	frameCountDisplay->m_font = "bauhaus";
	frameCountDisplay->m_pitch = 43;

	// Set parent to root menu node
	frameCountDisplay->SetParent(*PlayerUI::Get().m_rootUINode);
}

void PlayerUI::FPSDisplay::Update(float _deltaTime) {
	// Decrement delay remaining every frame call by delta time
	delayRemaining -= _deltaTime;

	if (delayRemaining <= 0.f) {
		// Reset update delay
		delayRemaining = fpsUpdateDelay;

		const std::string fpsCount = "FPS: " + std::to_string((int)(1 / _deltaTime));
		frameCountDisplay->m_text = fpsCount;
	}

	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Sizing and positioning on screen
	/*frameCountDisplay->m_width = 200 * screenScaleX;
	frameCountDisplay->m_height = 64 * screenScaleY;*/
	frameCountDisplay->m_x = 1700 * screenScaleX;
	frameCountDisplay->m_y = .01f * screenScaleY;
}

PlayerUI::HotBar::HotBar() 
	: menuMgr(PlayerUI::Get().m_menuManager) {
	hotBarBG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	primaryItem = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	secondaryItem = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	equipmentItem = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));

	//***** HOT BAR BACKGROUND *****//
	hotBarBG->m_nodeName = "ui_hot_bar_base";
	hotBarBG->m_textureName = TxtrNames::ITEM_HOTBAR_BG_MAME;
	hotBarBG->m_frameID = 0;

	hotBarBG->m_width = 288;
	hotBarBG->m_height = 96;
	
	hotBarBG->SetParent(*PlayerUI::Get().m_rootUINode);
	//******************************//
	//**** HOT BAR PRIMARY ITEM ****//
	primaryItem->m_nodeName = "ui_primary_item";
	primaryItem->m_textureName = TxtrNames::ITEM_HOTBAR_SLOT_NAME;
	primaryItem->m_frameID = 1;

	primaryItem->m_width = primaryItem->m_height = 96;

	primaryItem->SetParent(*hotBarBG);
	//******************************//
}

void PlayerUI::HotBar::Update(float _deltaTime) {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	/*hotBarBG->m_width = 288 * screenScaleX;
	hotBarBG->m_height = 96 * screenScaleY;*/

	// Ensure UI element position remains the same on resize
	hotBarBG->m_x = (Settings::GAME_RES.x - (hotBarBG->m_width / screenScaleX) - 25) * screenScaleX;
	hotBarBG->m_y = (Settings::GAME_RES.y - (hotBarBG->m_height / screenScaleX) - 25) * screenScaleY;

	DBOUT( 1 / screenScaleY << ": " << hotBarBG->m_width);
}
