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
	m_fpsDisplay = new FPSDisplay();
}

void PlayerUI::Update(float _deltaTime) {
#pragma omp parallel sections
	{
		#pragma omp section
			m_healthBar->Update(_deltaTime);
		#pragma omp section
			m_fpsDisplay->Update(_deltaTime);
	}
	
}


PlayerUI::~PlayerUI() {
	delete m_healthBar;
	delete m_fpsDisplay;

	m_healthBar = nullptr;
	m_fpsDisplay = nullptr;
}

PlayerUI::HealthBar::HealthBar()
	: menuMgr(PlayerUI::Get().m_menuManager) {
	// Initialise nodes before multi-threading to guarantee parallel referencing
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

	// Scale health bar positioning with screen
	healthBarBG->m_x = ((MainGame::Get().GetScreenDimScaleX() - healthBarBG->m_width) * .02f);
	healthBarBG->m_y = MainGame::Get().GetScreenDimScaleY() - 50.f;

	// Set health bar foreground width to match the players remaining health ratio
	healthBarFG->m_width = (430 * (playerHealth.GetHealthRatio()));
	// Update health bar display to show current player health stats
	healthBarDisplay->m_text = "HP: " + std::to_string((int)playerHealth.GetCurHealth()) + '/' + std::to_string((int)playerHealth.GetMaxHealth());
}

PlayerUI::InGameClock::InGameClock()
	: menuMgr(PlayerUI::Get().m_menuManager) {
	clockDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	clockDisplay->m_nodeName = "ui_clock_display";

	// Sizing
	clockDisplay->m_width = 512;
	clockDisplay->m_height = 64;

	clockDisplay->SetParent(*PlayerUI::Get().m_rootUINode);
}

PlayerUI::FPSDisplay::FPSDisplay() 
	: menuMgr(PlayerUI::Get().m_menuManager), 
	fpsUpdateDelay(1.f), delayRemaining(0.f) {
	frameCountDisplay = dynamic_cast<MenuText*>(&menuMgr.CreateNode(MenuNode::Type::TEXT));
	frameCountDisplay->m_nodeName = "ui_fps_display";

	frameCountDisplay->m_width = 512;
	frameCountDisplay->m_height = 64;

	frameCountDisplay->m_text = "FPS: 0";
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

	frameCountDisplay->m_x = MainGame::Get().GetScreenDimScaleX() * .89f;
	frameCountDisplay->m_y = MainGame::Get().GetScreenDimScaleY() * 0.01f;
}
