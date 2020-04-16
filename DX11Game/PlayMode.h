#pragma once
#include "Input.h"
#include "Player.h"

#include <vector>

class MainGame;

class PlayMode
{
public:
	PlayMode(MainGame& _mainGame);
	// Called once at the very start
	void Start();
	// Called every frame
	void Update(float _deltaTime);
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch);

	MainGame& GetMainGame();
	GameObject GetBox() {
		return m_boxCollisionTest;
	}
private:
	MainGame& m_mainGame;
	D3D& m_d3d;
	// Parallax background layers
	std::vector<Sprite> m_parallaxBGLayers;
	Player m_player;
	GameObject m_boxCollisionTest;
};
