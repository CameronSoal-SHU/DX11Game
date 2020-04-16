#pragma once
#include "TextureCache.h"

namespace DirectX { class SpriteBatch; }

class MenuManager;
class NodeTypes;
class Input;

// General info that Menu nodes need access to
struct ExtraData {
	float deltaTime;
	DirectX::SpriteBatch& spriteBatch;
	TextureCache& textureCache;
	Input& input;
	MenuManager& menuManager;

	ExtraData(float _dTime, DirectX::SpriteBatch& _sprBatch,
		TextureCache& _txtrCache, Input& _mouseKeyboard, MenuManager& _menuMgr)
		: deltaTime(_dTime), spriteBatch(_sprBatch), textureCache(_txtrCache),
		input(_mouseKeyboard), menuManager(_menuMgr)
	{};
};

class MenuNode
{
public:
	/*
	Type - Type of menu node
	ROOT - Displays nothing, it's children all the parts of one screen of a UI
	IMAGE - Non-Interactive image, could be a logo, background frame, or some other parts of the UI
	BUTTON - Mouse clickable
	TEXT - Anywhere text is used, incl. a button
	*/
	enum class Type { NONE, ROOT, IMAGE, BUTTON, TEXT };
	enum class Event { NONE, CLICK };	// Mouse click events

	static const float ZDEPTH_UILAYER;	// Default layer all UI sprites render on

	Type m_nodeType = Type::NONE;		// What type of node is this?
	int m_nodeID = -1;					// Unique node ID to distinguish nodes
	std::string m_nodeName;				// Unique node name, possibly duplicated on another page

	int m_x, m_y, m_width, m_height;	// Original dimensions in screen co-ords at default res

	MenuNode(Type _type);
	virtual ~MenuNode();

	virtual void Render(const ExtraData& _renData, 
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) = 0;

	static MenuNode& CreateNode(Type _type);
	static MenuNode& CreateNode(const std::string& _type);

	/*
	Recursive function to get a pointer to the root node
	*/
	MenuNode* GetRoot();

	/*
	Recursive function to search for a node by name, searches in this node and all child nodes
	_ptrIgnore = Node to ignore in search
	*/
	MenuNode* FindNode(const std::string& _name, MenuNode* _ptrIgnore = nullptr);
	/*
	Add a new child node to this node
	*/
	void AddChild(MenuNode& _child);

	/*
	Link this node to a new parent node
	*/
	void SetParent(MenuNode& _parent);

	void GetImageDest(RECT& _dst, const TextureCache::Data::Sprite* _ptrSpr,
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale);
	static void GetImageDest(RECT& _dst, const DirectX::SimpleMath::Vector2& _nodePos, const DirectX::SimpleMath::Vector2& _nodeDim,
		const TextureCache::Data::Sprite* _ptrSpr, const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale);
protected:
	std::vector<MenuNode*> m_childNodes;	// Child nodes
private:
	MenuNode* m_ptrParentNode;				// Parent node
};

