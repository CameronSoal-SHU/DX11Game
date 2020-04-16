#include "Font.h"
#include "SpriteFont.h"

void Font::Release() {
	delete m_ptrFont;
	m_ptrFont = nullptr;
}

bool Font::Load() {
	assert(!m_fileName.empty() && !m_name.empty() && m_pitch > 0);

	delete m_ptrFont;
	m_ptrFont = new DirectX::SpriteFont(&WindowUtil::Get().GetD3D().GetDevice(), m_fileName.c_str());

	assert(m_ptrFont);

	return m_ptrFont != nullptr;
}
