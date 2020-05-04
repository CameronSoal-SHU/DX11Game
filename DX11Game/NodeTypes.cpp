#include "NodeTypes.h"
#include "Input.h"
#include "SpriteFont.h"
#include "MenuManager.h"
#include "MainGame.h"

MenuPage::MenuPage()
	: MenuNode(MenuNode::Type::ROOT)
{}

void MenuPage::Render(const ExtraData & _renData,
	const DirectX::SimpleMath::Vector2 & _offset, const DirectX::SimpleMath::Vector2 & _scale) {
	DirectX::SimpleMath::Vector2 pos(_offset.x + m_x * _scale.x, _offset.y + m_y * _scale.y);
	MenuNode::Render(_renData, _offset, _scale);
}

/****************************************************************************************************/

MenuImage::MenuImage()
	: MenuNode(MenuNode::Type::IMAGE)
{}

void MenuImage::Render(const ExtraData & _renData, 
	const DirectX::SimpleMath::Vector2 & _offset, const DirectX::SimpleMath::Vector2 & _scale) {
	// Get sprite and data
	const TextureCache::Data& txtrData = _renData.textureCache.GetData(m_textureName);
	const TextureCache::Data::Sprite* sprite = &txtrData.GetSprite(m_frameID);

	RECT dst;	// Destination of render
	GetImageDest(dst, sprite, _offset, _scale);

	_renData.textureCache.Render(_renData.spriteBatch, m_textureName, dst, m_frameID);
	MenuNode::Render(_renData, DirectX::SimpleMath::Vector2((float)dst.left, (float)dst.top), _scale);
}

/***************************************************************************************************/

MenuButton::MenuButton()
	: MenuNode(MenuNode::Type::BUTTON)
{}

void MenuButton::Render(const ExtraData & _renData, 
	const DirectX::SimpleMath::Vector2 & _offset, const DirectX::SimpleMath::Vector2 & _scale) {
	// Button position
	const TextureCache::Data& txtrData = _renData.textureCache.GetData(buttons[NORMAL].textureName);
	const TextureCache::Data::Sprite* sprite = nullptr;
	// Scaling the interactable area to the games current screen size scale
	const DirectX::SimpleMath::Vector2 gameDimScale = { MainGame::Get().GetScreenDimScaleX(), MainGame::Get().GetScreenDimScaleY() };

	if (!txtrData.frames.empty() && (int)txtrData.frames.size() > buttons[NORMAL].spriteID)
		sprite = &txtrData.GetSprite(buttons[NORMAL].spriteID);

	RECT dst;
	GetImageDest(dst, sprite, _offset, _scale);

	// Check where the mouse is and what it's doing
	DirectX::SimpleMath::Vector2 mPos = _renData.input.GetMousePos(true) * gameDimScale;
	bool overlapping = (mPos.x >= dst.left && mPos.x <= dst.right && mPos.y >= dst.top && mPos.y <= dst.bottom);

	if (m_enabled) {
		if (overlapping) {
			if (_renData.input.GetMouseDown(Input::LMB)) {
				// Mouse click event
				_renData.textureCache.Render(_renData.spriteBatch, buttons[PRESSED].textureName,
					dst, buttons[PRESSED].spriteID, buttons[PRESSED].colour);

				m_wasDown = true;
			}
			else {
				// Mouse hovering event
				_renData.textureCache.Render(_renData.spriteBatch, buttons[HOVER].textureName,
					dst, buttons[HOVER].spriteID, buttons[HOVER].colour);

				if (m_wasDown)
					_renData.menuManager.TriggerEvent(*this, MenuNode::Event::CLICK);

				m_wasDown = false;
			}
		}
		else {
			// Normal event
			_renData.textureCache.Render(_renData.spriteBatch, buttons[NORMAL].textureName,
				dst, buttons[NORMAL].spriteID, buttons[NORMAL].colour);

			m_wasDown = false;
		}
	}
	else {
		// Button is disabled and cannot be interacted with
		_renData.textureCache.Render(_renData.spriteBatch, buttons[DISABLED].textureName,
			dst, buttons[DISABLED].spriteID, buttons[DISABLED].colour);

		m_wasDown = false;
	}
	

	MenuNode::Render(_renData, DirectX::SimpleMath::Vector2((float)dst.left, (float)dst.top), _scale);
}

MenuText::MenuText()
	: MenuNode(MenuNode::Type::TEXT)
{}

DirectX::SimpleMath::Vector2 MenuText::Render(const TextData & _tData) {
	RECT dst;
	MenuNode::GetImageDest(dst, _tData.pos, _tData.dim, nullptr, _tData.offset, _tData.scale);
	DirectX::SimpleMath::Vector2 newPos((float)dst.left, (float)dst.top);

	// Scale text to fit the rectangle whilst maintaining Aspect Ratio
	const DirectX::SpriteFont& font = _tData.ptrRenData->menuManager.GetFont(*_tData.ptrFontName, _tData.pitch);
	DirectX::SimpleMath::Vector2 actual = font.MeasureString(_tData.ptrText->c_str());
	DirectX::SimpleMath::Vector2 desired((float)dst.right - (float)dst.left, (float)dst.bottom - (float)dst.top);
	DirectX::SimpleMath::Vector2 scaled(desired.x / actual.x, desired.y / actual.y);

	if (scaled.x < scaled.y) {
		scaled.y = scaled.x;
	}
	else if (scaled.x > scaled.y) {
		scaled.x = scaled.y;
	}

	font.DrawString(&_tData.ptrRenData->spriteBatch, _tData.ptrText->c_str(), newPos, 
		DirectX::Colors::White, 0.f, DirectX::SimpleMath::Vector2(0, 0), scaled, 
		DirectX::SpriteEffects_None, _tData.layer);

	return newPos;
}

void MenuText::Render(const ExtraData & _renData,
	const DirectX::SimpleMath::Vector2 & _offset, const DirectX::SimpleMath::Vector2 & _scale) {
	DirectX::SimpleMath::Vector2 pos = Render(TextData{
		DirectX::SimpleMath::Vector2((float)m_x, (float)m_y),
		DirectX::SimpleMath::Vector2((float)m_width, (float)m_height), _offset, _scale,
		&_renData, &m_font, &m_text, m_pitch, MenuNode::ZDEPTH_UILAYER });

	MenuNode::Render(_renData, pos, _scale);
}
