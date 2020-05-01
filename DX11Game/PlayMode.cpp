#include "PlayMode.h"
#include "GameConstants.h"
#include "MainGame.h"
#include "GameSettings.h"
#include "ItemShopMode.h"
#include <algorithm>

const std::string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode()
	: m_menuManager(MainGame::Get().GetMenuManager()), m_playerUI(*this) {
	m_gameObjs.reserve(1000);

	// Set up the player object
	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->SetParentMode(*this);
	player->SetActive(true);
	player->SetupWeapons();
	AddObj(player);

	std::shared_ptr<Enemy> enemyTest = std::make_shared<Enemy>();
	enemyTest->SetParentMode(*this);
	enemyTest->SetActive(true);
	enemyTest->SetLookAtPlayer(true);
	AddObj(enemyTest);

	// Set up item shop menu
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
}

void PlayMode::Update(float _deltaTime) {
	GetPlayerInput(_deltaTime);
	ScrollBackground(_deltaTime);

	// Call all game objects update methods
	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Update(_deltaTime);
	}

	FindObj(typeid(Enemy), true)->GetSprite().SetVelocity(FindObj(typeid(Player), true)->GetSprite().GetVelocity());

	// Update player UI last so it's always ontop
	m_playerUI.Update(_deltaTime);
}

void PlayMode::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	// Draw each background element back-most first
	for (size_t i(0); i < m_parallaxBGLayers.size(); ++i) {
		m_parallaxBGLayers.at(i).Draw(_sprBatch);
	}

	// Draw each game object on screen
	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Render(_deltaTime, _sprBatch);
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
