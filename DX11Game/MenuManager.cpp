#include <algorithm>

#include "MenuManager.h"
#include "MenuNode.h"

// Static variable definition
int MenuManager::Handler::m_uniqueID = 0;

MenuManager::~MenuManager() {
	Reset();
}

void MenuManager::Reset() {
	for (MenuNode* node : m_nodes)
		delete node;

	m_nodes.clear();
	m_menus.clear();

	m_ptrActiveMenu = nullptr;
}

MenuNode & MenuManager::AddMenu(const char * _name, int _width, int _height) {
	MenuNode& node = CreateNode(MenuNode::Type::ROOT);

	node.m_nodeName = _name;
	node.m_x = node.m_y = 0;
	node.m_width = _width;
	node.m_height = _height;
	m_menus.push_back(&node);

	return node;
}

void MenuManager::ShowMenu(const std::string& _menuName) {
	std::vector<MenuNode*>::iterator it = std::find_if(m_menus.begin(), m_menus.end(),
		[&_menuName](MenuNode* ptrNode) { return ptrNode->m_nodeName == _menuName; });

	assert(it != m_menus.end() && (*it)->m_nodeType == MenuNode::Type::ROOT);

	m_ptrActiveMenu = dynamic_cast<MenuPage*>(*it);
}

MenuNode & MenuManager::CreateNode(MenuNode::Type _type) {
	MenuNode& node = MenuNode::CreateNode(_type);

	node.m_nodeID = m_nodes.size();
	m_nodes.push_back(&node);

	return node;
}

MenuNode & MenuManager::FindNode(const std::string & _menuName, const std::string & _nodeName) {
	std::vector<MenuNode*>::iterator it = std::find_if(m_nodes.begin(), m_nodes.end(),
		[&_menuName](MenuNode* ptrNode) { 
		return ptrNode->m_nodeType == MenuNode::Type::ROOT && ptrNode->m_nodeName == _menuName; 
	});

	assert(it != m_nodes.end());

	MenuNode* ptrNode = (*it)->FindNode(_nodeName);

	assert(ptrNode);

	return *ptrNode;
}

void MenuManager::TriggerEvent(MenuNode & _node, MenuNode::Event _type)
{}

MenuManager::Handler::Handler() {
	m_id = m_uniqueID++;
}

MenuManager::Handler::Handler(std::function<void(MenuNode&, MenuNode::Event)> _func)
	: func(_func) {
	m_id = m_uniqueID++;
}

int MenuManager::Handler::GetID() const {
	return m_id;
}

void MenuManager::AddEventHandler(const std::string & _menuName, 
	const std::string & _nodeName, MenuNode::Event _eType, Handler _func) {
	MenuNode* ptrNode = &FindNode(_menuName, _nodeName);

	// Is there an event handler already?
	// Use a lambda to check if event handler already exists
	std::vector<Event>::iterator eIt = std::find_if(m_eventHandlers.begin(), m_eventHandlers.end(),
		[_eType, ptrNode](Event& _event) { return _event.eventType == _eType && _event.ptrNode == ptrNode; });

	// Was an event handler found?
	if (eIt != m_eventHandlers.end())	// Found
		(*eIt).funcs.push_back(_func);
	else                                // No event handler found
		m_eventHandlers.push_back(Event(ptrNode, _eType, _func));
}

void MenuManager::RemoveEventHandler(const std::string & _menuName, 
	const std::string & _nodeName, MenuNode::Event _eType, Handler _func) {
	MenuNode* ptrNode = &FindNode(_menuName, _nodeName);

	// Look for an event handler
	std::vector<Event>::iterator eIt = std::find_if(m_eventHandlers.begin(), m_eventHandlers.end(),
		[_eType, ptrNode](Event& _event) { return _event.eventType == _eType && _event.ptrNode == ptrNode; });

	size_t funcSize = (*eIt).funcs.size();

	// Erase function where unique id matches
	(*eIt).funcs.erase(std::find_if((*eIt).funcs.begin(), (*eIt).funcs.end(),
		[&_func](Handler& handler) { return handler.GetID() == _func.GetID(); }));

	assert(funcSize > (*eIt).funcs.size());
}

const DirectX::SpriteFont& MenuManager::GetFont(const std::string & _name, int _pitch) const {
	std::vector<Font>::const_iterator it = std::find_if(m_fontCache.begin(), m_fontCache.end(),
		[&_name, _pitch](const Font& _font) { return _font.m_name == _name && _pitch == _font.m_pitch; });

	assert(it != m_fontCache.end());
	return *((*it).m_ptrFont);
}

bool MenuManager::LoadFont(const std::wstring & _filePath, 
	const std::string & _name, int _pitch) {
	m_fontCache.push_back(Font{ _name, _filePath, nullptr, _pitch });

	return m_fontCache.back().Load();
}

MenuManager::Event::Event(MenuNode * _ptrNode, MenuNode::Event _type, Handler _func)
	: ptrNode(_ptrNode), eventType(_type) {
	funcs.push_back(_func);
};