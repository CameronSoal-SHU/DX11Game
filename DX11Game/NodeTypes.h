#pragma once
#include "MenuNode.h"
#include "TextureCache.h"

// Non graphical root object, one for each distinct menu screen
class MenuPage : public MenuNode {
public:
	MenuPage();

	void Render(const ExtraData& _renData,
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) override;
};

// 2D image
class MenuImage : public MenuNode {
public:
	MenuImage();

	void Render(const ExtraData& _renData,
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) override;

	int m_frameID;
	std::string m_textureName;
};

// Clickable Button with Text
class MenuButton : public MenuNode {
public:
	struct {
		int spriteID = 0;
		std::string textureName;
		DirectX::SimpleMath::Vector4 colour{ 1,1,1,1 };
	} buttons[4];

	enum { PRESSED = 0, NORMAL = 1, HOVER = 2, DISABLED = 3 };

	MenuButton();

	void Render(const ExtraData& _renData,
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) override;

	void SetEnabled(bool _enabled) { m_enabled = _enabled; }
	bool IsEnabled() const { return m_enabled; }

	std::string m_text;
private:
	bool m_wasDown = false;
	bool m_enabled = true;	// Can the button be interacted with?
};

// Single or Multi-lined Text
class MenuText : public MenuNode {
public:
	struct TextData {
		DirectX::SimpleMath::Vector2 pos;		// Position
		DirectX::SimpleMath::Vector2 dim;		// Dimension
		DirectX::SimpleMath::Vector2 offset;
		DirectX::SimpleMath::Vector2 scale;
		const ExtraData* ptrRenData;			// Render data
		const std::string* ptrFontName;
		const std::string* ptrText;
		int pitch;
		float layer = 1.f;
	};

	MenuText();

	static DirectX::SimpleMath::Vector2 Render(const TextData& _tData);
	void Render(const ExtraData& _renData,
		const DirectX::SimpleMath::Vector2& _offset, const DirectX::SimpleMath::Vector2& _scale) override;

	std::string m_text;
	std::string m_font;
	int m_pitch;
};