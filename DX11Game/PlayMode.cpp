#include "PlayMode.h"
#include "GameConstants.h"
#include "MainGame.h"

#include <algorithm>

const std::string PlayMode::MODE_NAME = "PLAY";

PlayMode::PlayMode() {
	// Allocate memory for each background sprite
	for (int i(2); i > 0; --i) {
		m_parallaxBGLayers.push_back(Sprite(MainGame::Get().GetD3D()));
	}

	m_gameObjs.reserve(1000);

	Player* player = new Player();
	player->SetMode(*this);
	player->SetActive(true);
	AddObj(player);
	Start();
}

PlayMode::~PlayMode() {
	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		delete m_gameObjs.at(i);
	}

	m_gameObjs.clear();
}

void PlayMode::Start() {
	// Go through each sprite in the background parallax vector
	// And set the texture (back-most first)
	D3D& d3d = MainGame::Get().GetD3D();
	int i(0);

	for (Sprite& bgLayer : m_parallaxBGLayers) {
		std::string filePath = "backgroundLayers/space_background_" + std::to_string(i) + ".dds";
		std::string textureName = "background_" + std::to_string(i);

		bgLayer.SetTexture(textureName, *d3d.GetTextureCache().LoadTexture(&d3d.GetDevice(),
			filePath, textureName, true));

		bgLayer.SetScale({ 3.5f, 3.5f });

		++i;
	}

	// Set sprite and collider for test object
	/*m_boxCollisionTest.GetSprite().SetTexture("box", *m_d3d.GetTextureCache().LoadTexture(&m_d3d.GetDevice(),
		"Boss/test.dds", "box", true));
	m_boxCollisionTest.GetSprite().SetPos({ 400, 100 });
	m_boxCollisionTest.GetSprite().SetOrigin(m_boxCollisionTest.GetSprite().GetTextureData().dimensions / 2.f);
	m_boxCollisionTest.GetCollider() = Collider(m_boxCollisionTest.GetSprite().GetPos() + m_boxCollisionTest.GetSprite().GetOrigin(), 
		m_boxCollisionTest.GetSprite().GetScreenDimRadius() * 0.9f);*/
}

void PlayMode::Update(float _deltaTime) {
	const float scrollSpeed = _deltaTime * 50.f;

	// Scroll each background at different rates(closer = faster) to add a parallax effect
	for (unsigned i(0); i < m_parallaxBGLayers.size(); ++i) {
		Sprite& bgElement = m_parallaxBGLayers.at(i);
		RECTF scrollRect{ 0, scrollSpeed * (i + 1), 0, scrollSpeed * (i + 1) };

		bgElement.SetTextureRect(bgElement.GetTextureRect() - scrollRect);
	}

	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Update(_deltaTime);
	}
}

void PlayMode::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	// Draw each background element back-most first
	for (Sprite& bgLayer : m_parallaxBGLayers) {
		bgLayer.Draw(_sprBatch);
	}

	for (size_t i(0); i < m_gameObjs.size(); ++i) {
		m_gameObjs.at(i)->Render(_deltaTime, _sprBatch);
	}
}

void PlayMode::AddObj(GameObject * _ptrObj) {
	assert(_ptrObj);

	m_gameObjs.push_back(_ptrObj);
}

void PlayMode::RemoveObj(GameObject * _ptrObj) {
	size_t objSize = m_gameObjs.size();
	assert(objSize > 0);	// Make sure there's object that can be deleted

	m_gameObjs.erase(std::remove(m_gameObjs.begin(), m_gameObjs.end(), _ptrObj));
	assert(objSize != m_gameObjs.size());	// Check if the object has been deleted

	delete _ptrObj;
}

GameObject * PlayMode::Find(const std::type_info & _type, bool _isActive) {
	size_t i(0);

	// Search for object in list
	while (i < m_gameObjs.size() && 
		(typeid(*m_gameObjs.at(i)) != _type || m_gameObjs.at(i)->IsActive() != _isActive)) {
		++i;
	}

	// Return the object if anything was found
	return (i < m_gameObjs.size() ? m_gameObjs.at(i) : nullptr);
}
