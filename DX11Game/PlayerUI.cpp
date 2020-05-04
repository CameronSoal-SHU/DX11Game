#include "PlayerUI.h"
#include "MainGame.h"
#include "PlayMode.h"
#include "GameConstants.h"


PlayerUI::PlayerUI()
	: m_menuMgr(MainGame::Get().GetMenuManager()), 
	m_d3d(MainGame::Get().GetD3D()) {
	// Create root node for player UI
	m_ptrUIRoot = &m_menuMgr.AddMenu("menu_player_UI", Settings::GAME_RES);

	m_ptrHealthBar = new HealthBar();
	m_ptrGameClock = new InGameClock();
	m_ptrFPSDisplay = new FPSDisplay();
	m_ptrItemHotBar = new HotBar();
	m_ptrItemHotBar->Init();
}

void PlayerUI::Update(float _deltaTime) {
	// Update each UI element in parallel
	#pragma omp parallel sections
	{
		#pragma omp section
		{ m_ptrHealthBar->Update(_deltaTime); }
		#pragma omp section
		{ m_ptrFPSDisplay->Update(_deltaTime); }
		#pragma omp section
		{ m_ptrGameClock->Update(_deltaTime); }
		#pragma omp section
		{ m_ptrItemHotBar->Update(_deltaTime); }
	}
}

PlayerUI::~PlayerUI() {
	delete m_ptrHealthBar;
	delete m_ptrGameClock;
	delete m_ptrFPSDisplay;
	delete m_ptrItemHotBar;

	m_ptrHealthBar  = nullptr;
	m_ptrGameClock  = nullptr;
	m_ptrFPSDisplay = nullptr;
	m_ptrItemHotBar = nullptr;
}

PlayerUI::HealthBar::HealthBar() {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = PlayerUI::Get().m_menuMgr;

	// Initialise nodes
	ptrHealthBarBG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	ptrHealthBarFG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	ptrHealthBarDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));

	// Node name and texture name referencing
	ptrHealthBarBG->m_nodeName = "ui_health_bar_bg";
	ptrHealthBarBG->m_textureName = TxtrNames::HEALTH_BAR_BG_NAME;
	ptrHealthBarBG->m_frameID = 0;

	// Sizing
	ptrHealthBarBG->m_width = 440;
	ptrHealthBarBG->m_height = 32;

	// Set the parent to the UI root node
	ptrHealthBarBG->SetParent(*PlayerUI::Get().m_ptrUIRoot);

	// Node name and texture name referencing
	ptrHealthBarFG->m_nodeName = "ui_health_bar_fg";
	ptrHealthBarFG->m_textureName = TxtrNames::HEALTH_BAR_FG_NAME;

	// Positioning and sizing (relative to parent node)
	ptrHealthBarFG->m_x = 5;
	ptrHealthBarFG->m_y = 5;
	ptrHealthBarFG->m_width = 430;
	ptrHealthBarFG->m_height = 22;

	ptrHealthBarFG->m_frameID = 0;

	ptrHealthBarFG->SetParent(*ptrHealthBarBG);

	// Node name referencing
	ptrHealthBarDisplay->m_nodeName = "ui_health_bar_txt";

	// Set up font used and debug text
	// The text will be changed each frame, so this text should NOT display
	ptrHealthBarDisplay->m_font = "bauhaus";
	ptrHealthBarDisplay->m_text = "HP: NOT UPDATING";
	ptrHealthBarDisplay->m_pitch = 43;

	// Positioning and sizing (relative to parent node)
	ptrHealthBarDisplay->m_x = 0;
	ptrHealthBarDisplay->m_y = -64;
	ptrHealthBarDisplay->m_width = 512;
	ptrHealthBarDisplay->m_height = 64;

	ptrHealthBarDisplay->SetParent(*ptrHealthBarBG);
}


void PlayerUI::HealthBar::Update(float _deltaTime) {
	// Retrieve the players Health Handler from PlayMode
	HealthHandler& playerHealth = std::dynamic_pointer_cast<Player>(PlayMode::Get().FindObj(typeid(Player), true))->GetHealthHandler();
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Scale health bar positioning with screen
	ptrHealthBarBG->m_x = 25 * screenScaleX;
	ptrHealthBarBG->m_y = (Settings::GAME_RES.y - (ptrHealthBarBG->m_height / screenScaleX) - 25) * screenScaleY;

	// Set health bar foreground width to match the players remaining health ratio
	ptrHealthBarFG->m_width = (430 * (playerHealth.GetHealthRatio()));
	// Update health bar display to show current player health stats
	ptrHealthBarDisplay->m_text = "HP: " + std::to_string((int)playerHealth.GetCurHealth()) + '/' + std::to_string((int)playerHealth.GetMaxHealth());
}

