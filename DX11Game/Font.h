#pragma once
#include "D3D.h"
#include "WindowUtil.h"

namespace DirectX { class SpriteFont; }

struct Font
{
	void Release();
	bool Load();

	std::string m_name;
	std::wstring m_fileName;
	DirectX::SpriteFont* m_ptrFont = nullptr;
	int m_pitch = 0;
};

