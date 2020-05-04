#include "MenuManager.h"
#include "MenuNode.h"

const float MenuNode::ZDEPTH_UILAYER = 0.01f;

MenuNode::MenuNode(Type _type)
	: m_nodeType(_type)
{}

void MenuNode::Render(const ExtraData & _renData, const DirectX::SimpleMath::Vector2 & _offset, 
	const DirectX::SimpleMath::Vector2 & _scale) {
	for (MenuNode* node : m_childNodes)
		node->Render(_renData, _offset, _scale);
}

MenuNode & MenuNode::CreateNode(Type _type) {
	MenuNode* ptrNode = nullptr;

	switch (_type)
	{
	case MenuNode::Type::ROOT:
		ptrNode = new MenuPage();
		break;
	case MenuNode::Type::IMAGE:
		ptrNode = new MenuImage();
		break;
	case MenuNode::Type::BUTTON:
		ptrNode = new MenuButton();
		break;
	case MenuNode::Type::TEXT:
		ptrNode = new MenuText();
		break;
	default:
		break;
	}

	return *ptrNode;
}

MenuNode & MenuNode::CreateNode(const std::string & _type) {
	MenuNode* ptrNode = nullptr;

	if (_type == "Image")
		return CreateNode(Type::IMAGE);
	if (_type == "Button")
		return CreateNode(Type::BUTTON);
	if (_type == "Text")
		return CreateNode(Type::TEXT);

	if (_type != "Menu") {
		DBOUT("Couldn't make Menu Node of Type: " << _type);
		assert(false);
	}

	return CreateNode(Type::ROOT);
}

MenuNode * MenuNode::GetRoot() {
	if (m_nodeType == Type::ROOT)
		return this;
	assert(m_ptrParentNode);			// Make sure there is a parent node

	return m_ptrParentNode->GetRoot();	// Recursively go back up node tree until root is found
}

MenuNode* MenuNode::FindNode(const std::string & _name, MenuNode * _ptrIgnore) {
	if (this != _ptrIgnore && m_nodeName == _name) {
		return this;
	}

	for (MenuNode* child : m_childNodes) {
		// Recursively search each node of tree until the node is found
		MenuNode* ptrNode = child->FindNode(_name);
		// Has the node been found?
		if (ptrNode)
			return ptrNode;
	}

	// No node found
	return nullptr;
}

void MenuNode::AddChild(MenuNode & _child) {
	// Make sure node does not exist already
	assert(!GetRoot()->FindNode(_child.m_nodeName));

	m_childNodes.push_back(&_child);
}

void MenuNode::SetParent(MenuNode & _parent) {
	m_ptrParentNode = &_parent;
	m_ptrParentNode->m_childNodes.push_back(this);
}

void MenuNode::GetImageDest(RECT& _dst, const TextureCache::Data::Sprite* _ptrSpr,
	const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) {
	// If there's no sprite, the destination size is predetermined
	DirectX::SimpleMath::Vector2 dim((float)m_width, (float)m_height);

	if (_ptrSpr) {
		// Set destination size to match sprite
		DirectX::SimpleMath::Vector2 sprDim = DirectX::SimpleMath::Vector2(
			(float)(_ptrSpr->dimensions.right - _ptrSpr->dimensions.left), 
			(float)(_ptrSpr->dimensions.bottom - _ptrSpr->dimensions.top));

		// Maintain aspect ratio?
		if (m_width == -1) {
			if (m_height == -1) {
				dim = sprDim;
			}
			else {
				dim.x = m_height * (sprDim.x / sprDim.y);
			}
		}
		else if (m_height == -1) {
			dim.y = m_width * (sprDim.y / sprDim.y);
		}
	}
	else {
		assert(m_width >= 0 && m_height >= 0);
	}

	// Commit destination
	_dst.left = (LONG)round(_offset.x + m_x * _scale.x);
	_dst.top = (LONG)round(_offset.y + m_y * _scale.y);
	_dst.right = (LONG)round(_dst.left + dim.x * _scale.x);
	_dst.bottom = (LONG)round(_dst.top + dim.y * _scale.y);
}

void MenuNode::GetImageDest(RECT& _dst, const DirectX::SimpleMath::Vector2& _nodePos, 
	const DirectX::SimpleMath::Vector2& _nodeDim, const TextureCache::Data::Sprite* _ptrSpr, 
	const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) {
	// If there is no sprite, then destination size is predetermined
	DirectX::SimpleMath::Vector2 dim(_nodeDim);

	if (_ptrSpr) {
		// Set destination size to match sprite
		DirectX::SimpleMath::Vector2 sprDim(
			(float)(_ptrSpr->dimensions.right - _ptrSpr->dimensions.left),
			(float)(_ptrSpr->dimensions.bottom - _ptrSpr->dimensions.top)
		);

		// Maintain aspect ratio?
		if (_nodeDim.x == -1) {
			if (_nodeDim.y == -1) {
				dim = sprDim;
			}
			else {
				dim.x = _nodeDim.y * (sprDim.x / sprDim.y);
			}
		}
		else if (_nodeDim.y == -1) {
			dim.y = _nodeDim.x * (sprDim.y / sprDim.x);
		}
	}
	else {
		assert(_nodeDim.x >= 0 && _nodeDim.y >= 0);
	}

	// Commit destination
	_dst.left = (LONG)round(_offset.x + _nodePos.x * _scale.x);
	_dst.top = (LONG)round(_offset.y + _nodePos.y * _scale.y);
	_dst.right = (LONG)round(_dst.left + dim.x * _scale.x);
	_dst.bottom = (LONG)round(_dst.top + dim.y * _scale.y);
}