PlayerUI::InGameClock::InGameClock() {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = PlayerUI::Get().m_menuMgr;

	ptrClockDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	ptrClockDisplay->m_nodeName = "ui_clock_display";

	ptrClockDisplay->m_width = 512;
	ptrClockDisplay->m_height = 64;

	// Set up font used and debug message on failed updates
	ptrClockDisplay->m_text = "CLOCK NOT UPDATING";
	ptrClockDisplay->m_font = "bauhaus";
	ptrClockDisplay->m_pitch = 43;

	ptrClockDisplay->SetParent(*PlayerUI::Get().m_ptrUIRoot);
}

void PlayerUI::InGameClock::Update(float _deltaTime) {
	// Tick the clock
	clock.Update(_deltaTime);

	// Update the clock display text to show the clock time
	ptrClockDisplay->m_text = clock.GetMinutesFormatted() + ':' + clock.GetSecondsFormatted(2);

	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Scale clocks position on screen to current screen dimensions
	ptrClockDisplay->m_x = screenScaleX * .01f;
	ptrClockDisplay->m_y = screenScaleY * .01f;
}

PlayerUI::FPSDisplay::FPSDisplay()
	: fpsUpdateDelay(1.f), delayRemaining(0.f) {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = PlayerUI::Get().m_menuMgr;

	ptrFrameRateDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	ptrFrameRateDisplay->m_nodeName = "ui_fps_display";

	ptrFrameRateDisplay->m_width = 200;
	ptrFrameRateDisplay->m_height = 64;

	// Set up font used and debug message on failed updates
	ptrFrameRateDisplay->m_text = "FPS: FAILED UPDATE";
	ptrFrameRateDisplay->m_font = "bauhaus";
	ptrFrameRateDisplay->m_pitch = 43;

	// Set parent to root menu node
	ptrFrameRateDisplay->SetParent(*PlayerUI::Get().m_ptrUIRoot);
}

void PlayerUI::FPSDisplay::Update(float _deltaTime) {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();
	// Decrement delay remaining every frame call by delta time
	delayRemaining -= _deltaTime;

	if (delayRemaining <= 0.f) {
		// Reset update delay
		delayRemaining = fpsUpdateDelay;

		const std::string fpsCount = "FPS: " + std::to_string((int)(1 / _deltaTime));
		ptrFrameRateDisplay->m_text = fpsCount;
	}

	// Sizing and positioning on screen
	ptrFrameRateDisplay->m_x = 1700 * screenScaleX;
	ptrFrameRateDisplay->m_y = .01f * screenScaleY;
}

PlayerUI::HotBar::HotBar() {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = PlayerUI::Get().m_menuMgr;

	ptrHotBarBG = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	//***** HOT BAR BACKGROUND *****//
	ptrHotBarBG->m_nodeName = "ui_hot_bar_base";
	ptrHotBarBG->m_textureName = TxtrNames::ITEM_HOTBAR_BG_MAME;
	ptrHotBarBG->m_frameID = 0;

	ptrHotBarBG->m_width = 192;
	ptrHotBarBG->m_height = 96;

	ptrHotBarBG->SetParent(*PlayerUI::Get().m_ptrUIRoot);

	hotBarSlots.reserve(SLOT_COUNT);
}

void PlayerUI::HotBar::Init() {
	// Assign 3 hot bar slots
	for (int i(0); i < SLOT_COUNT; ++i) {
		hotBarSlots.push_back(HotBarSlot(i));
	}
}

