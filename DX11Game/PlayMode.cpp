#include "PlayMode.h"
#include "GameConstants.h"
#include "MainGame.h"
#include "GameSettings.h"

#include <algorithm>

const std::string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode()
	: m_menuManager(MainGame::Get().GetMenuManager()), 
	screenDimScaleX((Settings::GAME_RES.x * Settings::GAME_RES.x) / WindowUtil::Get().GetClientWidth()),
	screenDimScaleY((Settings::GAME_RES.y * Settings::GAME_RES.y) / WindowUtil::Get().GetClientHeight()) {
	m_gameObjs.reserve(1000);

	Player* player = new Player(150.f, 150.f);
	player->SetParentMode(*this);
	player->SetActive(true);
	AddObj(player);
	Start();
}

PlayMode::~PlayMode() {
	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		delete m_gameObjs.at(i);
	}

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
	m_inGameClock.Update(_deltaTime);
	UpdatePlayerUI();

	ScrollBackground(_deltaTime);

	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Update(_deltaTime);
	}
}

void PlayMode::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	// Draw each background element back-most first
	for (Sprite& bgLayer : m_parallaxBGLayers) {
		bgLayer.Draw(_sprBatch);
	}

	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Render(_deltaTime, _sprBatch);
	}
}

void PlayMode::AddObj(GameObject * _ptrObj) {
	assert(_ptrObj);

	m_gameObjs.push_back(_ptrObj);
}

void PlayMode::RemoveObj(GameObject * _ptrObj) {
	size_t objSize = m_gameObjs.size();
	assert(objSize > 0);					// Make sure there's object that can be deleted

	m_gameObjs.erase(std::remove(m_gameObjs.begin(), m_gameObjs.end(), _ptrObj));
	assert(objSize != m_gameObjs.size());	// Check if the object has been deleted

	delete _ptrObj;
}

GameObject* PlayMode::FindObj(const std::type_info & _type, bool _isActive) {
	size_t i(0);

	// Search for object in list
	while (i < m_gameObjs.size() && 
		(typeid(*m_gameObjs.at(i)) != _type || m_gameObjs.at(i)->IsActive() != _isActive)) {
		++i;
	}

	// Return the object if anything was found
	return (i < m_gameObjs.size() ? m_gameObjs.at(i) : nullptr);
}

void PlayMode::UpdateScreenDimScale() {
	screenDimScaleX = (Settings::GAME_RES.x * Settings::GAME_RES.x) / WindowUtil::Get().GetClientWidth();
	screenDimScaleY = (Settings::GAME_RES.y * Settings::GAME_RES.y) / WindowUtil::Get().GetClientHeight();
}

void PlayMode::SetupBackground() {
	D3D& d3d = MainGame::Get().GetD3D();

	// Allocate memory for each background sprite
	for (int i(0); i < 2; ++i) {
		m_parallaxBGLayers.push_back(Sprite(d3d));
	}

	int i(0);
	// Go through each sprite in the background parallax vector
	// And set the texture (back-most first)
	for (Sprite& bgLayer : m_parallaxBGLayers) {
		const std::string filePath = "backgroundLayers/space_background_" + std::to_string(i) + ".dds";
		const std::string textureName = "background_" + std::to_string(i);

		bgLayer.SetTexture(textureName, *d3d.GetTextureCache().LoadTexture(&d3d.GetDevice(),
			filePath, textureName, APPEND_PATH));

		bgLayer.SetScale({ 3.5f, 3.5f });

		++i;
	}
}

void PlayMode::ScrollBackground(float _deltaTime) {
	// Scroll each background at different rates(closer = faster) to add a parallax effect
	for (unsigned i(0); i < m_parallaxBGLayers.size(); ++i) {
		Sprite& bgElement = m_parallaxBGLayers.at(i);
		const float paraScrollRate = _deltaTime * m_scrollSpeed * (i + 1);

		RECTF scrollRect{ 0, paraScrollRate, 0, paraScrollRate };

		bgElement.SetTextureRect(bgElement.GetTextureRect() - scrollRect);
	}
}

void PlayMode::SetupPlayerUI() {
	D3D& d3d = MainGame::Get().GetD3D();
	SetupHealthbar(d3d);
	SetupGameClock();
}

void PlayMode::SetupGameClock() {
	// Create text for in game time clock
	MenuText* ptrClockDisplay = dynamic_cast<MenuText*>(&m_menuManager.CreateNode(MenuNode::Type::TEXT));
	ptrClockDisplay->m_nodeName = "ui_clock_display";
	// Set parent to root node
	ptrClockDisplay->SetParent(m_menuManager.FindNode("menu_player_UI", "menu_player_UI"));
	ptrClockDisplay->m_text = m_inGameClock.GetMinutesFormatted() + ':' + m_inGameClock.GetSecondsFormatted(2);
	ptrClockDisplay->m_width = 512;
	ptrClockDisplay->m_height = 64;
	ptrClockDisplay->m_x = screenDimScaleX * .01f;
	ptrClockDisplay->m_y = screenDimScaleY * 0.01f;
	ptrClockDisplay->m_nodeID = 1;
	ptrClockDisplay->m_font = "algerian";
	ptrClockDisplay->m_pitch = 12;
}

