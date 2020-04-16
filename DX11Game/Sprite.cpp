#include "Sprite.h"



Sprite::Sprite(D3D& _d3d)
	: m_ptrTexture(nullptr), m_d3d(_d3d), m_animator(*this) {
	m_origin   = { 0.f, 0.f };
	m_position = { 0.f, 0.f };
	m_scale	   = { 1.f, 1.f };
	m_velocity = { 0.f, 0.f };
	m_colour   = { 1.f, 1.f, 1.f, 1.f };
	m_depth	   = 0.f;
	m_rotation = 0.f;
}

Sprite::Sprite(const Sprite& _sprite)
	: m_d3d(_sprite.m_d3d), m_animator(*this) {
	*this = _sprite;
}

RECTF Sprite::GetTextureRect() {
	return m_textureRect;
}

const TextureCache::Data & Sprite::GetTextureData() const {
	assert(m_ptrTextureData);
	return *m_ptrTextureData;
}

ID3D11ShaderResourceView& Sprite::GetTexture() {
	assert(m_ptrTexture);
	return *m_ptrTexture;
}

DirectX::SimpleMath::Vector2 Sprite::GetScreenDimensions() const {
	assert(m_ptrTextureData);
	return (m_scale * m_ptrTextureData->dimensions);
}

DirectX::SimpleMath::Vector2 Sprite::GetScreenDimRadius() const {
	assert(m_ptrTextureData);
	return ((m_scale * m_ptrTextureData->dimensions) * 0.5f);
}

void Sprite::SetTexture(ID3D11ShaderResourceView& _txtrFilePath, const RECTF& _textureRect) {
	m_ptrTexture  = &_txtrFilePath;
	m_textureRect = _textureRect;

	m_ptrTextureData = &m_d3d.GetTextureCache().GetData(m_ptrTexture);

	m_textureSrcPath = m_ptrTextureData->filePath;

	if (m_textureRect.left == m_textureRect.right && m_textureRect.top == m_textureRect.bottom) {
		SetTextureRect(RECTF{ 0,0,m_ptrTextureData->dimensions.x, m_ptrTextureData->dimensions.y });
	}
}

void Sprite::SetTexture(const std::string & _txtrName, ID3D11ShaderResourceView& _txtrFilePath, const RECTF & _textureRect) {
	m_ptrTexture = &_txtrFilePath;
	m_textureRect = _textureRect;

	m_ptrTextureData = &m_d3d.GetTextureCache().GetData(_txtrName);

	m_textureSrcPath = m_ptrTextureData->filePath;
	m_txtrCacheName = _txtrName;

	if (m_textureRect.left == m_textureRect.right && m_textureRect.top == m_textureRect.bottom) {
		SetTextureRect(RECTF{ 0,0,m_ptrTextureData->dimensions.x, m_ptrTextureData->dimensions.y });
	}
}

DirectX::SimpleMath::Vector2 Sprite::GetOrigin() const {
	return m_origin;
}

void Sprite::SetOrigin(const DirectX::SimpleMath::Vector2 & _origin) {
	m_origin = _origin;
}

DirectX::SimpleMath::Vector2 Sprite::GetPos() const {
	return m_position;
}

void Sprite::SetPos(const DirectX::SimpleMath::Vector2 & _pos) {
	m_position = _pos;
}

DirectX::SimpleMath::Vector2 Sprite::GetScale() const {
	return m_scale;
}

void Sprite::SetScale(const DirectX::SimpleMath::Vector2 & _scale) {
	m_scale = _scale;
}

DirectX::SimpleMath::Vector2 Sprite::GetVelocity() const {
	return m_velocity;
}

void Sprite::SetVelocity(const DirectX::SimpleMath::Vector2 & _vel) {
	m_velocity = _vel;
}

DirectX::SimpleMath::Vector4 Sprite::GetColour() const {
	return m_colour;
}

void Sprite::SetColour(const DirectX::SimpleMath::Vector4 & _colour) {
	m_colour = _colour;
}

float Sprite::GetDepth() const {
	return m_depth;
}

void Sprite::SetDepth(float _depth) {
	m_depth = _depth;
}

float Sprite::GetRotation() const {
	return m_rotation;
}

void Sprite::SetRotation(float _rot) {
	m_rotation = _rot;
}

Animator& Sprite::GetAnim() {
	return m_animator;
}

void Sprite::SetTextureRect(const RECT& _textureRect) {
	m_textureRect = RECTF(_textureRect);
}

void Sprite::SetTextureRect(const RECTF& _rect) {
	m_textureRect = _rect;
}

void Sprite::SetFrame(int _idx) {
	const TextureCache::Data& data = m_d3d.GetTextureCache().GetData(m_txtrCacheName);
	SetTextureRect(data.GetSprite(_idx).dimensions);
}

Sprite& Sprite::operator=(const Sprite& _sprite) {
	m_origin   = _sprite.m_origin;
	m_position = _sprite.m_position;
	m_scale    = _sprite.m_scale;
	m_velocity = _sprite.m_velocity;
	m_colour   = _sprite.m_colour;
	m_depth	   = _sprite.m_depth;
	m_rotation = _sprite.m_rotation;

	m_textureRect = _sprite.m_textureRect;
	m_ptrTexture  = _sprite.m_ptrTexture;

	return *this;
}

void Sprite::Update() {
	m_position += m_velocity;
}

void Sprite::Draw(DirectX::SpriteBatch & _sprBatch) {
	_sprBatch.Draw(m_ptrTexture, m_position, &(RECT)m_textureRect, m_colour,
		m_rotation, m_origin, m_scale, DirectX::SpriteEffects::SpriteEffects_None, m_depth);
}