void PlayerUI::HotBar::Update(float _deltaTime) {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Ensure UI element position remains the same on resize
	ptrHotBarBG->m_x = (Settings::GAME_RES.x - (ptrHotBarBG->m_width / screenScaleX) - 25) * screenScaleX;
	ptrHotBarBG->m_y = (Settings::GAME_RES.y - (ptrHotBarBG->m_height / screenScaleX) - 25) * screenScaleY;

	for (int i(0); i < SLOT_COUNT; ++i) {
		MenuText& hotBarSlotCtrl = *hotBarSlots.at(i).ptrHotBarCtrl;
		const bool gamePadEnabled = MainGame::Get().gamePad.IsEnabled(0);

		switch (i)
		{
		case PRIMARY:
			hotBarSlotCtrl.m_text = gamePadEnabled ? "L2" : "M1";
			break;
		case SECONDARY:
			hotBarSlotCtrl.m_text = gamePadEnabled ? "R2" : "M2";
			break;
		default:
			break;
		}

		hotBarSlotCtrl.m_x = 32.f * (1.f / hotBarSlotCtrl.m_text.size());
	}

	UpdateHotBarItems();

	//DBOUT( 1 / screenScaleY << ": " << hotBarBG->m_width);
}

void PlayerUI::HotBar::UpdateHotBarItems() {
	const Player& playerGO = *std::dynamic_pointer_cast<Player>(PlayMode::Get().FindObj(typeid(Player), true));

	// Has a player game object been found?
	if (&playerGO) {
		for (int i(0); i < SLOT_COUNT; ++i) {
			const std::string weapTxtrName = playerGO.GetWeapons().at(i)->GetProjTextureName();
			// If player does not have a weapon equipped, set the hotbar icon to a blank image
			hotBarSlots.at(i).ptrHotBarItemSpr->m_textureName = (weapTxtrName.length() == 0) ? TxtrNames::NULL_TXTR_NAME : weapTxtrName;
		}
	}
}

PlayerUI::HotBar::HotBarSlot::HotBarSlot(int _slotNum) {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = PlayerUI::Get().m_menuMgr;
	const std::string strSlotNum = std::to_string(_slotNum);

	ptrHotBarSlotSpr = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	ptrHotBarItemSpr = dynamic_cast<MenuImage*>(&menuMgr.CreateNode(MenuNode::Type::IMAGE));
	ptrHotBarCtrl	 = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));

	//***** HOT BAR SLOT *****//
	ptrHotBarSlotSpr->m_nodeName = "ui_hot_bar_slot_" + strSlotNum;
	ptrHotBarSlotSpr->m_textureName = TxtrNames::ITEM_HOTBAR_SLOT_NAME;
	ptrHotBarSlotSpr->m_frameID = 1;

	/* Sizing */
	ptrHotBarSlotSpr->m_width = ptrHotBarSlotSpr->m_height = 96;
	ptrHotBarSlotSpr->m_x = (ptrHotBarSlotSpr->m_width * _slotNum);

	ptrHotBarSlotSpr->SetParent(*PlayerUI::Get().m_ptrItemHotBar->ptrHotBarBG);
	//******************************//
	//**** HOT BAR KEY BINDING *****//
	ptrHotBarCtrl->m_nodeName = "ui_hot_bar_ctrl_" + strSlotNum;

	// Sizing and positioning
	ptrHotBarCtrl->m_width = 288;
	ptrHotBarCtrl->m_height = 64;
	// Place the key-prompt above the respective UI icon
	ptrHotBarCtrl->m_y = -.75f * ptrHotBarCtrl->m_height;
	
	// Set up font used and debug message on failed updates
	ptrHotBarCtrl->m_text = "ERROR";
	ptrHotBarCtrl->m_font = "bauhaus";
	ptrHotBarCtrl->m_pitch = 43;

	ptrHotBarCtrl->SetParent(*ptrHotBarSlotSpr);
	//******************************//
	//******** HOT BAR ITEM ********//
	ptrHotBarItemSpr->m_nodeName = "ui_hot_bar_item_" + strSlotNum;
	/// TEMP
	ptrHotBarItemSpr->m_textureName = TxtrNames::NULL_TXTR_NAME;
	ptrHotBarItemSpr->m_frameID = 0;

	/* Sizing */
	ptrHotBarItemSpr->m_width = ptrHotBarItemSpr->m_height = 96;
	ptrHotBarItemSpr->m_x = 0;
	ptrHotBarItemSpr->m_y = 0;

	/* Set parent to corresponding hot bar slot */
	ptrHotBarItemSpr->SetParent(*ptrHotBarSlotSpr);
}