void PlayMode::SetupHealthbar(D3D& _d3d) {
	std::vector<TextureCache::Data::Sprite> frames{
		{{0,0}, STRETCHED, {0,0,32,32}}
	};

	// Create Root Menu for player UI
	MenuNode& playerUIRoot = m_menuManager.AddMenu("menu_player_UI", Settings::GAME_RES);

	_d3d.GetTextureCache().LoadTexture(&_d3d.GetDevice(), "UI/health_bar_bg.dds", "ui_healthbar_bg", APPEND_PATH, &frames);
	_d3d.GetTextureCache().LoadTexture(&_d3d.GetDevice(), "UI/health_bar_fg.dds", "ui_healthbar_fg", APPEND_PATH, &frames);

	// Health bar background
	MenuImage* ptrHPBarBG = dynamic_cast<MenuImage*>(&m_menuManager.CreateNode(MenuNode::Type::IMAGE));

	ptrHPBarBG->m_nodeName = "ui_healthbar_background";
	ptrHPBarBG->m_textureName = "ui_healthbar_bg";
	ptrHPBarBG->m_width = 540;
	ptrHPBarBG->m_height = 32;
	ptrHPBarBG->m_x = ((Settings::GAME_RES.x - ptrHPBarBG->m_width) * .5f);
	ptrHPBarBG->m_y = Settings::GAME_RES.y * .5f;
	ptrHPBarBG->m_nodeID = 0;
	ptrHPBarBG->m_frameID = 0;
	ptrHPBarBG->SetParent(playerUIRoot);

	// Health bar foreground
	MenuImage* ptrHPBarFG = dynamic_cast<MenuImage*>(&m_menuManager.CreateNode(MenuNode::Type::IMAGE));

	ptrHPBarFG->m_nodeName = "ui_healthbar_foreground";
	ptrHPBarFG->m_textureName = "ui_healthbar_fg";
	ptrHPBarFG->m_x = 5;
	ptrHPBarFG->m_y = 5;
	ptrHPBarFG->m_width = 530;
	ptrHPBarFG->m_height = 22;
	ptrHPBarFG->m_nodeID = 0;
	ptrHPBarFG->m_frameID = 0;
	ptrHPBarFG->SetParent(*ptrHPBarBG);

	// Health bar health count
	MenuText* ptrHPDisplay = dynamic_cast<MenuText*>(&m_menuManager.CreateNode(MenuNode::Type::TEXT));

	ptrHPDisplay->m_nodeName = "ui_healthbar_text";
	Player* ptrPlayer = dynamic_cast<Player*>(FindObj(typeid(Player), true));
	ptrHPDisplay->m_text = "HP: " + std::to_string((int)ptrPlayer->GetHealthHandler().GetCurHealth()) +
		'/' + std::to_string((int)ptrPlayer->GetHealthHandler().GetMaxHealth());
	ptrHPDisplay->m_x = 0;
	ptrHPDisplay->m_y = -64;
	ptrHPDisplay->m_width = 512;
	ptrHPDisplay->m_height = 64;
	ptrHPDisplay->m_font = "algerian";
	ptrHPDisplay->m_pitch = 12;
	ptrHPDisplay->m_nodeID = 1;
	ptrHPDisplay->SetParent(*ptrHPBarBG);
}

void PlayMode::UpdatePlayerUI() {
	UpdateScreenDimScale();
	UpdateHealthBar();
	UpdateGameClock();
}

void PlayMode::UpdateHealthBar() {
	Player* ptrPlayer = dynamic_cast<Player*>(m_gameObjs.at(0));

	// Health bar element references
	MenuNode& hpBarBGNode = MainGame::Get().GetMenuManager().FindNode("menu_player_UI", "ui_healthbar_background");
	MenuNode& hpBarFGNode = *hpBarBGNode.FindNode("ui_healthbar_foreground");
	MenuText& hpBarDisplay = dynamic_cast<MenuText&>(*hpBarBGNode.FindNode("ui_healthbar_text"));

	hpBarBGNode.m_x = ((screenDimScaleX - hpBarBGNode.m_width) * .02f);
	hpBarBGNode.m_y = screenDimScaleY - 50.f;
	// Length of foreground HP bar should scale with player health
	hpBarFGNode.m_width = (530 * (ptrPlayer->GetHealthHandler().GetHealthRatio()));
	hpBarDisplay.m_text = "HP: " + std::to_string((int)ptrPlayer->GetHealthHandler().GetCurHealth()) +
		'/' + std::to_string((int)ptrPlayer->GetHealthHandler().GetMaxHealth());
}

void PlayMode::UpdateGameClock() {
	MenuText& clockDisplay = dynamic_cast<MenuText&>(MainGame::Get().GetMenuManager().FindNode("menu_player_UI", "ui_clock_display"));
	clockDisplay.m_text = m_inGameClock.GetMinutesFormatted() + ':' + m_inGameClock.GetSecondsFormatted(2);
	clockDisplay.m_x = screenDimScaleX * .01f;
	clockDisplay.m_y = screenDimScaleY * 0.01f;
}
