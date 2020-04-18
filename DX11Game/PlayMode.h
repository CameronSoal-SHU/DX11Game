#pragma once
#include "ModeBase.h"
#include "Input.h"
#include "Player.h"

#include <vector>

class MainGame;

class PlayMode : public ModeBase
{
public:
	static const std::string MODE_NAME;

	PlayMode();
	~PlayMode();
	// Called once at the very start
	void Start();
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	std::string GetModeName() const override { return MODE_NAME; }

	void AddObj(GameObject* _ptrObj);
	void RemoveObj(GameObject* _ptrObj);
	/*
	Find the first object in container that matches the type and active status
	*/
	GameObject* Find(const std::type_info& _type, bool _isActive);

	/*GameObject GetBox() {
		return m_boxCollisionTest;
	}*/
private:
	// Parallax background layers
	std::vector<Sprite> m_parallaxBGLayers;
	std::vector<GameObject*> m_gameObjs;
	//Player m_player;
	//GameObject m_boxCollisionTest;
};
