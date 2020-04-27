#include <string>
#include <vector>
#include "TextureCache.h"


namespace GameConsts {
	const unsigned char KEY_NUMPAD_ENTER { 0xf0 };
	const unsigned char KEY_LALT { 0xf1 };
	const unsigned char KEY_RALT { 0xf2 };

	const unsigned char KEY_0 { 0x30 };
	const unsigned char KEY_1 { 0x31 };
	const unsigned char KEY_2 { 0x32 };
	const unsigned char KEY_3 { 0x33 };
	const unsigned char KEY_4 { 0x34 };
	const unsigned char KEY_5 { 0x35 };
	const unsigned char KEY_6 { 0x36 };
	const unsigned char KEY_7 { 0x37 };
	const unsigned char KEY_8 { 0x38 };
	const unsigned char KEY_9 { 0x39 };

	const unsigned char KEY_A { 0x41 };
	const unsigned char KEY_B { 0x42 };
	const unsigned char KEY_C { 0x43 };
	const unsigned char KEY_D { 0x44 };
	const unsigned char KEY_E { 0x45 };
	const unsigned char KEY_F { 0x46 };
	const unsigned char KEY_G { 0x47 };
	const unsigned char KEY_H { 0x48 };
	const unsigned char KEY_I { 0x49 };
	const unsigned char KEY_J { 0x4a };
	const unsigned char KEY_K { 0x4b };
	const unsigned char KEY_L { 0x4c };
	const unsigned char KEY_M { 0x4d };
	const unsigned char KEY_N { 0x4e };
	const unsigned char KEY_O { 0x4f };
	const unsigned char KEY_P { 0x50 };
	const unsigned char KEY_Q { 0x51 };
	const unsigned char KEY_R { 0x52 };
	const unsigned char KEY_S { 0x53 };
	const unsigned char KEY_T { 0x54 };
	const unsigned char KEY_U { 0x55 };
	const unsigned char KEY_V { 0x56 };
	const unsigned char KEY_W { 0x57 };
	const unsigned char KEY_X { 0x58 };
	const unsigned char KEY_Y { 0x59 };
	const unsigned char KEY_Z { 0x5a };

	const float EPSILON { .00000001f };
}

// Collection of all texture directories for use across entire project
namespace TxtrDirs {
	// Scrolling space groundground and stars
	const std::string SPACE_BG_PATH			= "Backgrounds/BackgroundLayers/space_background_0.dds";
	const std::string SPACE_BG_STARS_PATH	= "Backgrounds/BackgroundLayers/space_background_1.dds";
	const std::string SHOP_BG_PATH			= "Backgrounds/item_shop_bg.dds";	// Item shop background

	const std::string PLAYER_PATH			= "Ship/ship_test.dds";				// Player ship sprite
	const std::string THRUST_PATH			= "Ship/thrust.dds";				// Ship thrust animated sprite

	//const std::string ITEM_HOTBAR_PATH		= "UI/item_hotbar.dds";				// Player UI item hotbar
	const std::string ITEM_HOTBAR_BG_PATH	= "UI/item_hotbar_bg.dds";			// Player UI item hotbar background
	const std::string ITEM_HOTBAR_SLOT_PATH = "UI/item_hotbar_slot.dds";		// Player UI item hotbar slot
	const std::string HEALTH_BAR_BG_PATH	= "UI/health_bar_bg.dds";			// Player UI healthbar background
	const std::string HEALTH_BAR_FG_PATH	= "UI/health_bar_fg.dds";			// Player UI healthbar foreground

	const std::string ENERGY_BALL_PATH		= "Projectiles/energy_ball.dds";	// Energy ball Projectile
}

// Collection of all texture nicknames in TextureCache for use across entire project
namespace TxtrNames {
	const std::string SPACE_BG_NAME		    = "scrolling_space_bg";
	const std::string SPACE_BG_STARS_NAME   = "scrolling_stars_bg";
	const std::string SHOP_BG_NAME			= "item_shop_bg";

	const std::string PLAYER_NAME			= "player_ship";
	const std::string THRUST_NAME			= "ship_thrust";

	//const std::string ITEM_HOTBAR_NAME		= "ui_item_hotbar";
	const std::string ITEM_HOTBAR_BG_MAME   = "ui_item_hotbar_bg";			// Player UI item hotbar background
	const std::string ITEM_HOTBAR_SLOT_NAME = "ui_item_hotbar_slot";		// Player UI item hotbar slot
	const std::string HEALTH_BAR_BG_NAME	= "ui_health_bar_bg";
	const std::string HEALTH_BAR_FG_NAME	= "ui_health_bar_fg";

	const std::string ENERGY_BALL_NAME		= "proj_energy_ball";
}

// Collection of all texture frames in TextureCache for use across entire project
namespace TxtrFrames {
	const std::vector<TextureCache::Data::Sprite> THRUST_TXTR_FRAMES{
		{{0, 0}, NOT_STRETCHED, { 0,  0, 15, 16} },
		{{0, 0}, NOT_STRETCHED, { 16, 0, 31, 16} },
		{{0, 0}, NOT_STRETCHED, { 32, 0, 47, 16} },
		{{0, 0}, NOT_STRETCHED, { 48, 0, 64, 16} },
	};
	const std::vector<TextureCache::Data::Sprite> HEALTH_BAR_FRAMES {
		{{0,0}, NOT_STRETCHED, {0,0,32,32}}
	};
	const std::vector<TextureCache::Data::Sprite> ITEM_HOTBAR_FRAMES {
		{{0,0}, NOT_STRETCHED, {0,0,96,32}},
		{{0,0}, NOT_STRETCHED, {0,0,32,32}},
	};
	const std::vector<TextureCache::Data::Sprite> ENERGY_BALL_FRAMES {
		{{0,0}, NOT_STRETCHED, {0,0,783,775}}
	};
}