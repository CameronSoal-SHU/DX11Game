#include "PlayMode.h"
#include "GameConstants.h"
#include "MainGame.h"

PlayMode::PlayMode(MainGame& _mainGame)
	: m_mainGame(_mainGame), m_d3d(_mainGame.GetD3D()), m_player(*this), m_boxCollisionTest(_mainGame) {
	// Allocate memory for each background sprite
	for (int i(2); i > 0; --i) {
		m_parallaxBGLayers.push_back(Sprite(m_mainGame.GetD3D()));
	}

	Start();
}

void PlayMode::Start() {
	// Go through each sprite in the background parallax vector
	// And set the texture (back-most first)
	int i(0);

	for (Sprite& bgLayer : m_parallaxBGLayers) {
		std::string filePath = "backgroundLayers/space_background_" + std::to_string(i) + ".dds";
		std::string textureName = "background_" + std::to_string(i);

		bgLayer.SetTexture(textureName, *m_d3d.GetTextureCache().LoadTexture(&m_d3d.GetDevice(),
			filePath, textureName, true));

		bgLayer.SetScale({ 3.5f, 3.5f });

		++i;
	}

	// Set sprite and collider for test object
	m_boxCollisionTest.GetSprite().SetTexture("box", *m_d3d.GetTextureCache().LoadTexture(&m_d3d.GetDevice(),
		"Boss/test.dds", "box", true));
	m_boxCollisionTest.GetSprite().SetPos({ 400, 100 });
	m_boxCollisionTest.GetSprite().SetOrigin(m_boxCollisionTest.GetSprite().GetTextureData().dimensions / 2.f);
	m_boxCollisionTest.GetCollider() = Collider(m_boxCollisionTest.GetSprite().GetPos() + m_boxCollisionTest.GetSprite().GetOrigin(), 
		m_boxCollisionTest.GetSprite().GetScreenDimRadius() * 0.9f);
}

void PlayMode::Update(float _deltaTime) {
	const float scrollSpeed = _deltaTime * 50.f;

	// Scroll each background at different rates(closer = faster) to add a parallax effect
	for (unsigned i(0); i < m_parallaxBGLayers.size(); ++i) {
		Sprite& bgElement = m_parallaxBGLayers.at(i);
		RECTF scrollRect{ 0, scrollSpeed * (i + 1), 0, scrollSpeed * (i + 1) };

		bgElement.SetTextureRect(bgElement.GetTextureRect() - scrollRect);
	}

	m_player.Update(_deltaTime);
	m_boxCollisionTest.Update(_deltaTime);
}

void PlayMode::Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) {
	// Draw each background element back-most first
	for (Sprite& bgLayer : m_parallaxBGLayers) {
		bgLayer.Draw(_sprBatch);
	}

	m_player.Render(_sprBatch);
	m_boxCollisionTest.Render(_sprBatch);
}

MainGame& PlayMode::GetMainGame() {
	return m_mainGame;
}
