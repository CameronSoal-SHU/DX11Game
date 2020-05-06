#include "ItemShopMode.h"
#include "GameConstants.h"
#include "MainGame.h"
#include <sstream>
#include <iomanip>


const std::string ItemShopMode::MODE_NAME = "SHOP";

ItemShopMode::ItemShopMode(D3D& _d3d)
	:m_menuMgr(MainGame::Get().GetMenuManager()), m_d3d(_d3d), 
	m_backgroundSpr(_d3d) {
	m_ptrUIRoot = &m_menuMgr.AddMenu("menu_item_shop_UI", Settings::GAME_RES);

	Start();
}

ItemShopMode::~ItemShopMode() {
	for (int i(0); i < 2; ++i) {
		delete m_itemCatagories[i];
		m_itemCatagories[i] = nullptr;
	}

	delete m_ptrWeapPreview;
	m_ptrWeapPreview = nullptr;
}

void ItemShopMode::Enter() {
	MainGame::Get().GetMenuManager().ShowMenu("menu_item_shop_UI");
	// Reset the item preview every time the menu is opened
	m_ptrWeapPreview->ResetPreview();
}

bool ItemShopMode::Exit()
{
	MainGame::Get().GetMenuManager().HideMenu();
	return true;
}

void ItemShopMode::Start() {
	SetupBackground();
	SetupItemShopUI();
}

void ItemShopMode::Update(float _deltaTime) {
	GetPlayerInput();
	m_backgroundSpr.Update(_deltaTime);

	for (int i(0); i < 2; ++i) {
		m_itemCatagories[i]->Update((int)i);
	}

	m_ptrWeapPreview->Update();

	// Check for any items already equipped by player
	CheckPlayerWeapons();
}

void ItemShopMode::Render(float _deltaTime, DirectX::SpriteBatch & _sprBatch) {
	m_backgroundSpr.Draw(_sprBatch);
}

void ItemShopMode::GetPlayerInput() {
	if (MainGame::mouseKeyboardInput.IsPressed(VK_TAB)) {
		if (m_canSwitchMode) {
			m_canSwitchMode = false;
			MainGame::Get().GetModeManager().ChangeMode(PlayMode::Get().GetModeName());
		}
	}
	else {
		m_canSwitchMode = true;
	}
}

void ItemShopMode::DisplayPreviewUIEvent(MenuNode& _node, MenuNode::Event _e) {
	// DBOUT(_node.m_nodeName << ", BUTTON CLICKED!");
	Weapon* weapon = nullptr;
	int i(0);

	// Find the weapon linked to the button node
	while (i < 2 && !weapon) {
		weapon = m_itemCatagories[i]->GetLinkedWeapon(&_node);
		++i;
	}
	
	m_ptrWeapPreview->DisplayStats(weapon);
}

void ItemShopMode::EquipWeaponUIEvent(MenuNode& _node, MenuNode::Event _e) {
	// Grab the currently displayed weapon
	Weapon& displayWeap = *ItemShopMode::Get().m_ptrWeapPreview->GetDisplayedWeap();

	// Create a new instance of the weapon
	Weapon* weaponCopy = new Weapon(displayWeap);
	weaponCopy->SetModeOwner(PlayMode::Get());
	weaponCopy->SetProjTextureName(displayWeap.GetProjTextureName());

	// Set the players weapon at the appropriate slot
	m_ptrPlayer->SetWeapon(displayWeap.GetWeapType(), weaponCopy);

	// Reset the preview on weapon purchase
	m_ptrWeapPreview->ResetPreview();
}

void ItemShopMode::SetupBackground() {
	TextureCache& txtrCache = m_d3d.GetTextureCache();

	m_backgroundSpr.SetTexture(TxtrNames::SHOP_BG_NAME, *txtrCache.GetData(TxtrNames::SHOP_BG_NAME).ptrTexture);
	m_backgroundSpr.SetScale({ 0.945f, 0.945f });
}

