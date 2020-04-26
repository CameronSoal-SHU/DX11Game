#include "ItemShopMode.h"
#include "GameConstants.h"
#include "MainGame.h"

const std::string ItemShopMode::MODE_NAME = "SHOP";

ItemShopMode::ItemShopMode(D3D& _d3d)
	:m_menuManager(MainGame::Get().GetMenuManager()), m_d3d(_d3d), 
	m_background(_d3d) {
	Start();
}


ItemShopMode::~ItemShopMode()
{
}

void ItemShopMode::Enter() 
{}

bool ItemShopMode::Exit()
{
	MainGame::Get().GetMenuManager().HideMenu();
	return true;
}

void ItemShopMode::Start() {
	SetUpBackground();
}

void ItemShopMode::Update(float _deltaTime) {
	GetPlayerInput();
	m_background.Update(_deltaTime);
}

void ItemShopMode::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	m_background.Draw(_sprBatch);
}

void ItemShopMode::GetPlayerInput() {
	if (MainGame::mouseKeyboardInput.IsPressed(VK_TAB)) {
		if (m_canSwitchMode) {
			m_canSwitchMode = false;
			MainGame::Get().GetModeManager().ChangeMode(PlayMode::MODE_NAME);
		}
	}
	else {
		m_canSwitchMode = true;
	}
}

void ItemShopMode::SetUpBackground() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	m_background.SetTexture(TxtrNames::SHOP_BG_NAME, *txtrCache.GetData(TxtrNames::SHOP_BG_NAME).ptrTexture);
	m_background.SetScale({ 0.945f, 0.945f });
}
