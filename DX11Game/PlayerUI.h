#pragma once
#include "MenuManager.h"
#include "GameClock.h"

class PlayerUI : public Singleton<PlayerUI>
{
public:
	// Structs to keep collections of UI components
	// together and organised for easier referencing
	struct HealthBar {
		HealthBar();
		void Update(float _deltaTime);

		MenuImage* ptrHealthBarBG;			// Health bar background
		MenuImage* ptrHealthBarFG;			// Health bar foreground
		MenuText*  ptrHealthBarDisplay;		// Health bar health remaining display
	};

	struct InGameClock {
		InGameClock();
		void Update(float _deltaTime);

		GameClock clock;
		MenuText* ptrClockDisplay;
	};

	// Used to display on-screen FPS
	struct FPSDisplay {
		FPSDisplay();
		void Update(float _deltaTime);

		MenuText* ptrFrameRateDisplay;

		// Delay the updating of the FPS count,
		// as this does not need to be updated at every frame.
		float fpsUpdateDelay = 0.f;
		float delayRemaining = 0.f;
	};

	struct HotBar {
		const int SLOT_COUNT = 2;
		enum SlotType { PRIMARY = 0, SECONDARY = 1 };

		struct HotBarSlot {
			HotBarSlot(int _slotNum);

			MenuText*  ptrHotBarCtrl;		// Which button is bound to this hot bar item?
			MenuImage* ptrHotBarSlotSpr;	// Pointer to store hot bar slot icon
			MenuImage* ptrHotBarItemSpr;	// Pointer to store the currently equipped item
		};

		HotBar();

		void Init();
		void Update(float _deltaTime);
		void UpdateHotBarItems();			// Update item slots to be whatever the player is using at that frame

		MenuImage* ptrHotBarBG;
		std::vector<HotBarSlot> hotBarSlots;
	};

	PlayerUI();
	~PlayerUI();

	void Update(float _deltaTime);
private:
	MenuManager& m_menuMgr;			// Holds the menu manager reference
	D3D&		 m_d3d;				// Holds reference to MainGames d3d

	MenuNode*	 m_ptrUIRoot;		// Holds root menu node for player UI

	FPSDisplay*  m_ptrFPSDisplay;	// On-screen Frames per second
	HealthBar*   m_ptrHealthBar;	// Players health bar
	InGameClock* m_ptrGameClock;	// In-game clock of current play session
	HotBar*		 m_ptrItemHotBar;	// Item hot bar to show player items
};