void ItemShopMode::SetupItemShopUI() {
	for (int i(0); i < 2; ++i) {
		m_itemCatagories[i] = new ItemCatagory(i);
	}

	m_ptrWeapPreview = new WeapPreview();
}

void ItemShopMode::CheckPlayerWeapons() {
	// Retrieve the players currently equipped weapons 
	std::vector<Weapon*> playerWeapons = m_ptrPlayer->GetWeapons();

	if (!playerWeapons.empty()) {
		#pragma omp parallel for
		for (int i(0); i < CATAGORY_COUNT; ++i) {
			ItemCatagory& catagory = *m_itemCatagories[i];
			#pragma omp parallel for
			for (int j(0); j < (int)catagory.GetBuyables().size(); ++j) {
				ItemCatagory::Buyable& buyable = *catagory.GetBuyables().at(j);

				// Does the player own a weapon by the same name?
				if (playerWeapons.at(i)->GetWeapName() == buyable.linkedWeapon->GetWeapName()) {
					buyable.itemButton->SetEnabled(false);
				}
				else {
					buyable.itemButton->SetEnabled(true);
				}
			}
		}
	}
}

ItemShopMode::ItemCatagory::ItemCatagory(int _containerID) {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = ItemShopMode::Get().m_menuMgr;
	SetupContainer(menuMgr, _containerID);
	SetupHeader(menuMgr, _containerID);
	SetupItems(_containerID);
}

ItemShopMode::ItemCatagory::~ItemCatagory() {
	// Delete all option pointers
	for (size_t i(0); i < m_buyables.size(); ++i) {
		delete m_buyables.at(i);
		m_buyables.at(i) = nullptr;
	}

	m_buyables.clear();
}

void ItemShopMode::ItemCatagory::Update(int _containerID) {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	m_ptrItemContainer->m_x = 100 * screenScaleX;
	m_ptrItemContainer->m_y = (100 + (460 * _containerID)) * screenScaleY;
	m_ptrItemContainer->m_width = 1080 * screenScaleX;
	m_ptrItemContainer->m_height = 360 * screenScaleY;

	for (size_t i(0); i < m_buyables.size(); ++i) {
		m_buyables.at(i)->Update();
	}
}

void ItemShopMode::ItemCatagory::SetupContainer(MenuManager& _menuMgr, int _containerID) {
	m_ptrItemContainer = dynamic_cast<MenuImage*>(&_menuMgr.CreateNode(MenuNode::Type::IMAGE));

	m_ptrItemContainer->m_nodeName = "ui_item_container_" + std::to_string(_containerID);
	m_ptrItemContainer->m_textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrItemContainer->m_frameID = 0;

	m_ptrItemContainer->SetParent(*ItemShopMode::Get().m_ptrUIRoot);
}

void ItemShopMode::ItemCatagory::SetupHeader(MenuManager& _menuMgr, int _containerID) {
	m_ptrItemCtrHeader = dynamic_cast<MenuText*>(&_menuMgr.CreateNode(MenuNode::Type::TEXT));

	m_ptrItemCtrHeader->m_nodeName = "ui_item_catagory_" + std::to_string(_containerID);
	m_ptrItemCtrHeader->m_text = (_containerID == 0) ? "PRIMARY WEAPONS" : "SECONDARY WEAPONS";
	m_ptrItemCtrHeader->m_font = "bauhaus";
	m_ptrItemCtrHeader->m_pitch = 43;

	m_ptrItemCtrHeader->m_width = 512;
	m_ptrItemCtrHeader->m_height = 256;
	m_ptrItemCtrHeader->m_y = (-65.f + (8 * _containerID));

	m_ptrItemCtrHeader->SetParent(*m_ptrItemContainer);
}

void ItemShopMode::ItemCatagory::SetupItems(int _containerID) {
	m_buyables.push_back(new Buyable(*m_ptrItemContainer, _containerID, new EnergyBallWeapon()));
}

