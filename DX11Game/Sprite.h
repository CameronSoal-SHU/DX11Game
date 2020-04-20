#pragma once
#include "SpriteBatch.h"
#include "Animator.h"
#include "D3D.h"


class Sprite
{
public:
	Sprite(D3D& _d3d);
	Sprite(const Sprite& _sprite);
	Sprite& operator=(const Sprite& _sprite);

	void Update();
	void Draw(DirectX::SpriteBatch& _sprBatch);

	RECTF GetTextureRect();
	const TextureCache::Data& GetTextureData() const;
	ID3D11ShaderResourceView& GetTexture();

	DirectX::SimpleMath::Vector2 GetDimensions() const;
	DirectX::SimpleMath::Vector2 GetDimRadius() const;
	// Returns the dimensions of a sprite scaled to screen size
	DirectX::SimpleMath::Vector2 GetScreenDimensions() const;
	DirectX::SimpleMath::Vector2 GetScreenDimRadius() const;

	// SLOW method
	void SetTexture(ID3D11ShaderResourceView& _txtrFilePath, const RECTF& _textureRect = RECTF{ 0,0,0,0 });
	// FAST method
	void SetTexture(const std::string& _txtrName, ID3D11ShaderResourceView& _txtrFilePath, const RECTF& _textureRect = RECTF{ 0,0,0,0 });

	DirectX::SimpleMath::Vector2 GetOrigin() const;
	void SetOrigin(const DirectX::SimpleMath::Vector2& _origin);

	DirectX::SimpleMath::Vector2 GetPos() const;
	void SetPos(const DirectX::SimpleMath::Vector2& _pos);

	DirectX::SimpleMath::Vector2 GetScale() const;
	void SetScale(const DirectX::SimpleMath::Vector2& _scale);

	DirectX::SimpleMath::Vector2 GetVelocity() const;
	void SetVelocity(const DirectX::SimpleMath::Vector2& _vel);

	DirectX::SimpleMath::Vector4 GetColour() const;
	void SetColour(const DirectX::SimpleMath::Vector4& _colour);

	float GetDepth() const;
	void SetDepth(float _depth);

	float GetRotation() const;
	void SetRotation(float _depth);

	Animator& GetAnim();
	void SetTextureRect(const RECT& _textureRect);
	void SetTextureRect(const RECTF& _textureRect);

	void SetFrame(int _idx);
	// Are any of the velocity components not 0?
	bool IsMoving() const;
private:
	ID3D11ShaderResourceView* m_ptrTexture;
	D3D& m_d3d;

	const TextureCache::Data* m_ptrTextureData;
	RECTF m_textureRect;
	Animator m_animator;

	DirectX::SimpleMath::Vector2 m_origin;
	DirectX::SimpleMath::Vector2 m_position;
	DirectX::SimpleMath::Vector2 m_scale;
	DirectX::SimpleMath::Vector2 m_velocity;
	DirectX::SimpleMath::Vector4 m_colour;
	float m_depth;
	float m_rotation;

	std::string m_textureSrcPath;
	std::string m_txtrCacheName;
};

