#pragma once
#include "ModeBase.h"
#include <memory>

/*
Store, organise and switch mode instances
*/
class ModeManager
{
public:
	~ModeManager();

	// Changes current GameMode
	void ChangeMode(const std::string& _newMode);

	void Update(float _dTime);
	void Render(float _dTime, DirectX::SpriteBatch& _sprBatch);
	// Dynamically add a new GameMode
	void AddMode(std::shared_ptr<ModeBase> _modePtr);
	// Free up all GameMode instances, can be explicitly called, or called in destructor
	void Release();
private:
	bool CurIndexValid();
	std::vector<std::shared_ptr<ModeBase>> m_modes;	// GameMode container
	int m_curModeIdx = -1;			// Currently active GameMode
	int m_desModeIdx = -1;			// Wanted GameMode to be active
};