Weapon* ItemShopMode::ItemCatagory::GetLinkedWeapon(MenuNode* _node)
{
	assert(!m_buyables.empty());

	size_t i(0);
	Weapon* foundWeapon = nullptr;

	do {
		if (m_buyables.at(i)->itemButton->m_nodeID == _node->m_nodeID) {
			foundWeapon = m_buyables.at(i)->linkedWeapon;
		}

		++i;
	} while (!foundWeapon && i < m_buyables.size());

	return foundWeapon;
}

ItemShopMode::ItemCatagory::Buyable::Buyable(MenuImage& _parent, int _containerID, Weapon* _weap) {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = ItemShopMode::Get().m_menuMgr;

	linkedWeapon = _weap;
	linkedWeapon->SetWeapType(_containerID);	// Container ID will match up with weapon slot assignment (PRIMARY = 0, SECONDARY = 1)
	
	itemButton = dynamic_cast<MenuButton*>(&menuMgr.CreateNode(MenuNode::Type::BUTTON));
	// Link an event handler to the button
	MenuManager::Handler hEvent{ [this](MenuNode& _node, MenuNode::Event _e) { ItemShopMode::Get().DisplayPreviewUIEvent(_node, _e); } };

	// Set unique node name
	itemButton->m_nodeName = "ui_container_" + std::to_string(_containerID) + "_item_" + linkedWeapon->GetWeapName();
	itemButton->SetParent(_parent);
	menuMgr.AddEventHandler("menu_item_shop_UI", itemButton->m_nodeName, MenuNode::Event::CLICK, hEvent);

	// No mouse interaction
	itemButton->buttons[MenuButton::NORMAL].spriteID = 0;
	itemButton->buttons[MenuButton::NORMAL].textureName = linkedWeapon->GetProjTextureName();
	itemButton->buttons[MenuButton::NORMAL].colour = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	// Mouse is hovering over an item
	itemButton->buttons[MenuButton::HOVER].spriteID = 0;
	itemButton->buttons[MenuButton::HOVER].textureName = linkedWeapon->GetProjTextureName();
	itemButton->buttons[MenuButton::HOVER].colour = DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1);

	// Mouse is clicking on an item
	itemButton->buttons[MenuButton::PRESSED].spriteID = 0;
	itemButton->buttons[MenuButton::PRESSED].textureName = linkedWeapon->GetProjTextureName();
	itemButton->buttons[MenuButton::PRESSED].colour = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1);

	// Button cannot be interacted with
	itemButton->buttons[MenuButton::DISABLED].spriteID = 0;
	itemButton->buttons[MenuButton::DISABLED].textureName = linkedWeapon->GetProjTextureName();
	itemButton->buttons[MenuButton::DISABLED].colour = DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1);

	itemButton->m_x = 0;
	itemButton->m_y = 0;
}

ItemShopMode::ItemCatagory::Buyable::~Buyable() {
	// Free up the memory holding the linked weapon ptr
	delete linkedWeapon;
	linkedWeapon = nullptr;
}

void ItemShopMode::ItemCatagory::Buyable::Update() {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Make sure the item display image maintains it's size with screen size
	itemButton->m_width = 360 *  screenScaleX;
	itemButton->m_height = 360 * screenScaleY;
}

ItemShopMode::WeapPreview::WeapPreview() {
	// MenuManager reference from PlayerUI
	MenuManager& menuMgr = ItemShopMode::Get().m_menuMgr;

	SetupPreviewContainer(menuMgr);
	SetupPreviewHeader(menuMgr);
	SetupPreviewStats(menuMgr);
	SetupEquipButton(menuMgr);
}

