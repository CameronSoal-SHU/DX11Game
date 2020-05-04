#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <d3d11.h>

#include "D3DUtil.h"
#include "SpriteBatch.h"

// Explicit naming for append path argument
// in loading textures
#define APPEND_PATH true
#define STRETCHED true
#define NO_APPEND_PATH false
#define NOT_STRETCHED false

struct RECTF {
	float left, top, right, bottom;

	RECTF()
		: left(0), top(0), right(0), bottom(0)
	{};

	RECTF(float _l, float _t, float _r, float _b)
		: left(_l), top(_t), right(_r), bottom(_b)
	{};

	RECTF(const RECT& _rect)
		: left((float)_rect.left), top((float)_rect.top),
		right((float)_rect.right), bottom((float)_rect.bottom)
	{};

	operator RECT() {
		return RECT{ (int)left, (int)top, (int)right, (int)bottom };
	}

	RECTF operator+(const RECTF& _rect) {
		left += _rect.left;
		top += _rect.top;
		right += _rect.right;
		bottom += _rect.bottom;

		return (*this);
	}

	RECTF operator-(const RECTF& _rect) {
		return (*this) + RECTF{ -_rect.left, -_rect.top, -_rect.right, -_rect.bottom };
	}
};

class TextureCache
{
public:
	struct Data {
		struct Sprite {
			DirectX::SimpleMath::Vector2 corner;
			bool stretched = NOT_STRETCHED;
			RECT dimensions;
		};

		std::string filePath;
		ID3D11ShaderResourceView* ptrTexture = nullptr;
		DirectX::SimpleMath::Vector2 dimensions;
		std::vector<Sprite> frames;

		/*
		<param "_fName"> File name for texture </param>
		<param "_ptrTxtr"> Pointer to texture resource </param>
		<param "_dim"> Dimensions of texture </param>
		<param "_frames"> Frames of the sprite for animation </param>
		*/
		Data(const std::string& _filePath, ID3D11ShaderResourceView* _ptrTxtr, const DirectX::SimpleMath::Vector2& _dim)
			: filePath(_filePath), ptrTexture(_ptrTxtr), dimensions(_dim) {
			frames.clear();
		}
		Data(const std::string& _filePath, ID3D11ShaderResourceView* _ptrTxtr, const DirectX::SimpleMath::Vector2& _dim, const std::vector<Sprite>* _frames)
			: filePath(_filePath), ptrTexture(_ptrTxtr), dimensions(_dim) {
			if (_frames != nullptr)
				frames = *_frames;
		}

		const Sprite& GetSprite(int _id) const {
			assert(_id >= 0 && _id < (int)frames.size());

			return frames[_id];
		}
	};

	// Garbage collection
	void Release();

	// If this texture is new load it in, otherwise find it and return a handle
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device* _ptrDevice, const std::string& _filePath,
		const std::string& _textureName = "", bool _appendPath = APPEND_PATH, const std::vector<Data::Sprite>* _frames = nullptr);

	static void SetAssetPath(const std::string& _newPath);
	static std::string GetAssetPath() { return m_assetPath; }

	// Retrieve texture via name (FAST)
	const Data& GetData(const std::string& _textureName);
	// Retrieve texture via handle (SLOW)
	const Data& GetData(ID3D11ShaderResourceView* _ptrTexture);

	void Render(DirectX::SpriteBatch& _sprBatch, const std::string& _txtrName, const RECT& _dest, const int _sprID,
		const DirectX::SimpleMath::Vector4& _colour = DirectX::SimpleMath::Vector4(1, 1, 1, 1), float _angle = 0.f, float _depth = 0.f);
private:
	DirectX::SimpleMath::Vector2 GetDimensions(ID3D11ShaderResourceView* _ptrTexture);

	// Map to store texture name and data
	typedef std::unordered_map<std::string, Data> TextureDataMap;
	TextureDataMap m_textureCache;

	// Some data sub folder with all the textures in
	static std::string m_assetPath;
};

