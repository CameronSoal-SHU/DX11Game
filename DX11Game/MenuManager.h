#pragma once
#include "Font.h"
#include "SimpleMath.h"
#include "MenuNode.h"
#include "NodeTypes.h"

class MenuPage;


/*
Manager for all UI screens
*/
class MenuManager
{
public:
	~MenuManager();

	// Clean all screens and reset everything
	void Reset();

	/*
	Add a menu screen
	_name - Unique name for menu
	_width, _height - Resolution of the menu screen
	*/
	MenuNode& AddMenu(const char* _name, int _width, int _height);

	// Display a menu by name
	void ShowMenu(const std::string& _menuName);

	// Hide the currently active menu screen
	void HideMenu();

	// Getter for currently shown menu
	MenuPage& GetActiveMenuPage();

	// Create any type of node
	MenuNode& CreateNode(MenuNode::Type _type);

	/*
	Find a node by name
	_menuName - Which menu has the node in its tree
	_nodeName - Name of node
	*/
	MenuNode& FindNode(const std::string& _menuName, const std::string& _nodeName);

	void Render(float _dTime, DirectX::SpriteBatch& _sprBatch, TextureCache& txtrCache, Input& _input);

	void TriggerEvent(MenuNode& _node, MenuNode::Event _type);

	// When something happens on a menu, trigger code to run
	class Handler {
	public:
		// Sets a unique ID for this handler
		Handler();
		/*
		Construct handler with pointer to a function to handle the event
		*/
		Handler(std::function<void(MenuNode&, MenuNode::Event)> _func);

		/*
		Function pointer for handler
		*/
		std::function<void(MenuNode&, MenuNode::Event)> func;

		int GetID() const;
	private:
		int m_id;				// Unique ID for each user interface event to handle
		static int m_uniqueID;	// Used to generate unique IDs
	};

	/*
	Add function for UI event to appropriate menu screen
	_menuName - which screen to add the function to
	_nodeName - which menu node to respond to
	_type - what type of event does this relate to 
	_func - the function pointer to use when the event happens
	*/
	void AddEventHandler(const std::string& _menuName, const std::string& _nodeName, MenuNode::Event _eType, Handler _func);
	void RemoveEventHandler(const std::string& _menuName, const std::string& _nodeName, MenuNode::Event _eType, Handler _func);

	// Retrieve font from font cache by name
	const DirectX::SpriteFont& GetFont(const std::string& _name, int _pitch) const;

	// Load font into font cache 
	bool LoadFont(const std::wstring& _filePath, const std::string& _name, int _pitch);
private:
	std::vector<MenuNode*> m_nodes;			// Menu nodes such as Buttons, Text, etc.
	std::vector<MenuNode*> m_menus;			// Root menu nodes that represent individual pages of UI Screens

	// Event Handler
	struct Event {
		Event(MenuNode* _ptrNode, MenuNode::Event _eType, Handler _func);

		MenuNode* ptrNode;
		MenuNode::Event eventType;
		std::vector<Handler> funcs;
	};

	std::vector<Event> m_eventHandlers;
	MenuPage* m_ptrActiveMenu = nullptr;	// If the menu system is visible, only one menu is active
	std::vector<Font> m_fontCache;			// Font library
};