void ItemShopMode::WeapPreview::Update() {
	const float screenScaleX = MainGame::Get().GetScreenDimScaleX();
	const float screenScaleY = MainGame::Get().GetScreenDimScaleY();

	// Scale preview container to screen size
	m_ptrPreviewContainer->m_x = 1280 * screenScaleX;
	m_ptrPreviewContainer->m_y = 100 * screenScaleY;
	m_ptrPreviewContainer->m_width = 540 * screenScaleX;
	m_ptrPreviewContainer->m_height = 820 * screenScaleY;

	// Scale header for preview container
	m_ptrPreviewHeader->m_height = 78.f * screenScaleY;
	m_ptrPreviewHeader->m_y = -55.f * (screenScaleY);

	// Scale each weapon stat preview to screen size
	#pragma omp parallel for
	for (int i(0); i < STAT_COUNT; ++i) {
		MenuText*& weapStat = m_ptrPreviewStats[i];
		
		weapStat->m_height = 64.f * screenScaleY;
		weapStat->m_x = 15.f * screenScaleX;
		weapStat->m_y = (50.f * (i + 1)) * screenScaleY;
	}

	m_ptrPreviewStats[WEAP_DESC]->m_height = 128.f * screenScaleY;

	// Hide the equip button if no weapon is selected
	if (m_ptrDisplayWeapon) {
		m_ptrEquipButton->m_width = 240 * screenScaleX;
		m_ptrEquipButton->m_height = 120 * screenScaleY;
	}
	else {
		m_ptrEquipButton->m_width = 0;
		m_ptrEquipButton->m_height = 0;
	}

	m_ptrEquipButton->m_x = (m_ptrPreviewContainer->m_width - m_ptrEquipButton->m_width) / 2.f;
	m_ptrEquipButton->m_y = (m_ptrPreviewContainer->m_height - (200.f * screenScaleY));

}

void ItemShopMode::WeapPreview::DisplayStats(Weapon* _weap) {
	m_ptrDisplayWeapon = _weap;	// Link weapon
	const Weapon::WeaponModifiers& weapMods = _weap->GetWeapMods();

	// Truncate floating-point values to 2 d.p
	std::ostringstream dmg, rof, lt, ps;
	dmg << std::setprecision(2) << weapMods.damageMult;		// Damage
	rof << std::setprecision(2) << weapMods.fireRateMult;	// Fire rate
	lt << std::setprecision(2) << weapMods.lifeTimeMult;	// Life-time / Range
	ps << std::setprecision(2) << weapMods.projSpeedMult;	// Projectile Speed

	m_ptrPreviewStats[WEAP_NAME]->m_text = "NAME: " + _weap->GetWeapName();
	m_ptrPreviewStats[WEAP_TYPE]->m_text = "TYPE: " + _weap->GetWeapTypeToString();
	m_ptrPreviewStats[WEAP_DMG]->m_text = "DAMAGE: " + dmg.str();
	m_ptrPreviewStats[WEAP_ROF]->m_text = "FIRE RATE: " + rof.str();
	m_ptrPreviewStats[WEAP_LIFE_TIME]->m_text = "RANGE: " + lt.str();
	m_ptrPreviewStats[WEAP_PROJ_SPEED]->m_text = "PROJ. SPEED: " + ps.str();
	m_ptrPreviewStats[WEAP_DESC]->m_text = _weap->GetWeapDesc();
}

void ItemShopMode::WeapPreview::ResetPreview() {
	#pragma omp parallel for
	for (int i(0); i < STAT_COUNT; ++i) {
		MenuText*& statDisplay = m_ptrPreviewStats[i];
		statDisplay->m_text = "";
	}

	m_ptrDisplayWeapon = nullptr;
}

void ItemShopMode::WeapPreview::SetupPreviewContainer(MenuManager& _menuMgr) {
	m_ptrPreviewContainer = dynamic_cast<MenuImage*>(&_menuMgr.CreateNode(MenuNode::Type::IMAGE));

	m_ptrPreviewContainer->m_nodeName = "ui_item_stats_container";
	m_ptrPreviewContainer->SetParent(*ItemShopMode::Get().m_ptrUIRoot);

	m_ptrPreviewContainer->m_textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrPreviewContainer->m_frameID = 0;
}

void ItemShopMode::WeapPreview::SetupPreviewHeader(MenuManager& _menuMgr) {
	m_ptrPreviewHeader = dynamic_cast<MenuText*>(&_menuMgr.CreateNode(MenuNode::Type::TEXT));

	m_ptrPreviewHeader->m_nodeName = "ui_item_stats_header";
	m_ptrPreviewHeader->SetParent(*m_ptrPreviewContainer);

	m_ptrPreviewHeader->m_text = "WEAPON STATS";
	m_ptrPreviewHeader->m_font = "bauhaus";
	m_ptrPreviewHeader->m_pitch = 43;

	m_ptrPreviewHeader->m_width = 512;
	m_ptrPreviewHeader->m_height = 256;
	m_ptrPreviewHeader->m_y = -80.f;
}

