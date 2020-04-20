#include <DDSTextureLoader.h>
#include <filesystem>

#include "TextureCache.h"

std::string TextureCache::m_assetPath;

void TextureCache::Release() {
	// Go through each resource and release
	for (auto& pair : m_textureCache) {
		ReleaseCOM(pair.second.ptrTexture);
	}

	m_textureCache.clear();
}

ID3D11ShaderResourceView* TextureCache::LoadTexture(ID3D11Device* _ptrDevice, const std::string& _filePath, 
	const std::string& _textureName, bool _appendPath, const std::vector<Data::Sprite>* _frames) {

	std::string textureName = _textureName;

	if (textureName.empty()) {
		std::filesystem::path path(_filePath);
		textureName = path.stem().string();
	}

	// Search through the texture cache
	TextureDataMap::iterator it = m_textureCache.find(textureName);
	if (it != m_textureCache.end())
		return (*it).second.ptrTexture;

	// Prepare the path to load
	const std::string* ptrPath = &_filePath;
	std::string fPath;

	if (_appendPath) {
		fPath = m_assetPath + _filePath;
		ptrPath = &fPath;
	}

	std::wstring wstr_fPath(ptrPath->begin(), ptrPath->end());

	// Loading
	DirectX::DDS_ALPHA_MODE alphaMode;
	ID3D11ShaderResourceView* ptrTexture;

	if (DirectX::CreateDDSTextureFromFile(_ptrDevice, wstr_fPath.c_str(), nullptr, &ptrTexture, 0, &alphaMode) != S_OK) {
		DBOUT("Cannot load " << *ptrPath << "\n");
		assert(false);
	}

	// Saving
	assert(ptrTexture);

	m_textureCache.insert(TextureDataMap::value_type(textureName, Data(_filePath, ptrTexture, GetDimensions(ptrTexture), _frames)));

	return ptrTexture;
}

TextureCache::Data& TextureCache::GetData(const std::string& _textureName) {
	return m_textureCache.at(_textureName);
}

const TextureCache::Data& TextureCache::GetData(ID3D11ShaderResourceView* _ptrTexture) {
	TextureDataMap::iterator it = m_textureCache.begin();
	Data* ptrData = nullptr;

	while (it != m_textureCache.end() && !ptrData) {
		if ((*it).second.ptrTexture == _ptrTexture) {
			ptrData = &(*it).second;
		}
		++it;
	}
	assert(ptrData);

	return *ptrData;
}

void TextureCache::Render(DirectX::SpriteBatch & _sprBatch, const std::string & _txtrName, const RECT & _dest, 
	const int _sprID, const DirectX::SimpleMath::Vector4 & _colour, float _angle, float _depth) {
	const Data& txtrData = GetData(_txtrName);
	const Data::Sprite* sprite = &txtrData.GetSprite(_sprID);

	if (!sprite || !sprite->stretched) {
		// if there's no sprite, render the whole texture
		// else, it's a 1 image sprite
		_sprBatch.Draw(txtrData.ptrTexture, _dest, (sprite) ? &sprite->dimensions : nullptr, 
			_colour, _angle, DirectX::SimpleMath::Vector2(0, 0), DirectX::SpriteEffects_None, _depth);
	}
	else {
		// Stretched image
		assert(_angle == 0);

		int widths[] = { sprite->dimensions.left, sprite->dimensions.left + (int)sprite->corner.x,
			sprite->dimensions.right - (int)sprite->corner.x, (int)sprite->dimensions.right };
		int heights[] = { sprite->dimensions.top, sprite->dimensions.top + (int)sprite->corner.y,
			sprite->dimensions.bottom - (int)sprite->corner.y, sprite->dimensions.bottom };

		int destWidths[] = { _dest.left, _dest.left + (int)sprite->corner.x,
			_dest.right - (int)sprite->corner.x, _dest.right };
		int destHeights[] = { _dest.top, _dest.top + (int)sprite->corner.y,
			_dest.bottom - (int)sprite->corner.y, _dest.bottom };

		for (int x(0); x < 3; ++x) {
			for (int y(0); y < 3; ++y) {
				RECT src{ widths[x], heights[y], widths[x + 1], heights[y + 1] };
				RECT dst{ destWidths[x], destHeights[y], destWidths[x + 1], destHeights[x + 1] };

				_sprBatch.Draw(txtrData.ptrTexture, dst, &src, _colour, 0, 
					DirectX::SimpleMath::Vector2(0, 0), DirectX::SpriteEffects_None, _depth);
			}
		}
	}
}

DirectX::SimpleMath::Vector2 TextureCache::GetDimensions(ID3D11ShaderResourceView* _ptrTexture) {
	assert(_ptrTexture);

	ID3D11Resource* ptrResource = nullptr;
	_ptrTexture->GetResource(&ptrResource);

	assert(ptrResource);

	ID3D11Texture2D* texture2d = nullptr;
	HRESULT hRes = ptrResource->QueryInterface(&texture2d);

	DirectX::SimpleMath::Vector2 dimensions(0, 0);

	if (SUCCEEDED(hRes)) {
		D3D11_TEXTURE2D_DESC desc;						// Holds a description of the texture being retrieved

		texture2d->GetDesc(&desc);						// Store the dimensions to be returned
		dimensions.x = static_cast<float>(desc.Width);
		dimensions.y = static_cast<float>(desc.Height);
	}
														// Garbage collection
	ReleaseCOM(texture2d);
	ReleaseCOM(ptrResource);

	return dimensions;
}

void TextureCache::SetAssetPath(const std::string& _newPath) {
	m_assetPath = _newPath;
}