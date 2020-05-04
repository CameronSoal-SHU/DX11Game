#pragma once
#include "ModeBase.h"
#include "MenuManager.h"
#include "Player.h"
#include "WeaponData.h"

class ItemShopMode : public Singleton<ItemShopMode>, public ModeBase
{
public:
	/*
	A class to hold each weapon available to the player.
	When an item is clicked on, the weapon info should be dumped onto the screen (defined below)
	with the option to buy.
	*/
	class ItemCatagory
	{
	public:
		struct Buyable {
			Buyable(MenuImage& _parent, int _containerID, Weapon* _weap);
			~Buyable();

			void Update();

			MenuButton* itemButton;
			Weapon* linkedWeapon;
		};

		ItemCatagory(int _containerID = 0);
		~ItemCatagory();

		void Update(int _containerID = 0);

		Weapon* GetLinkedWeapon(MenuNode* _node);
		std::vector<Buyable*> GetBuyables() { return m_buyables; }
	private:
		// Containers to hold the weapon options
		MenuText* m_ptrItemCtrHeader;
		MenuImage* m_ptrItemContainer;

		std::vector<Buyable*> m_buyables;	// Items for the player to buy

		void SetupContainer(MenuManager& _menuMgr, int _containerID);
		void SetupHeader(MenuManager& _menuMgr, int _containerID);
		void SetupItems(int _containerID = 0);	// Load items to display in the shop
	};

	/*
	Class to hold a selected weapons info dump to show the player
	the weapon stats and let them equip said weapon
	*/
	class WeapPreview 
	{
	public:
		enum StatIdx {
			WEAP_NAME = 0,
			WEAP_TYPE = 1,
			WEAP_DMG = 2,
			WEAP_ROF = 3,
			WEAP_LIFE_TIME = 4,
			WEAP_PROJ_SPEED = 5,
			WEAP_DESC = 6,
		};

		WeapPreview();

		void Update();

		void DisplayStats(Weapon* _weap);	// Display a given weapons stats on-screen

	private:
		const static int STAT_COUNT = 7;

		MenuImage* m_ptrPreviewContainer;
		MenuText* m_ptrPreviewHeader;
		MenuText* m_ptrPreviewStats[STAT_COUNT];	// This can be an array, because the information shown should never EVER change

		MenuButton* m_ptrEquipButton;

		void SetupPreviewContainer(MenuManager& _menuMgr);
		void SetupPreviewHeader(MenuManager& _menuMgr);
		void SetupPreviewStats(MenuManager& _menuMgr);
		void SetupEquipButton(MenuManager& _menuMgr);
	};

	ItemShopMode(D3D& _d3d);
	~ItemShopMode();

	void Enter() override;
	bool Exit() override;
	// Called once at the very start
	void Start() override;
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	std::string GetModeName() const override { return MODE_NAME; }

	void SetPlayerRef(Player& _player) { m_ptrPlayer = &_player; }
private:
	const static std::string MODE_NAME;
	const static int CATAGORY_COUNT = 2;

	D3D& m_d3d;
	MenuManager& m_menuMgr;			// Holds menu manager reference

	Player* m_ptrPlayer;			// Player reference from MainGame to get and modify stats
	MenuNode* m_ptrUIRoot;			// Holds the menu page for the UI root

	ItemCatagory* m_ptrItemCatagories[CATAGORY_COUNT];
	WeapPreview* m_ptrWeapPreview;

	Sprite m_backgroundSpr;

	void SetupBackground();
	void SetupItemShopUI();
	void CheckPlayerWeapons();	// Is a given weapon already equipped by the player? if so, it shouldn't be purchasable 

	void GetPlayerInput();

	void HandleUIEvent(MenuNode& _node, MenuNode::Event _e);
};

