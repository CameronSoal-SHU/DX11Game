#pragma once
#include "ModeBase.h"
#include "Singleton.h"

class MainMenu :public Singleton<MainMenu>, public ModeBase
{
public:
	MainMenu();
	~MainMenu();

	void Start();
	void Enter() override {};
	bool Exit() override { return false; };
	// Called every frame
	void Update(float _deltaTime) override;
	// Called after every frame to render elements
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	std::string GetModeName() const override { return MODE_NAME; };
private:
	static const std::string MODE_NAME;
};

