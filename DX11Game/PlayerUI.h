#pragma once
#include "Singleton.h"
#include "MenuManager.h"
#include "GameClock.h"

class MainGame;
class PlayMode;

class PlayerUI : public Singleton<PlayerUI>
{
public:
	// Structs to keep collections of UI components
	// together and organised for easier referencing
	struct HealthBar {
		HealthBar();
		void Update(float _deltaTime);

		MenuImage* healthBarBG;			// Health bar background
		MenuImage* healthBarFG;			// Health bar foreground
		MenuText*  healthBarDisplay;	// Health bar health remaining display
	};

	struct InGameClock {
		InGameClock();
		void Update(float _deltaTime);

		GameClock clock;
		MenuText* clockDisplay;
	};

	// Used to display on-screen FPS
	struct FPSDisplay {
		FPSDisplay();
		void Update(float _deltaTime);

		MenuText* frameCountDisplay;

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

		MenuImage* hotBarBG;
		std::vector<HotBarSlot> hotBarSlots;
	};

	PlayerUI(PlayMode& _playMode);
	~PlayerUI();

	void Update(float _deltaTime);
private:
	MenuManager& m_menuManager;		// Holds the menu manager reference
	PlayMode&	 m_playMode;		// Holds the playMode reference
	D3D&		 m_d3d;				// Holds reference to MainGames d3d

	MenuNode*	 m_rootUINode;		// Holds root menu node for player UI

	FPSDisplay*  m_fpsDisplay;		// On-screen Frames per second
	HealthBar*   m_healthBar;		// Players health bar
	InGameClock* m_gameClock;		// In-game clock of current play session
	HotBar*		 m_itemHotBar;		// Item hot bar to show player items
};