void ItemShopMode::WeapPreview::SetupPreviewStats(MenuManager& _menuMgr) {
	#pragma omp parallel for
	for (int i(0); i < STAT_COUNT; ++i) {
		MenuText*& statDisplay = m_ptrPreviewStats[i];

		statDisplay = dynamic_cast<MenuText*>(&_menuMgr.CreateNode(MenuNode::Type::TEXT));
		statDisplay->m_nodeName = "ui_stat_display_";
		statDisplay->SetParent(*m_ptrPreviewContainer);

		switch (static_cast<StatIdx>(i))
		{
		case WEAP_NAME:
			statDisplay->m_nodeName += "name";
			break;
		case WEAP_TYPE:
			statDisplay->m_nodeName += "type";
			break;
		case WEAP_DMG:
			statDisplay->m_nodeName += "damage";
			break;
		case WEAP_ROF:
			statDisplay->m_nodeName += "fire_Rate";
			break;
		case WEAP_LIFE_TIME:
			statDisplay->m_nodeName += "life_time";
			break;
		case WEAP_PROJ_SPEED:
			statDisplay->m_nodeName += "proj_speed";
			break;
		case WEAP_DESC:
			statDisplay->m_nodeName += "desc";
			break;
		default:
			assert(false);
			break;
		}
		statDisplay->m_font = "bauhaus";
		statDisplay->m_pitch = 43;

		statDisplay->m_width = 960;
	}
}

void ItemShopMode::WeapPreview::SetupEquipButton(MenuManager& _menuMgr) {
	m_ptrEquipButton = dynamic_cast<MenuButton*>(&_menuMgr.CreateNode(MenuNode::Type::BUTTON));
	m_ptrEquipText = dynamic_cast<MenuText*>(&_menuMgr.CreateNode(MenuNode::Type::TEXT));

	m_ptrEquipButton->m_nodeName = "ui_weapon_equip";
	m_ptrEquipButton->SetParent(*m_ptrPreviewContainer);

	// No mouse interaction
	m_ptrEquipButton->buttons[MenuButton::NORMAL].spriteID = 0;
	m_ptrEquipButton->buttons[MenuButton::NORMAL].textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrEquipButton->buttons[MenuButton::NORMAL].colour = DirectX::SimpleMath::Vector4(1, 1, 1, 1);

	// Mouse is hovering over the button
	m_ptrEquipButton->buttons[MenuButton::HOVER].spriteID = 0;
	m_ptrEquipButton->buttons[MenuButton::HOVER].textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrEquipButton->buttons[MenuButton::HOVER].colour = DirectX::SimpleMath::Vector4(0.75f, 0.75f, 0.75f, 1);

	// Mouse is clicking on the button
	m_ptrEquipButton->buttons[MenuButton::PRESSED].spriteID = 0;
	m_ptrEquipButton->buttons[MenuButton::PRESSED].textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrEquipButton->buttons[MenuButton::PRESSED].colour = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1);

	// Button cannot be interacted with
	m_ptrEquipButton->buttons[MenuButton::DISABLED].spriteID = 0;
	m_ptrEquipButton->buttons[MenuButton::DISABLED].textureName = TxtrNames::SHOP_CONTAINER_NAME;
	m_ptrEquipButton->buttons[MenuButton::DISABLED].colour = DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1);

	// Link an event handler to the button
	MenuManager::Handler hEvent{ [this](MenuNode& _node, MenuNode::Event _e) { ItemShopMode::Get().EquipWeaponUIEvent(_node, _e); } };
	_menuMgr.AddEventHandler("menu_item_shop_UI", m_ptrEquipButton->m_nodeName, MenuNode::Event::CLICK, hEvent);
}
